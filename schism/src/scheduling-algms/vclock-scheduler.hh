#ifndef VCLOCK_SCHEDULER_HH
#define VCLOCK_SCHEDULER_HH

#include "../scheduler.hh"
#include "../slotted-sender.hh"
#include "../packet.hh"

typedef std::tuple<Packet,double> TaggedPacket ;

/* Virtual Clock Scheduler (Lixia Zhang 1989) */
class VClockScheduler : public Scheduler
{
private :
	/* Flow queues */
	uint32_t _num_flows;
	std::vector<std::queue<TaggedPacket>> _flow_queues;
	std::vector<double> _intervals;

	/* Book-keeping for VirtualClock */
	std::vector<double> _avc;

	uint64_t _tick;
public :
	VClockScheduler();
	
	void add_sender( double avg_interval );
	
	Packet get_next_packet();
	
	void tick( uint64_t current_tick, std::vector<Packet> new_pkts );
	
	Packet dequeue( uint32_t flow_id );
	
	void enqueue( Packet p );
};

#endif
