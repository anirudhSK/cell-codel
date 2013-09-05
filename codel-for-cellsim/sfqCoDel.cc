#include <algorithm>
#include "sfqCoDel.hh"
#include "pkt-classifier.h"

sfqCoDel::sfqCoDel(const string & s_name, const uint64_t s_ms_delay, const char* filename, const uint64_t base_timestamp)
    : DelayQueue(s_name, s_ms_delay, filename, base_timestamp),
      pkt_queues_(),
      bin_credits_(),
      bin_quantums_(),
      active_list_(),
      active_indicator_(),
      first_above_time_(),
      drop_next_(),
      count_(),
      dropping_(),
      drop_count_(),
      bytes_in_queue_(0)
{}

void sfqCoDel::enque(DelayedPacket p) {
  /* Implements pure virtual function Queue::enque() */
  uint64_t bin_id = hash(p);
  enque_packet(p, bin_id);
  if (active_indicator_.at(bin_id) == false) {
    active_indicator_.at(bin_id) = true;
    active_list_.push(bin_id);
    bin_credits_.at(bin_id)=0;
  }
}

DelayedPacket & sfqCoDel::head(void) {
  uint64_t to_sched = next_bin();
  assert (to_sched != (uint64_t) -1);
  assert (!pkt_queues_.at(to_sched).empty());
  return pkt_queues_.at(to_sched).front();
}

DelayedPacket sfqCoDel::deque() {
  DelayedPacket p = DelayedPacket::nullpkt(); /* TODO Check if this is ok */
  uint64_t current_bin = 0;
  /*
     Try dequeuing a until you get a non-null pkt,
     or the queues are drained 
  */
  do {
    if (active_list_.empty()) {
      /* No packets in any bin */
      return DelayedPacket::nullpkt();
    }
    current_bin = next_bin();
    p = codel_deque(current_bin);
  } while (p.contents.size() == 0);

  /* Decrement credits for the bin */
  bin_credits_.at(current_bin) -= p.contents.size();
  return p;
}

bool sfqCoDel::empty(void) const {
  for (auto it = pkt_queues_.begin(); it != pkt_queues_.end(); it++) {
    if (!it->second.empty()) {
      return false;
    }
  }
  return true;
}

uint64_t sfqCoDel::hash(DelayedPacket pkt) const {
  /* TODO: Improve implementation, dumb hash for now. */
  return PktClassifier().get_flow_id(pkt.contents);
}

void sfqCoDel::init_bin(uint64_t bin_id) {
  /* Initialize bin */
  pkt_queues_[bin_id] = std::queue<DelayedPacket>();
  bin_credits_[bin_id] = 0;
  bin_quantums_[bin_id] = MTU_SIZE;
  active_indicator_[bin_id] = false;
  first_above_time_[bin_id] = 0;
  drop_next_[bin_id] = 0;
  count_[bin_id] = 0;
  dropping_[bin_id] = false;
  drop_count_[bin_id] = 0;
  bytes_in_queue_ = 0;
}

void sfqCoDel::remove_bin(uint64_t bin_id) {
  /* Remove bin from active list */
  bin_credits_.at(bin_id) = 0;
  assert(active_list_.front() == bin_id); /* Can't remove anything else */
  active_list_.pop();
  active_indicator_.at(bin_id) = false;
}

uint64_t sfqCoDel::next_bin(void) {
  uint64_t ret = -1;
  while (!active_list_.empty()) {
    uint64_t front_bin = active_list_.front();

    if(pkt_queues_.at(front_bin).empty()) {
      /* Remove empty bins from the list */
      remove_bin(front_bin);
    } else if (bin_credits_.at(front_bin) < 0) {
      /* Replenish credits if they fall to below zero */
      /* Since queue is non-empty reinsert bin */
      remove_bin(front_bin);
      active_indicator_.at(front_bin) = true;
      active_list_.push(front_bin);
      bin_credits_.at(front_bin) += MTU_SIZE;
    } else {
      return front_bin;
    }
  }
  return ret;
}

void sfqCoDel::enque_packet(DelayedPacket p, uint64_t bin_id) {
  if (pkt_queues_.find(bin_id)  != pkt_queues_.end()) {
    pkt_queues_.at(bin_id).push(p);
    bytes_in_queue_ += p.contents.size();
  } else {
    init_bin(bin_id);
    pkt_queues_.at(bin_id).push(p);
    bytes_in_queue_ += p.contents.size();
  }
}

DelayedPacket sfqCoDel::codel_deq_helper(uint64_t bin_id) {
  if (!pkt_queues_.at(bin_id).empty()) {
    DelayedPacket p(pkt_queues_.at(bin_id).front());
    pkt_queues_.at(bin_id).pop();
    bytes_in_queue_ -= p.contents.size();
    return p;
  } else {
    return DelayedPacket::nullpkt(); /* TODO: Ensure this is correct */
  }
}

sfqCoDel::dodeque_result sfqCoDel::codel_dodeque(uint64_t bin_id) {
  uint64_t now=timestamp();
  dodeque_result r = {codel_deq_helper(bin_id), false};
  if (r.p.contents.size() == 0 ) {
    first_above_time_.at(bin_id) = 0;
  } else {
    uint64_t sojourn_time = now - r.p.release_time;
    if ( sojourn_time < target || bytes_in_queue_ < MTU_SIZE || pkt_queues_.at(bin_id).empty() ) {
      // went below so we'll stay below for at least interval
      first_above_time_.at(bin_id) = 0;
    } else {
      if (first_above_time_.at(bin_id) == 0) {
        // just went above from below. if we stay above
        // for at least interval we'll say it's ok to drop
        first_above_time_.at(bin_id) = now + interval;
      } else if (now >= first_above_time_.at(bin_id)) {
        r.ok_to_drop = true;
      }
    }
  }
  return r;
}

DelayedPacket sfqCoDel::codel_deque(uint64_t bin_id) {
  assert(!pkt_queues_.at(bin_id).empty());
  uint64_t now = timestamp();
  dodeque_result r = codel_dodeque(bin_id);
  
  assert (r.p.contents.size() != 0);
  // There has to be a packet here.

  if (dropping_.at(bin_id)) {
    if (! r.ok_to_drop) {
      // sojourn time below target - leave dropping_.at(bin_id) state
      // and send this bin's packet
      dropping_.at(bin_id) = false;
    }

    // It’s time for the next drop. Drop the current packet and dequeue
    // the next.  If the dequeue doesn't take us out of dropping state,
    // schedule the next drop. A large backlog might result in drop
    // rates so high that the next drop should happen now, hence the
    // ‘while’ loop.
    while (now >= drop_next_.at(bin_id) && dropping_.at(bin_id)) {
      drop(r.p, bin_id);
      r = codel_dodeque(bin_id);

      //if drop emptied queue, it gets to be new on next arrival
      // and want to move on to next bin to find a packet to send
      if(r.p.contents.size() == 0) {
        first_above_time_.at(bin_id) = 0;
        remove_bin(bin_id);
      }

      if (! r.ok_to_drop) {
        // leave dropping_.at(bin_id) state
        dropping_.at(bin_id) = false;
      } else {
        // schedule the next drop.
        ++count_.at(bin_id);
        drop_next_.at(bin_id) = control_law(drop_next_.at(bin_id), count_.at(bin_id));
      }
    }

  // If we get here we’re not in dropping state. 'ok_to_drop' means that the
  // sojourn time has been above target for interval so enter dropping state.
  } else if (r.ok_to_drop) {
    drop(r.p, bin_id);
    r = codel_dodeque(bin_id);
    dropping_.at(bin_id) = true;

    //if drop emptied bin's queue, it gets to be new on next arrival
    // and want to move on to next bin to find a packet to send
    if (r.p.contents.size() == 0) {
      first_above_time_.at(bin_id) = 0; // TODO: Why does the ns2 code reset count and drop_next as well?
      remove_bin(bin_id);
    }

    // If min went above target close to when it last went below,
    // assume that the drop rate that controlled the queue on the
    // last cycle is a good starting point to control it now.
    count_.at(bin_id) = (count_.at(bin_id) > 2 && now - drop_next_.at(bin_id) < 8*interval)? count_.at(bin_id) - 2 : 1;
    drop_next_.at(bin_id) = control_law(now, count_.at(bin_id));
  }
  return (r.p);
}

void sfqCoDel::drop(DelayedPacket p, uint64_t bin_id) {
  drop_count_.at(bin_id)++;
  fprintf(stderr,"%s :Codel dropped a packet from bin id %ld with size %ld, count now at %u \n", _name.c_str(), bin_id, p.contents.size(), drop_count_.at(bin_id));
}
