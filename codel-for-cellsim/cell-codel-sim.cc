#include <unistd.h>
#include <string>
#include <assert.h>
#include <stdio.h>

#include "select.h"
#include "packetsocket.hh"
#include "delay-queue.hh"
#include "timestamp.h"
#include "skype-delays.h"

int main( int argc, char *argv[] )
{
  const char *up_filename, *down_filename, *client_mac;

  assert( argc == 5 );

  up_filename = argv[ 1 ];
  down_filename = argv[ 2 ];
  client_mac = argv[ 3 ];
  
  PacketSocket internet_side( "eth0", string(), string( client_mac ) );
  PacketSocket client_side( "eth1", string( client_mac ), string() );

  /* Read in schedule */
  uint64_t now = timestamp();
  DelayQueue uplink( "uplink", 20, up_filename, now );
  DelayQueue downlink( "downlink", 20, down_filename, now );

  Select &sel = Select::get_instance();
  sel.add_fd( internet_side.fd() );
  sel.add_fd( client_side.fd() );

  while ( 1 ) {
    int wait_time = std::min( uplink.wait_time(), downlink.wait_time() );
    int active_fds = sel.select( wait_time );
    if ( active_fds < 0 ) {
      perror( "select" );
      exit( 1 );
    }

    if ( sel.read( client_side.fd() ) ) {
      std::vector< string > filtered_packets( client_side.recv_raw() );
      for ( auto it = filtered_packets.begin(); it != filtered_packets.end(); it++ ) {
        skype_delays( *it, get_absolute_us(), "RECEIVED UPLINKIN ");
	uplink.write( *it );
      }
    }

    if ( sel.read( internet_side.fd() ) ) {
      std::vector< string > filtered_packets( internet_side.recv_raw() );
      for ( auto it = filtered_packets.begin(); it != filtered_packets.end(); it++ ) {
        skype_delays( *it, get_absolute_us(), "RECEIVED DOWNLINKIN ");
	downlink.write( *it );
      }
    }

    std::vector< string > uplink_packets( uplink.read() );
    for ( auto it = uplink_packets.begin(); it != uplink_packets.end(); it++ ) {
      skype_delays( *it, get_absolute_us(), "SENT UPLINKOUT ");
      internet_side.send_raw( *it );
    }

    std::vector< string > downlink_packets( downlink.read() );
    for ( auto it = downlink_packets.begin(); it != downlink_packets.end(); it++ ) {
      skype_delays( *it, get_absolute_us(), "SENT DOWNLINKOUT ");
      client_side.send_raw( *it );
    }
  }
}
