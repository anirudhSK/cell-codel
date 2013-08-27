#include <stdint.h>
#include <float.h>
#include <algorithm>
#include <utility>
#include "las.hh"
#include "pkt-classifier.h"

Las::Las(const string & s_name, const uint64_t s_ms_delay, const char* filename, const uint64_t base_timestamp)
    : DelayQueue(s_name, s_ms_delay, filename, base_timestamp),
      pkt_queues_(std::map<uint64_t, std::queue<DelayedPacket>>()),
      attained_service_(std::map<uint64_t, uint32_t>())
{}

void Las::enque(DelayedPacket p) {
  /* Implements pure virtual function Queue::enque() */
  uint64_t flow_id = hash(p);
  enque_packet(p, flow_id);
  return;
}

DelayedPacket & Las::head(void) {
  assert (pkt_queues_.find(las()) != pkt_queues_.end()); /* The particular flow id exists */
  assert (!pkt_queues_.at(las()).empty());               /* That particular flow queue is non-empty */
  return pkt_queues_.at(las()).front();
}

bool Las::empty(void) const {
  for (auto it = pkt_queues_.begin(); it != pkt_queues_.end(); it++) {
    if (!it->second.empty()) {
      return false;
    }
  }
  return true;
}

DelayedPacket Las::deque() {
  /* Implements pure virtual function Queue::deque() */
  uint64_t chosen_flow = las();
  if ( pkt_queues_.find(chosen_flow) != pkt_queues_.end() ) {
    assert(!pkt_queues_.at(chosen_flow).empty());
    DelayedPacket p = pkt_queues_.at(chosen_flow).front();
    pkt_queues_.at(chosen_flow).pop();
    attained_service_.at(chosen_flow) += p.contents.size();
    if (pkt_queues_.at(chosen_flow).empty()) { /* Queue is empty now */
      attained_service_.at(chosen_flow) = 0; /* reset flow counter */
    }
    return p;
  } else {
    return DelayedPacket::nullpkt();
  }
}

void Las::enque_packet(DelayedPacket p, uint64_t flow_id) {
  if ( pkt_queues_.find(flow_id)  != pkt_queues_.end() ) {
    pkt_queues_.at(flow_id).push(p);
  } else {
    pkt_queues_[flow_id] = std::queue<DelayedPacket>();
    attained_service_[flow_id] = 0; /* New flow, never seen before */
    pkt_queues_.at(flow_id).push(p);
  }
}

uint64_t Las::hash(DelayedPacket pkt) const {
  /* TODO: Improve implementation, dumb hash for now. */
  return PktClassifier().get_flow_id(pkt.contents);
}

uint64_t Las::las() const {
  /* Clone attained_service_ map */
  typedef std::pair<uint64_t, uint32_t> FlowServices;
  std::map<uint64_t,uint32_t> service_clone;

  /* Purge all empty queues from consideration */
  for (auto it = attained_service_.begin(); it != attained_service_.end(); it++) {
    if (!pkt_queues_.at(it->first).empty()) {
      service_clone[it->first] = attained_service_.at(it->first);
    }
  }

  /* Among the non-empty queues, find the one with least sttained service */
  auto flow_compare = [&] (const FlowServices & q1, const FlowServices &q2 )
                      { assert (!pkt_queues_.at(q1.first).empty());
                        assert (!pkt_queues_.at(q2.first).empty());
                        return q1.second < q2.second ; };
  if (service_clone.empty()) {
    return -1;
  } else {
    return std::min_element(service_clone.begin(), service_clone.end(), flow_compare) -> first;
  }
}
