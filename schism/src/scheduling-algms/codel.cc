#include "codel.hh"

CoDel::CoDel( std::queue<Packet> & flow_queue, uint32_t flow_id ) :
  _flow_queue( flow_queue ),
  _flow_id( flow_id ),
  first_above_time( 0 ),
  drop_next( 0 ),
  count( 0 ),
  dropping( false ),
  drop_count( 0 )
{}

CoDel::DelayedPacket CoDel::_codel_deq()
{
  if (!_flow_queue.empty()) {
    DelayedPacket p( _flow_queue.front()._tick, _flow_queue.front()._tick, "legit" );
    _flow_queue.pop();
    return p;
  }
  else {
    DelayedPacket p ( 0,0,"");
    return p;
  }
}

CoDel::dodeque_result CoDel::dodeque( uint64_t now )
{
  dodeque_result r = { _codel_deq(), false };
  if (r.p.contents.size() == 0 ) {
    first_above_time = 0;
  } else {
    uint64_t sojourn_time = now - r.p.release_time;
    if ( sojourn_time < target || _flow_queue.size() < maxpacket ) {
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

CoDel::DelayedPacket CoDel::deque( uint64_t now )
{
  dodeque_result r = dodeque( now );
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
        r = dodeque( now );
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
    r = dodeque( now );
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

void CoDel::drop (DelayedPacket p)
{
  drop_count++;
  fprintf( stderr,"Flow %d :Codel dropped a packet with size %ld, count now at %d \n", _flow_id, p.contents.size(), drop_count );
}
