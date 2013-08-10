#ifndef DELAYED_PACKET_HH_
#define DELAYED_PACKET_HH_

#include <string>

class DelayedPacket
{
public:
  uint64_t entry_time;
  uint64_t release_time;
  std::string contents;
  uint16_t bytes_earned;

  DelayedPacket( uint64_t s_e, uint64_t s_r, const std::string & s_c )
    : entry_time( s_e ), release_time( s_r ), contents( s_c ) , bytes_earned(0) {}

  static DelayedPacket nullpkt() { return DelayedPacket( -1, -1, ""); }
};

#endif  // DELAYED_PACKET_HH_
