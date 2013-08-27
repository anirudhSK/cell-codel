#ifndef CODEL_HH_
#define CODEL_HH_

#include <string>
#include "delay-queue.hh"
#include "delayed-packet.hh"

class CoDel : public DelayQueue {
 public:
  CoDel(const string & s_name, const uint64_t s_ms_delay, const char* filename, const uint64_t base_timestamp);

  /* Data structures */
  typedef struct {
       DelayedPacket p; 
       bool ok_to_drop;
  } dodeque_result; 

 protected:
  /* override deque from DelayQueue */
  virtual DelayedPacket deque(void) override;

 private:
  /* Codel - specific parameters */
  static const uint64_t    target=5 ;    /* 5   ms as per the spec */
  static const uint64_t  interval=100;   /* 100 ms as per the spec */
  static const uint16_t maxpacket=1500;    /* MTU of the link */

  /* Codel - specific tracker variables */
  uint64_t first_above_time;
  uint64_t drop_next;
  uint32_t count;
  bool     dropping;
  uint32_t _bytes_in_queue;
  uint32_t drop_count; /* for statistics */
  
  /* Codel - control rule */
  uint64_t control_law (uint64_t t) const { return t + interval/sqrt(count);}

  /* Codel - utility functions */
  DelayedPacket _pdp_deq();
  dodeque_result dodeque ( void );
  void drop ( DelayedPacket p);
};

#endif  // CODEL_HH_
