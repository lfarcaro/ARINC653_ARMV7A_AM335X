// Includes
#include "protocol.h"

// Configurations

// Definitions
#define ETHERNET_HEADER_LENGTH 14
#define IP_HEADER_LENGTH 20
#define IP_IDENTIFICATION 0x0000
#define IP_DONTFRAGMENT 1
#define IP_DONTFRAGMENT_SHIFT 14
#define IP_MOREFRAGMENTS 0
#define IP_MOREFRAGMENTS_SHIFT 13
#define IP_FRAGMENTOFFSET 0
#define IP_FRAGMENTOFFSET_MASK 0x1FFF
#define IP_TIMETOLIVE 10
#define IP_PROTOCOL_UDP 17
#define UDP_HEADER_LENGTH 8

// Ethernet/IP/UDP packet interpretation method
unsigned char INTERPRET_ETHERNET_IP_UDP_PACKET(unsigned char *PACKET, unsigned int PACKET_LENGTH, unsigned char *SOURCE_MAC_ADDRESS, unsigned char *DESTINATION_MAC_ADDRESS, unsigned int *SOURCE_ADDRESS, unsigned int *DESTINATION_ADDRESS, unsigned short *SOURCE_PORT, unsigned short *DESTINATION_PORT, unsigned int *DATA_INDEX, unsigned int *DATA_LENGTH) {
	unsigned int PACKET_INDEX;
	unsigned int INDEX;
	unsigned int FIELD;
	unsigned int FINISH;
	unsigned int LENGTH;

	// Verifies packet length
	if (PACKET_LENGTH < (ETHERNET_HEADER_LENGTH + IP_HEADER_LENGTH + UDP_HEADER_LENGTH)) {
		return false;
	}

	// Resets packet index
	PACKET_INDEX = 0;

	// Ethernet part
	for (INDEX = 0; INDEX < 6; INDEX++) { // Destination MAC address
		DESTINATION_MAC_ADDRESS[INDEX] = PACKET[PACKET_INDEX++];
	}
	for (INDEX = 0; INDEX < 6; INDEX++) { // Source MAC address
		SOURCE_MAC_ADDRESS[INDEX] = PACKET[PACKET_INDEX++];
	}
	FIELD = (PACKET[PACKET_INDEX + 0] << 8) | (PACKET[PACKET_INDEX + 1] << 0); // Type: IP
	PACKET_INDEX += 2;
	if (FIELD != 0x0800) {
		return false;
	}

	// Calculates IP header checksum
	FIELD = 0;
	INDEX = PACKET_INDEX;
	FINISH = PACKET_INDEX + IP_HEADER_LENGTH;
	while (INDEX < FINISH) {
		FIELD += ((PACKET[INDEX + 0] << 8) | (PACKET[INDEX + 1] << 0)) & 0xFFFF;
		INDEX += 2;
	}
	FIELD += (FIELD >> 16) & 0xFFFF;
	FIELD &= 0xFFFF;
	FIELD ^= 0xFFFF;
	if (FIELD != 0x0000) {
		return false;
	}

	// IP part
	if (PACKET[PACKET_INDEX++] != 0x45) { // Version 4 (IPv4), IHL 5 (20-byte header)
		return false;
	}
	if (PACKET[PACKET_INDEX++] != 0x00) { // DSCP zero (default), ECN zero (not used)
		return false;
	}
	FIELD = (PACKET[PACKET_INDEX + 0] << 8) | (PACKET[PACKET_INDEX + 1] << 0); // Total length
	PACKET_INDEX += 2;
	LENGTH = FIELD;
	if (PACKET_LENGTH < (ETHERNET_HEADER_LENGTH + FIELD)) {
		return false;
	}
	PACKET_INDEX += 2; // Identification
	FIELD = (PACKET[PACKET_INDEX + 0] << 8) | (PACKET[PACKET_INDEX + 1] << 0); // Flags/Fragment offset
	PACKET_INDEX += 2;
	if (((FIELD & IP_MOREFRAGMENTS) >> IP_MOREFRAGMENTS_SHIFT) || ((FIELD & IP_FRAGMENTOFFSET_MASK) != 0)) {
		return false;
	}
	PACKET_INDEX += 1; // Time to live
	if (PACKET[PACKET_INDEX++] != IP_PROTOCOL_UDP) { // Protocol (UDP)
		return false;
	}
	PACKET_INDEX += 2; // Header checksum
	*SOURCE_ADDRESS = (PACKET[PACKET_INDEX + 0] << 24) | (PACKET[PACKET_INDEX + 1] << 16) | (PACKET[PACKET_INDEX + 2] << 8) | (PACKET[PACKET_INDEX + 3] << 0); // Source address
	PACKET_INDEX += 4;
	*DESTINATION_ADDRESS = (PACKET[PACKET_INDEX + 0] << 24) | (PACKET[PACKET_INDEX + 1] << 16) | (PACKET[PACKET_INDEX + 2] << 8) | (PACKET[PACKET_INDEX + 3] << 0); // Destination address
	PACKET_INDEX += 4;

	// Calculates UDP checksum
	if ((LENGTH % 2) != 0) {
		PACKET[ETHERNET_HEADER_LENGTH + LENGTH] = 0x00;
	}
	FIELD = 0;
	INDEX = PACKET_INDEX;
	FINISH = ETHERNET_HEADER_LENGTH + LENGTH;
	while (INDEX < FINISH) {
		FIELD += ((PACKET[INDEX + 0] << 8) | (PACKET[INDEX + 1] << 0)) & 0xFFFF;
		INDEX += 2;
	}
	FIELD += (*SOURCE_ADDRESS >> 16) & 0xFFFF;
	FIELD += (*SOURCE_ADDRESS >> 0) & 0xFFFF;
	FIELD += (*DESTINATION_ADDRESS >> 16) & 0xFFFF;
	FIELD += (*DESTINATION_ADDRESS >> 0) & 0xFFFF;
	FIELD += (IP_PROTOCOL_UDP) & 0xFFFF;
	FIELD += (LENGTH - IP_HEADER_LENGTH) & 0xFFFF;
	FIELD += (FIELD >> 16) & 0xFFFF;
	FIELD &= 0xFFFF;
	FIELD ^= 0xFFFF;
	if (FIELD != 0x0000) {
		return false;
	}

	// UDP part
	*SOURCE_PORT = (PACKET[PACKET_INDEX + 0] << 8) | (PACKET[PACKET_INDEX + 1] << 0); // Source port
	PACKET_INDEX += 2;
	*DESTINATION_PORT = (PACKET[PACKET_INDEX + 0] << 8) | (PACKET[PACKET_INDEX + 1] << 0); // Destination port
	PACKET_INDEX += 2;
	FIELD = (PACKET[PACKET_INDEX + 0] << 8) | (PACKET[PACKET_INDEX + 1] << 0); // Length
	PACKET_INDEX += 2;
	if (PACKET_LENGTH < (ETHERNET_HEADER_LENGTH + IP_HEADER_LENGTH + FIELD)) {
		return false;
	}
	PACKET_INDEX += 2; // Checksum
	// Sets data index
	*DATA_INDEX = PACKET_INDEX;
	// Sets data length
	*DATA_LENGTH = FIELD - UDP_HEADER_LENGTH;

	// Success
	return true;
}

// Ethernet/IP/UDP packet preparation method
unsigned int PREPARE_ETHERNET_IP_UDP_PACKET(unsigned char *BUFFER, unsigned int BUFFER_LENGTH, unsigned char *SOURCE_MAC_ADDRESS, unsigned char *DESTINATION_MAC_ADDRESS, unsigned int SOURCE_ADDRESS, unsigned int DESTINATION_ADDRESS, unsigned short SOURCE_PORT, unsigned short DESTINATION_PORT, unsigned char *DATA, unsigned int DATA_INDEX, unsigned int DATA_LENGTH) {
	unsigned int PACKET_LENGTH;
	unsigned int INDEX;
	unsigned int FIELD;

	// Verifies buffer length
	if (((BUFFER_LENGTH % 2) != 0) || (BUFFER_LENGTH < (ETHERNET_HEADER_LENGTH + IP_HEADER_LENGTH + UDP_HEADER_LENGTH))) {
		return 0;
	}

	// Resets packet length
	PACKET_LENGTH = 0;

	// Ethernet part
	for (INDEX = 0; INDEX < 6; INDEX++) { // Destination MAC address
		BUFFER[PACKET_LENGTH++] = DESTINATION_MAC_ADDRESS[INDEX];
	}
	for (INDEX = 0; INDEX < 6; INDEX++) { // Source MAC address
		BUFFER[PACKET_LENGTH++] = SOURCE_MAC_ADDRESS[INDEX];
	}
	BUFFER[PACKET_LENGTH++] = 0x08; // Type: IP
	BUFFER[PACKET_LENGTH++] = 0x00;

	// IP part
	BUFFER[PACKET_LENGTH++] = 0x45; // Version 4 (IPv4), IHL 5 (20-byte header)
	BUFFER[PACKET_LENGTH++] = 0x00; // DSCP zero (default), ECN zero (not used)
	FIELD = (IP_HEADER_LENGTH + UDP_HEADER_LENGTH + DATA_LENGTH); // Total length
	BUFFER[PACKET_LENGTH++] = (FIELD >> 8) & 0xFF;
	BUFFER[PACKET_LENGTH++] = (FIELD >> 0) & 0xFF;
	BUFFER[PACKET_LENGTH++] = (IP_IDENTIFICATION >> 8) & 0xFF; // Identification
	BUFFER[PACKET_LENGTH++] = (IP_IDENTIFICATION >> 0) & 0xFF;
	FIELD = (IP_DONTFRAGMENT << IP_DONTFRAGMENT_SHIFT) | (IP_MOREFRAGMENTS << IP_MOREFRAGMENTS_SHIFT) | IP_FRAGMENTOFFSET; // Flags/Fragment offset
	BUFFER[PACKET_LENGTH++] = (FIELD >> 8) & 0xFF;
	BUFFER[PACKET_LENGTH++] = (FIELD >> 0) & 0xFF;
	BUFFER[PACKET_LENGTH++] = IP_TIMETOLIVE; // Time to live
	BUFFER[PACKET_LENGTH++] = IP_PROTOCOL_UDP; // Protocol (UDP)
	BUFFER[PACKET_LENGTH++] = 0x00; // Header checksum (placeholder)
	BUFFER[PACKET_LENGTH++] = 0x00;
	BUFFER[PACKET_LENGTH++] = (SOURCE_ADDRESS >> 24) & 0xFF; // Source address
	BUFFER[PACKET_LENGTH++] = (SOURCE_ADDRESS >> 16) & 0xFF;
	BUFFER[PACKET_LENGTH++] = (SOURCE_ADDRESS >> 8) & 0xFF;
	BUFFER[PACKET_LENGTH++] = (SOURCE_ADDRESS >> 0) & 0xFF;
	BUFFER[PACKET_LENGTH++] = (DESTINATION_ADDRESS >> 24) & 0xFF; // Destination address
	BUFFER[PACKET_LENGTH++] = (DESTINATION_ADDRESS >> 16) & 0xFF;
	BUFFER[PACKET_LENGTH++] = (DESTINATION_ADDRESS >> 8) & 0xFF;
	BUFFER[PACKET_LENGTH++] = (DESTINATION_ADDRESS >> 0) & 0xFF;
	// Calculates IP header checksum
	FIELD = 0;
	INDEX = PACKET_LENGTH - IP_HEADER_LENGTH;
	while (INDEX < PACKET_LENGTH) {
		FIELD += ((BUFFER[INDEX + 0] << 8) | (BUFFER[INDEX + 1] << 0)) & 0xFFFF;
		INDEX += 2;
	}
	FIELD += (FIELD >> 16) & 0xFFFF;
	FIELD &= 0xFFFF;
	FIELD ^= 0xFFFF;
	// Sets IP header checksum
	BUFFER[PACKET_LENGTH - 10] = (FIELD >> 8) & 0xFF;
	BUFFER[PACKET_LENGTH - 9] = (FIELD >> 0) & 0xFF;

	// UDP part
	BUFFER[PACKET_LENGTH++] = (SOURCE_PORT >> 8) & 0xFF; // Source port
	BUFFER[PACKET_LENGTH++] = (SOURCE_PORT >> 0) & 0xFF;
	BUFFER[PACKET_LENGTH++] = (DESTINATION_PORT >> 8) & 0xFF; // Destination port
	BUFFER[PACKET_LENGTH++] = (DESTINATION_PORT >> 0) & 0xFF;
	FIELD = (UDP_HEADER_LENGTH + DATA_LENGTH); // Length
	BUFFER[PACKET_LENGTH++] = (FIELD >> 8) & 0xFF;
	BUFFER[PACKET_LENGTH++] = (FIELD >> 0) & 0xFF;
	BUFFER[PACKET_LENGTH++] = 0x00; // Checksum (placeholder)
	BUFFER[PACKET_LENGTH++] = 0x00;
	// Copies data
	for (INDEX = 0; INDEX < DATA_LENGTH; INDEX++) {
		BUFFER[PACKET_LENGTH++] = DATA[DATA_INDEX + INDEX];
	}
	if ((PACKET_LENGTH % 2) != 0) {
		BUFFER[PACKET_LENGTH] = 0x00;
	}
	// Calculates checksum
	FIELD = 0;
	INDEX = PACKET_LENGTH - UDP_HEADER_LENGTH - DATA_LENGTH;
	while (INDEX < PACKET_LENGTH) {
		FIELD += ((BUFFER[INDEX + 0] << 8) | (BUFFER[INDEX + 1] << 0)) & 0xFFFF;
		INDEX += 2;
	}
	FIELD += (SOURCE_ADDRESS >> 16) & 0xFFFF;
	FIELD += (SOURCE_ADDRESS >> 0) & 0xFFFF;
	FIELD += (DESTINATION_ADDRESS >> 16) & 0xFFFF;
	FIELD += (DESTINATION_ADDRESS >> 0) & 0xFFFF;
	FIELD += (IP_PROTOCOL_UDP) & 0xFFFF;
	FIELD += (UDP_HEADER_LENGTH + DATA_LENGTH) & 0xFFFF;
	FIELD += (FIELD >> 16) & 0xFFFF;
	FIELD &= 0xFFFF;
	FIELD ^= 0xFFFF;
	if (FIELD == 0x0000) {
		FIELD = 0xFFFF;
	}
	// Sets UDP checksum
	BUFFER[PACKET_LENGTH - DATA_LENGTH - 2] = (FIELD >> 8) & 0xFF;
	BUFFER[PACKET_LENGTH - DATA_LENGTH - 1] = (FIELD >> 0) & 0xFF;

	// Success
	return PACKET_LENGTH;
}
