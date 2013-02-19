#include "pkt-classifier.h"
#include <sys/time.h>

void skype_delays( std::string packet, uint64_t ts, std::string action )
{
  static PktClassifier skype_classifier;
  std::string hash_str = skype_classifier.pkt_hash( packet );
  uint8_t flow_id = skype_classifier.get_flow_id( packet ); 
  const MACAddress destination_address( packet.substr( 0, 6 ) );
  assert( hash_str != "" );
  if ( destination_address.is_broadcast() ){
    /* if pkt is broadcast, return, theres lots of network chatter */
    return;

  } else if ( flow_id != PktClassifier::UDP_PROTOCOL_NUM ) {
    /* Its not Skype, because the flow type isn't UDP */
    return;

  } else {
    printf( " Skype Packet %s %lu, size : %lu hash : ", action.c_str(), ts, packet.size() );
    for( uint32_t i=0; i < hash_str.size(); i++) {
      printf("%02x",(unsigned char)hash_str[i]);
    }
    printf( "\n");
  }
}

uint64_t get_absolute_us()
{
  struct timeval tv;
  gettimeofday( &tv, NULL );
  return (uint64_t)tv.tv_sec*1000000 + (uint64_t)tv.tv_usec;
}
