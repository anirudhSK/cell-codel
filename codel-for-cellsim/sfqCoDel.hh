#ifndef DRR_H_
#define DRR_H_

#include <math.h>
#include <string>
#include <map>
#include <list>
#include <queue>
#include "delay-queue.hh"
#include "delayed-packet.hh"

/* Stochastic Fair Queueing (McKinney 1990) */

class sfqCoDel : public DelayQueue {
 public :
  sfqCoDel(const string & s_name, const uint64_t s_ms_delay, const char* filename, const uint64_t base_timestamp);

 protected:
  /* inherited functions from DelayQueue */
  virtual void enque(DelayedPacket p) override;

  virtual DelayedPacket & head(void) override;

  virtual DelayedPacket deque(void) override;

  virtual bool empty(void) const override;

 private :
  /* Data structures */
  typedef struct {
       DelayedPacket p;
       bool ok_to_drop;
  } dodeque_result;

  /* Hash from packet to flow */
  uint64_t hash(DelayedPacket p) const;
 
  /* sfq - Initialization */
  void init_bin(uint64_t bin_id);

  /* sfq - purging */
  void remove_bin(uint64_t bin_id);

  /* sfq - next flow in cyclic order */
  uint64_t next_bin(void);

  /* sfq - helper functions */
  void enque_packet(DelayedPacket p, uint64_t bin_id);

  /* Codel - control rule */
  uint64_t control_law (uint64_t t, uint32_t count) const { return t + interval/sqrt(count);}

  /* Main codel routines */
  DelayedPacket  codel_deq_helper(uint64_t bin_id);
  dodeque_result    codel_dodeque(uint64_t bin_id);
  DelayedPacket       codel_deque(uint64_t bin_id);
  void drop(DelayedPacket p, uint64_t bin_id);

  /* MTU Size */
  static const uint32_t MTU_SIZE = 1514;

  /* sfqCoDel structures */
  std::map<uint64_t, std::queue<DelayedPacket>> pkt_queues_;
  std::map<uint64_t, int32_t>  bin_credits_;
  std::map<uint64_t, uint32_t> bin_quantums_;
  std::queue<uint64_t> active_list_;
  std::map<uint64_t, bool> active_indicator_;

  /* Codel - specific parameters */
  static const uint64_t    target=5 ;    /* 5   ms as per the spec */
  static const uint64_t  interval=100;   /* 100 ms as per the spec */

  /* Codel - specific tracker variables */
  std::map<uint64_t, uint64_t> first_above_time_;
  std::map<uint64_t, uint64_t> drop_next_;
  std::map<uint64_t, uint32_t> count_;
  std::map<uint64_t, bool    > dropping_;
  std::map<uint64_t, uint32_t> drop_count_; /* for statistics */
  uint32_t bytes_in_queue_; /* Total number of bytes across all queues */
};

#endif  // DRR_H_
