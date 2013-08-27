#include "codel.hh"

CoDel::CoDel(const string & s_name, const uint64_t s_ms_delay, const char* filename, const uint64_t base_timestamp)
    : DelayQueue(s_name, s_ms_delay, filename, base_timestamp),
      first_above_time(0),
      drop_next(0),
      count(0),
      dropping(false),
      _bytes_in_queue(0),
      drop_count(0)
{}

DelayedPacket CoDel::deque() {
  uint64_t now = timestamp();
  dodeque_result r = dodeque();
  if (r.p.contents.size() == 0 ) {
    // an empty queue takes us out of dropping state
    dropping = 0;
    return r.p;
  }
  
  if (dropping) {
    if (! r.ok_to_drop) {
      // sojourn time below target - leave dropping state
      dropping = 0;
    } else if (now >= drop_next) {
      while (now >= drop_next && dropping) {
        drop(r.p);
        ++count;
        r = dodeque();
        if (! r.ok_to_drop)
          // leave dropping state
          dropping = 0;
        else
          // schedule the next drop.
          drop_next = control_law(drop_next);
      }
    }
  } else if (r.ok_to_drop &&
             ((now - drop_next < interval) ||
             (now - first_above_time >= interval))) {
      drop(r.p);
      r = dodeque();
      dropping = 1;
      // If we're in a drop cycle, the drop rate that controlled the queue
      // on the last cycle is a good starting point to control it now.
      if (now - drop_next < interval)
        count = count>2? count-2 : 1;
      else
        count = 1;
      drop_next = control_law(now);
  }
  return (r.p);
}
void CoDel::drop (DelayedPacket p) {
  drop_count++;
  fprintf(stderr,"%s :Codel dropped a packet with size %ld, count now at %d \n",_name.c_str(),p.contents.size(),drop_count);
}

DelayedPacket CoDel::_pdp_deq() {
  if (!_pdp.empty()) {
    _bytes_in_queue-=_pdp.front().contents.size();
    DelayedPacket p(_pdp.front());
    _pdp.pop();
    return p;
  }
  else {
   return DelayedPacket::nullpkt(); /*TODO: Ensure this is correct */
  }
}

CoDel::dodeque_result CoDel::dodeque (void) {
  uint64_t now=timestamp();
  dodeque_result r = { _pdp_deq(), false };
  if (r.p.contents.size() == 0 ) {
    first_above_time = 0;
  } else {
    uint64_t sojourn_time = now - r.p.release_time;
    if (sojourn_time < target || _bytes_in_queue < maxpacket) {
      // went below so we'll stay below for at least interval
      first_above_time = 0;
    } else {
      if (first_above_time == 0) {
        // just went above from below. if we stay above
        // for at least interval we'll say it's ok to drop
        first_above_time = now + interval;
      } else if (now >= first_above_time) {
        r.ok_to_drop = true;
      }
    }
  }
  return r; 
}
