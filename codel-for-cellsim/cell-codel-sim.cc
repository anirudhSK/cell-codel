#include <unistd.h>
#include <string>
#include <assert.h>
#include <stdio.h>

#include "select.h"
#include "packetsocket.hh"
#include "delay-queue.hh"
#include "timestamp.h"

int main( int argc, char *argv[] )
{
  const char *up_filename, *down_filename, *client_mac, *codel_enable;

  assert( argc == 5 );

  up_filename = argv[ 1 ];
  down_filename = argv[ 2 ];
  client_mac = argv[ 3 ];
  codel_enable = argv[ 4 ];
  
  PacketSocket internet_side( "eth0", string(), string( client_mac ) );
  PacketSocket client_side( "eth1", string( client_mac ), string() );

  /* Read in schedule */
  uint64_t now = timestamp();
  DelayQueue uplink( "uplink", 20, up_filename, now , atoi(codel_enable) == 1 );
  DelayQueue downlink( "downlink", 20, down_filename, now , atoi (codel_enable) == 1 );

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
	uplink.write( *it );
      }
    }

    if ( sel.read( internet_side.fd() ) ) {
      std::vector< string > filtered_packets( internet_side.recv_raw() );
      for ( auto it = filtered_packets.begin(); it != filtered_packets.end(); it++ ) {
	downlink.write( *it );
      }
    }

    std::vector< string > uplink_packets( uplink.read() );
    for ( auto it = uplink_packets.begin(); it != uplink_packets.end(); it++ ) {
      internet_side.send_raw( *it );
    }

    std::vector< string > downlink_packets( downlink.read() );
    for ( auto it = downlink_packets.begin(); it != downlink_packets.end(); it++ ) {
      client_side.send_raw( *it );
    }
  }
}
