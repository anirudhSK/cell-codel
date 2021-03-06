#ifndef QUEUE_LAS_H_
#define QUEUE_LAS_H_

#include <string>
#include <map>
#include <list>
#include <queue>
#include "delay-queue.hh"
#include "delayed-packet.hh"

/*
 * LAS: Least Attained Service
 */

class Las : public DelayQueue {
 public :
  Las(const string & s_name, const uint64_t s_ms_delay, const char* filename, const uint64_t base_timestamp);

 protected:
  /* inherited functions from DelayQueue */
  virtual void enque(DelayedPacket p) override;

  virtual DelayedPacket & head(void) override;

  virtual DelayedPacket deque(void) override;

  virtual bool empty(void) const override;

 private :
  /* Hash from packet to bin */
  uint64_t hash(DelayedPacket p) const;

  /* Pick the bin with the LAS */
  uint64_t las(void) const;

  /* Underlying per bin FIFOs and enque wrapper */
  std::map<uint64_t, std::queue<DelayedPacket>> pkt_queues_;
  void enque_packet(DelayedPacket p, uint64_t bin_id);

  /* Maintain attained service for each bin */
  std::map<uint64_t, uint32_t> attained_service_;
};

#endif  // QUEUE_LAS_H_
