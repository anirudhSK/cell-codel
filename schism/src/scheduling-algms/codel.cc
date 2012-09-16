#include "codel.hh"
DelayQueue::DelayQueue( const string & s_name, const uint64_t s_ms_delay, const char *filename, const uint64_t base_timestamp , bool codel_enable )
  : _name( s_name ),
    _delay(),
    _pdp(),
    _schedule(),
    _delivered(),
    _ms_delay( s_ms_delay ),
    _total_bytes( 0 ),
    _used_bytes( 0 ),
    _queued_bytes( 0 ),
    _bin_sec( base_timestamp / 1000 ),
    _base_timestamp( base_timestamp ),
    first_above_time( 0),
    drop_next(0),
    count(0),
    dropping(false),
    _bytes_in_queue(0) ,
    drop_count(0) ,
    _codel_enabled(codel_enable)
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

  /* move packets from end of delay to PDP */
  while ( (!_delay.empty())
	  && (_delay.front().release_time <= now) ) {
    _pdp.push( _delay.front() );
    _bytes_in_queue+=_delay.front().contents.size(); 
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
      if ( _pdp.empty() ) {
	_total_bytes += bytes_to_play_with;
	bytes_to_play_with = 0;
	/* underflow */
//	fprintf( stderr, "%s %f underflow!\n", _name.c_str(), now / 1000.0 );
      } else {
	DelayedPacket packet = _pdp.front();
	if ( bytes_to_play_with + packet.bytes_earned >= (int)packet.contents.size() ) {
	  /* deliver whole packet */
	  _total_bytes += packet.contents.size();
	  _used_bytes += packet.contents.size();


          /* TODO : Implement codel here */
          if (_codel_enabled) {
            packet=deque();
            if(packet.contents.size() > 0) {
              _delivered.push_back( packet.contents );
              fprintf( stderr, "%s %f delivery %d\n", _name.c_str(), convert_timestamp( now ) / 1000.0, int(now - packet.entry_time) );
            }
          }
          else {
            packet=_pdp.front();
	    _delivered.push_back( packet.contents );
	    fprintf( stderr, "%s %f delivery %d\n", _name.c_str(), convert_timestamp( now ) / 1000.0, int(now - packet.entry_time) );
	    _pdp.pop();
          }
	  bytes_to_play_with -= (packet.contents.size()-packet.bytes_earned);
	} else {
	  /* continue to accumulate credit */
	  assert( bytes_to_play_with + packet.bytes_earned < (int)packet.contents.size() );
          _pdp.front().bytes_earned += bytes_to_play_with;
  //        fprintf( stderr , "%s %f Accumulating credit, current earned credit is %d and pkt size is %ld \n",
  //                        _name.c_str(), convert_timestamp(now)/1000.0,_pdp.front().bytes_earned,_pdp.front().contents.size());
          assert( _pdp.front().bytes_earned < (int)_pdp.front().contents.size() );
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

DelayQueue::DelayedPacket DelayQueue::_pdp_deq()
{
   if (!_pdp.empty()) {
     _bytes_in_queue-=_pdp.front().contents.size();
     DelayedPacket p(_pdp.front());
     _pdp.pop();
     return p;
   }
   else {
    DelayedPacket p ( 0,0,"");
    return p;
   }
}


DelayQueue::dodeque_result DelayQueue::dodeque ( ) 
  {
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


DelayQueue::DelayedPacket DelayQueue::deque( )
{
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
                        /* execute exactly once on limbo queue */
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

void DelayQueue::drop (DelayedPacket p) 
  { 
   drop_count++;
   fprintf(stderr,"%s :Codel dropped a packet with size %ld, count now at %d \n",_name.c_str(),p.contents.size(),drop_count);
  }

