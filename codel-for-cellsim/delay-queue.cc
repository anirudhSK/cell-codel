#include "delay-queue.hh"
DelayQueue::DelayQueue( const string & s_name, const uint64_t s_ms_delay, const char *filename, const uint64_t base_timestamp)
  : _name( s_name ),
    _pdp(),
    _delay(),
    _schedule(),
    _delivered(),
    _ms_delay( s_ms_delay ),
    _total_bytes( 0 ),
    _used_bytes( 0 ),
    _queued_bytes( 0 ),
    _bin_sec( base_timestamp / 1000 ),
    _base_timestamp( base_timestamp )
{
  FILE *f = fopen( filename, "r" );
  if ( f == NULL ) {
    perror( "fopen" );
    exit( 1 );
  }

  while ( 1 ) {
    uint64_t ms;
    int num_matched = fscanf( f, "%lu\n", &ms );
    if ( num_matched != 1 ) {
      break;
    }

    ms += base_timestamp;

    if ( !_schedule.empty() ) {
      assert( ms >= _schedule.back() );
    }

    _schedule.push( ms );
  }

  fprintf( stderr, "Initialized %s queue with %d services.\n", filename, (int)_schedule.size() );
}

int DelayQueue::wait_time( void )
{
  int delay_wait = INT_MAX, schedule_wait = INT_MAX;

  uint64_t now = timestamp();

  tick();

  if ( !_delay.empty() ) {
    delay_wait = _delay.front().release_time - now;
    if ( delay_wait < 0 ) {
      delay_wait = 0;
    }
  }

  if ( !_schedule.empty() ) {
    schedule_wait = _schedule.front() - now;
    assert( schedule_wait >= 0 );
  }

  return std::min( delay_wait, schedule_wait );
}

std::vector< string > DelayQueue::read( void )
{
  tick();

  std::vector< string > ret( _delivered );
  _delivered.clear();

  return ret;
}

void DelayQueue::write( const string & packet )
{
  uint64_t now( timestamp() );
  DelayedPacket p( now, now + _ms_delay, packet );
  _delay.push( p );
  _queued_bytes=_queued_bytes+packet.size();
}

void DelayQueue::tick( void )
{
  uint64_t now = timestamp();

  /* If the schedule is empty, end playback */
  if ( _schedule.empty() ) {
    exit( 0 );
  }

  /* move packets from end of delay to PDP */
  while ( (!_delay.empty())
	  && (_delay.front().release_time <= now) ) {
    enque(_delay.front());
   /* track bytes */
    _delay.pop();
  }

  /* execute packet delivery schedule */
  while ( (!_schedule.empty())
	  && (_schedule.front() <= now) ) {
    /* grab a PDO */
    _schedule.pop();
    int bytes_to_play_with = SERVICE_PACKET_SIZE;
    /* execute regular queue */
    while ( bytes_to_play_with > 0 ) {
      /* will this be an underflow? */
      if (empty()) {
	_total_bytes += bytes_to_play_with;
	bytes_to_play_with = 0;
	/* underflow */
//	fprintf( stderr, "%s %f underflow!\n", _name.c_str(), now / 1000.0 );
      } else {
	DelayedPacket packet = head();
	if ( bytes_to_play_with + packet.bytes_earned >= (int)packet.contents.size() ) {
	  /* deliver whole packet */
	  _total_bytes += packet.contents.size();
	  _used_bytes += packet.contents.size();

          packet=deque();
          if(packet.contents.size() > 0) {
            _delivered.push_back( packet.contents );
            fprintf( stderr, "%s %f delivery %d\n", _name.c_str(), convert_timestamp( now ) / 1000.0, int(now - packet.entry_time) );
          }

	  bytes_to_play_with -= (packet.contents.size()-packet.bytes_earned);
	} else {
	  /* continue to accumulate credit */
	  assert( bytes_to_play_with + packet.bytes_earned < (int)packet.contents.size() );
          head().bytes_earned += bytes_to_play_with;
  //        fprintf( stderr , "%s %f Accumulating credit, current earned credit is %d and pkt size is %ld \n",
  //                        _name.c_str(), convert_timestamp(now)/1000.0,_pdp.front().bytes_earned,_pdp.front().contents.size());
          assert( head().bytes_earned < (int)head().contents.size() );
	  bytes_to_play_with = 0;
	}
      }
    }
  }

  while ( now / 1000 > _bin_sec ) {
    fprintf( stderr, "%s %ld %ld / %ld = %.1f %% %ld \n", _name.c_str(), _bin_sec - (_base_timestamp / 1000), _used_bytes, _total_bytes, 100.0 * _used_bytes / (double) _total_bytes , _queued_bytes );
    _total_bytes = 0;
    _used_bytes = 0;
    _queued_bytes = 0;
    _bin_sec++;
  }
}

void DelayQueue::enque(DelayedPacket p) {
  _pdp.push(p);
}

bool DelayQueue::empty() const {
  return  _pdp.empty();
}

DelayedPacket & DelayQueue::head() {
  assert (!_pdp.empty());
  return _pdp.front();
}

DelayedPacket DelayQueue::deque() {
  DelayedPacket packet=_pdp.front();
  _pdp.pop();
  return packet;
}
