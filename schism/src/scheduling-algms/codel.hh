#ifndef DELAY_QUEUE_HH
#define DELAY_QUEUE_HH
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <assert.h>
#include <queue>
#include <limits.h>
#include <list>
#include "timestamp.h"
#include <math.h>
using namespace std;
/* Codel parameter */

class DelayQueue
{
private:
  class DelayedPacket
  {
  public:
    uint64_t entry_time;
    uint64_t release_time;
    string contents;
    uint16_t bytes_earned;

    DelayedPacket( uint64_t s_e, uint64_t s_r, const string & s_c )
      : entry_time( s_e ), release_time( s_r ), contents( s_c ) , bytes_earned(0) {}
  };

  typedef struct {
       DelayedPacket p; 
       bool ok_to_drop;
  } dodeque_result; 

  static const int SERVICE_PACKET_SIZE = 1500;

  uint64_t convert_timestamp( const uint64_t absolute_timestamp ) const { return absolute_timestamp - _base_timestamp; }

  const string _name;

  std::queue< DelayedPacket > _delay;
  std::queue< DelayedPacket > _pdp;

  std::queue< uint64_t > _schedule;

  std::vector< string > _delivered;

  const uint64_t _ms_delay;

  uint64_t _total_bytes;
  uint64_t _used_bytes;

  uint64_t _queued_bytes;
  uint64_t _bin_sec;

  uint64_t _base_timestamp;

  void tick( void );

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
  bool _codel_enabled;
  /* Codel - control rule */
  uint64_t control_law (uint64_t t) { return t + interval/sqrt(count);}
  /* Codel - reqd. deq functions */
  DelayedPacket _pdp_deq();

  /* Main codel routines */
  dodeque_result dodeque ( void );
  DelayedPacket deque ( void ); 
  void drop ( DelayedPacket p);

public:
  DelayQueue( const string & s_name, const uint64_t s_ms_delay, const char *filename, const uint64_t base_timestamp , bool codel_enable );

  int wait_time( void );
  std::vector< string > read( void );
  void write( const string & packet );
};
#endif
