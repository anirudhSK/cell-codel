#ifndef DELAY_QUEUE_HH
#define DELAY_QUEUE_HH
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <assert.h>
#include <queue>
#include <limits.h>
#include <list>
#include <math.h>
#include "timestamp.h"
#include "delayed-packet.hh"

using namespace std;

class DelayQueue {
 public:
  DelayQueue( const string & s_name, const uint64_t s_ms_delay, const char *filename, const uint64_t base_timestamp);
  virtual ~DelayQueue() {}

  int wait_time( void );
  std::vector< string > read( void );
  void write( const string & packet );

 protected:
  /* virtual functions that can be overridden in the derived class */
  virtual void enque(DelayedPacket p);

  virtual DelayedPacket & head(void);

  virtual DelayedPacket deque(void);

  virtual bool empty(void);

  const string _name;
 
  std::queue< DelayedPacket > _pdp;

 private:
  static const int SERVICE_PACKET_SIZE = 1500;

  uint64_t convert_timestamp( const uint64_t absolute_timestamp ) const { return absolute_timestamp - _base_timestamp; }


  std::queue< DelayedPacket > _delay;

  std::queue< uint64_t > _schedule;

  std::vector< string > _delivered;

  const uint64_t _ms_delay;

  uint64_t _total_bytes;
  uint64_t _used_bytes;

  uint64_t _queued_bytes;
  uint64_t _bin_sec;

  uint64_t _base_timestamp;

  void tick( void );
};
#endif
