#ifndef ETHERNET_H_
#define ETHERNET_H_

// Includes
#include "arinc653_port_types.h"
#include "beaglebone.h"
#include "interrupt.h"
#include "mdio.h"
#include "cpsw.h"
#include "phy.h"

// Definitions
#define ETHERNET_DEFAULTINSTANCE 0
#define ETHERNET_DEFAULTINSTANCEPORT 1
#define ETHERNET_PACKETLENGTH_MINIMUM 60
#define ETHERNET_PACKETLENGTH_MAXIMUM 1510
#define ETHERNET_BUFFERLENGTH 1514

// Startup method
unsigned char ETHERNET_STARTUP(unsigned char POLLING_MODE);

// Instance initialization method
unsigned char ETHERNET_INITIALIZE_INSTANCE(unsigned int INSTANCE);

// Initialize instance port method
unsigned char ETHERNET_INITIALIZE_INSTANCE_PORT(unsigned int INSTANCE, unsigned int PORT, unsigned char PHY_GIGABIT, unsigned char *MAC_ADDRESS);

// Auto-negotiate instance port method
unsigned char ETHERNET_AUTONEGOTIATE_INSTANCE_PORT(unsigned int INSTANCE, unsigned int PORT, unsigned int TIME_OUT);

// Link up instance port verification method
unsigned char ETHERNET_INSTANCE_PORT_ISLINKUP(unsigned int INSTANCE, unsigned int PORT, unsigned int TIME_OUT);

// Packet receiving method
unsigned int ETHERNET_RECEIVEPACKET(unsigned int INSTANCE, unsigned int *PORT, unsigned char *PACKET, unsigned int LENGTH, unsigned int TIME_OUT, unsigned char ENTER_CRITICAL_SECTION);

// Packet sending method
unsigned char ETHERNET_SENDPACKET(unsigned int INSTANCE, unsigned int PORT, unsigned char *PACKET, unsigned int LENGTH, unsigned char ENTER_CRITICAL_SECTION);

// Initialization hook
extern unsigned char ETHERNET_HOOK_INITIALIZE(void);

// Delay hook
extern void ETHERNET_HOOK_DELAY(unsigned int MILLISECONDS);

// Yield hook
extern void ETHERNET_HOOK_YIELD(void);

// Enter critical section hook
extern void ETHERNET_HOOK_ENTERCRITICALSECTION(void);

// Exit critical section hook
extern void ETHERNET_HOOK_EXITCRITICALSECTION(void);

// Buffer allocation hook
extern unsigned int ETHERNET_HOOK_ALLOCATEBUFFER(unsigned int SIZE);

// Packet reception hook
extern void ETHERNET_HOOK_RECEIVEPACKET(unsigned int INSTANCE, unsigned int PORT, unsigned char *PACKET, unsigned int LENGTH);

#endif
