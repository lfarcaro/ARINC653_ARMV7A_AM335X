#ifndef PROTOCOL_H_
#define PROTOCOL_H_

// Includes
#include "hw_types.h"

// General definitions
#ifndef null
#define null 0
#endif

// Definitions

// Ethernet/IP/UDP packet interpretation method
unsigned char INTERPRET_ETHERNET_IP_UDP_PACKET(unsigned char *PACKET, unsigned int PACKET_LENGTH, unsigned char *SOURCE_MAC_ADDRESS, unsigned char *DESTINATION_MAC_ADDRESS, unsigned int *SOURCE_ADDRESS, unsigned int *DESTINATION_ADDRESS, unsigned short *SOURCE_PORT, unsigned short *DESTINATION_PORT, unsigned int *DATA_INDEX, unsigned int *DATA_LENGTH);

// Ethernet/IP/UDP packet preparation method
unsigned int PREPARE_ETHERNET_IP_UDP_PACKET(unsigned char *BUFFER, unsigned int BUFFER_LENGTH, unsigned char *SOURCE_MAC_ADDRESS, unsigned char *DESTINATION_MAC_ADDRESS, unsigned int SOURCE_ADDRESS, unsigned int DESTINATION_ADDRESS, unsigned short SOURCE_PORT, unsigned short DESTINATION_PORT, unsigned char *DATA, unsigned int DATA_INDEX, unsigned int DATA_LENGTH);

#endif
