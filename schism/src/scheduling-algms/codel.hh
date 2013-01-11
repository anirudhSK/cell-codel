#ifndef CODEL_HH
#define CODEL_HH
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <assert.h>
#include <queue>
#include <limits.h>
#include <list>
#include <math.h>
#include "../packet.hh"
using namespace std;

class DelayedPacket
  {
  public:
    uint64_t entry_time;
    uint64_t release_time;
    string contents;

    DelayedPacket( uint64_t s_e, uint64_t s_r, const string & s_c )
      : entry_time( s_e ), release_time( s_r ), contents( s_c ) {}
  };

class CoDel
{
private:
  typedef struct {
       DelayedPacket p; 
       bool ok_to_drop;
  } dodeque_result; 

  /* Flow Queue that Codel is tracking */
  std::queue<Packet> _flow_queue;
  uint32_t _flow_id;

  /* Codel - specific parameters */
  static const uint64_t    target=5 ;    /* 5   ms as per the spec */
  static const uint64_t  interval=100;   /* 100 ms as per the spec */
  static const uint16_t maxpacket=1;    /* MTU of the link, 1 in our tick simulator */

  /* Codel - specific tracker variables */
  uint64_t first_above_time;
  uint64_t drop_next;
  uint32_t count;
  bool     dropping;
  uint32_t drop_count; /* for statistics */

  /* Codel - control rule */
  uint64_t control_law (uint64_t t) { return t + interval/sqrt(count);}

  /* Codel - reqd. deq functions */
  DelayedPacket _codel_deq();

  /* Main codel routines */
  dodeque_result dodeque ( uint64_t now );
  DelayedPacket deque ( uint64_t now );
  void drop ( DelayedPacket p);

public:
  CoDel( std::queue<Packet> & flow_queue, uint32_t flow_id );

};
#endif
