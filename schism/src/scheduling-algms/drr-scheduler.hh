#ifndef DRR_SCHEDULER_HH
#define DRR_SCHEDULER_HH

#include "../slotted-sender.hh"
#include "../packet.hh"
#include "../scheduler.hh"
#include "codel.hh"

/* Deficit Round Robin Scheduler (Shrredhar and Varghese 1995) */
class DRRScheduler : public Scheduler
{
private :
	/* DRR structures */
	uint32_t _num_flows;
	std::vector<std::queue<Packet>> _flow_queues;
	std::vector<double>  _flow_credits;
	std::vector<double>  _flow_quantums;
	std::queue<uint32_t> _active_list;
	std::vector<bool> _active_indicator;
	uint32_t _current_flow;

	/* Optional CoDel servo bank for all flows */
	std::vector<CoDel> _codel_servo_bank;
	bool _codel_enabled;

public :
	DRRScheduler( bool t_codel_enabled );
	
	void add_sender( double weight );
	
	Packet get_next_packet();
	
	void tick( uint64_t current_tick, std::vector<Packet> new_pkts );
	
	Packet dequeue( uint32_t flow_id );
	
	void enqueue( Packet p);
};

#endif
