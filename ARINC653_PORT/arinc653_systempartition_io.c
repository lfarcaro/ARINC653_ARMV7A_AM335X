// Partition include
//#include "systempartition_io.h"

// SNIPPET_START GLOBAL_INCLUDES
// ARINC653 includes
#include "arinc653_core.h"
// SNIPPET_END

// -------------------- PARTITION DESCRIPTION START --------------------
// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_DESCRIPTION
// SNIPPET_END
// -------------------- PARTITION DESCRIPTION END --------------------

// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_INCLUDES
// SNIPPET_END

// SAMPLINGPORTS process identifier
static PROCESS_ID_TYPE SAMPLINGPORTS_PROCESS_ID;

// QUEUINGPORTS process identifier
static PROCESS_ID_TYPE QUEUINGPORTS_PROCESS_ID;

// ETHERNET process identifier
static PROCESS_ID_TYPE ETHERNET_PROCESS_ID;

// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_GLOBAL_VARIABLES
// Ethernet available flag
static BOOLEAN_TYPE ETHERNET_AVAILABLE = false;

// Ethernet network configuration
static portCHARACTER ETHERNET_MODULEMACADDRESS[6];
static portUINT32 ETHERNET_MODULEIPADDRESS;
// SNIPPET_END

// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_FUNCTIONS
// SNIPPET_END

// -------------------- SAMPLINGPORTS PROCESS START --------------------

// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_SAMPLINGPORTS_PROCESS_GLOBAL_VARIABLES
// Sampling ports message
static CHARACTER_TYPE SAMPLINGPORTS_MESSAGE[PORT_INTERPARTITIONCOMMUNICATION_SAMPLINGPORT_MAXIMUMMESSAGESIZE];

// Sampling ports message length
static MESSAGE_SIZE_TYPE SAMPLINGPORTS_MESSAGE_LENGTH;

// Sampling ports ethernet buffer
static CHARACTER_TYPE SAMPLINGPORTS_ETHERNET_BUFFER[ETHERNET_PACKETLENGTH_MAXIMUM];

// Sampling ports ethernet buffer length
static MESSAGE_SIZE_TYPE SAMPLINGPORTS_ETHERNET_BUFFER_LENGTH;
// SNIPPET_END

// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_SAMPLINGPORTS_PROCESS_FUNCTIONS
// SNIPPET_END

// SAMPLINGPORTS process
static void SAMPLINGPORTS(void) {
	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_SAMPLINGPORTS_PROCESS_VARIABLES
	MODULE_INFORMATION_TYPE *MODULE_INFORMATION;
	RETURN_CODE_TYPE RETURN_CODE;
	MODULE_CONFIGURATION_CONNECTIONTABLE_TYPE *MODULE_CONFIGURATION_CONNECTIONTABLE;
	INDEX_TYPE SAMPLINGPORT_INDEX;
	MODULE_CONFIGURATION_CONNECTIONTABLE_SAMPLINGPORTMAPPING_TYPE *SOURCE;
	PARTITION_ID_TYPE SOURCE_PARTITION_INTERNAL_IDENTIFIER;
	SAMPLING_PORT_ID_TYPE SOURCE_SAMPLINGPORT_IDENTIFIER;
	INDEX_TYPE DESTINATION_INDEX;
	MODULE_CONFIGURATION_CONNECTIONTABLE_SAMPLINGPORTMAPPING_TYPE *DESTINATION;
	PARTITION_ID_TYPE DESTINATION_PARTITION_INTERNAL_IDENTIFIER;
	SAMPLING_PORT_ID_TYPE DESTINATION_SAMPLINGPORT_IDENTIFIER;
	portCHARACTER *DESTINATION_PHYSICAL_ADDRESS;
	portCHARACTER DESTINATION_MAC_ADDRESS[6];
	portUINT32 DESTINATION_IP_ADDRESS;
	portUINT16 DESTINATION_UDP_PORT;
	INDEX_TYPE INDEX;
	BOOLEAN_TYPE COMPLETED;
	// SNIPPET_END

	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_SAMPLINGPORTS_PROCESS_INITIALIZATION
	// Gets module information
	MODULE_INFORMATION = _CORE_MODULE_INFORMATION;

	// Gets module configuration connection table
	MODULE_CONFIGURATION_CONNECTIONTABLE = MODULE_INFORMATION->MODULE_CONFIGURATION->MODULE_CONFIGURATION_CONNECTIONTABLE;

	// Verifies module configuration connection table
	if (MODULE_CONFIGURATION_CONNECTIONTABLE == null) {
		STOP_SELF();
		return;
	}

	// Verifies sampling ports
	if (MODULE_CONFIGURATION_CONNECTIONTABLE->SAMPLINGPORT_SIZE == 0) {
		STOP_SELF();
		return;
	}
	// SNIPPET_END

	// Main loop
	while (true) {

		// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_SAMPLINGPORTS_PROCESS_BEFORE_SUSPENSION_CODE
		// Iterates sampling ports
		for (SAMPLINGPORT_INDEX = 0; SAMPLINGPORT_INDEX < MODULE_CONFIGURATION_CONNECTIONTABLE->SAMPLINGPORT_SIZE; SAMPLINGPORT_INDEX++) {

			// Gets source mapping
			SOURCE = MODULE_CONFIGURATION_CONNECTIONTABLE->SAMPLINGPORT[SAMPLINGPORT_INDEX].SOURCE;

			// Verifies if source is not a standard partition
			if (SOURCE->STANDARD_PARTITION == null) {
				continue;
			}

			// Gets partition internal identifier
			GET_PARTITION_INTERNAL_IDENTIFIER(SOURCE->STANDARD_PARTITION->PARTITION_EXTERNAL_IDENTIFIER, &SOURCE_PARTITION_INTERNAL_IDENTIFIER, &RETURN_CODE);
			if (RETURN_CODE != NO_ERROR) {
				continue;
			}

			// Gets partition sampling port identifier
			GET_PARTITION_SAMPLING_PORT_ID(SOURCE_PARTITION_INTERNAL_IDENTIFIER, SOURCE->STANDARD_PARTITION->SAMPLINGPORT_NAME, &SOURCE_SAMPLINGPORT_IDENTIFIER, &RETURN_CODE);
			if (RETURN_CODE != NO_ERROR) {
				continue;
			}

			// Reads partition sampling message
			READ_PARTITION_SAMPLING_MESSAGE(SOURCE_PARTITION_INTERNAL_IDENTIFIER, SOURCE_SAMPLINGPORT_IDENTIFIER, SAMPLINGPORTS_MESSAGE, &SAMPLINGPORTS_MESSAGE_LENGTH, true, &RETURN_CODE);
			if (RETURN_CODE == NO_ERROR) {

				// Sets completed flag
				COMPLETED = true;

				// Iterates destinations
				for (DESTINATION_INDEX = 0; DESTINATION_INDEX < MODULE_CONFIGURATION_CONNECTIONTABLE->SAMPLINGPORT[SAMPLINGPORT_INDEX].DESTINATION_SIZE; DESTINATION_INDEX++) {

					// Gets destination mapping
					DESTINATION = &MODULE_CONFIGURATION_CONNECTIONTABLE->SAMPLINGPORT[SAMPLINGPORT_INDEX].DESTINATION[DESTINATION_INDEX];

					// Verifies if destination is a standard partition
					if (DESTINATION->STANDARD_PARTITION != null) {

						// Gets partition internal identifier
						GET_PARTITION_INTERNAL_IDENTIFIER(DESTINATION->STANDARD_PARTITION->PARTITION_EXTERNAL_IDENTIFIER, &DESTINATION_PARTITION_INTERNAL_IDENTIFIER, &RETURN_CODE);
						if (RETURN_CODE != NO_ERROR) {

							// Sets completed flag
							COMPLETED = false;
							continue;
						}

						// Gets partition sampling port identifier
						GET_PARTITION_SAMPLING_PORT_ID(DESTINATION_PARTITION_INTERNAL_IDENTIFIER, DESTINATION->STANDARD_PARTITION->SAMPLINGPORT_NAME, &DESTINATION_SAMPLINGPORT_IDENTIFIER, &RETURN_CODE);
						if (RETURN_CODE != NO_ERROR) {

							// Sets completed flag
							COMPLETED = false;
							continue;
						}

						// Writes partition sampling message
						WRITE_PARTITION_SAMPLING_MESSAGE(DESTINATION_PARTITION_INTERNAL_IDENTIFIER, DESTINATION_SAMPLINGPORT_IDENTIFIER, SAMPLINGPORTS_MESSAGE, SAMPLINGPORTS_MESSAGE_LENGTH, &RETURN_CODE);
						if (RETURN_CODE != NO_ERROR) {

							// Sets completed flag
							COMPLETED = false;
							continue;
						}
					} else if (DESTINATION->PSEUDO_PARTITION != null) {

						// Verifies ethernet availability
						if (ETHERNET_AVAILABLE) {

							// Gets destination physical address
							DESTINATION_PHYSICAL_ADDRESS = DESTINATION->PSEUDO_PARTITION->PHYSICAL_ADDRESS;

							// Sets destination MAC address
							for (INDEX = 0; INDEX < 6; INDEX++) {
								DESTINATION_MAC_ADDRESS[INDEX] = DESTINATION_PHYSICAL_ADDRESS[INDEX];
							}

							// Sets destination IP addres
							DESTINATION_IP_ADDRESS = (DESTINATION_PHYSICAL_ADDRESS[6 + 0] << 24) | (DESTINATION_PHYSICAL_ADDRESS[6 + 1] << 16) | (DESTINATION_PHYSICAL_ADDRESS[6 + 2] << 8) | (DESTINATION_PHYSICAL_ADDRESS[6 + 3] << 0);

							// Sets destination UDP port
							DESTINATION_UDP_PORT = (DESTINATION_PHYSICAL_ADDRESS[10 + 0] << 8) | (DESTINATION_PHYSICAL_ADDRESS[10 + 1] << 0);

							// Prepares packet
							SAMPLINGPORTS_ETHERNET_BUFFER_LENGTH = PREPARE_ETHERNET_IP_UDP_PACKET(SAMPLINGPORTS_ETHERNET_BUFFER, sizeof(SAMPLINGPORTS_ETHERNET_BUFFER), ETHERNET_MODULEMACADDRESS, DESTINATION_MAC_ADDRESS, ETHERNET_MODULEIPADDRESS, DESTINATION_IP_ADDRESS, DESTINATION_UDP_PORT, DESTINATION_UDP_PORT, SAMPLINGPORTS_MESSAGE, 0, SAMPLINGPORTS_MESSAGE_LENGTH);

							// Verifies packet
							if (SAMPLINGPORTS_ETHERNET_BUFFER_LENGTH <= 0) {

								// Sets completed flag
								COMPLETED = false;
								continue;
							}

							// Sends packet
							if (!ETHERNET_SENDPACKET(ETHERNET_DEFAULTINSTANCE, ETHERNET_DEFAULTINSTANCEPORT, SAMPLINGPORTS_ETHERNET_BUFFER, SAMPLINGPORTS_ETHERNET_BUFFER_LENGTH, true)) {

								// Sets completed flag
								COMPLETED = false;
								continue;
							}
						}
					}
				}

				// Verifies completed flag
				if (COMPLETED) {

					// Reads partition sampling message
					READ_PARTITION_SAMPLING_MESSAGE(SOURCE_PARTITION_INTERNAL_IDENTIFIER, SOURCE_SAMPLINGPORT_IDENTIFIER, SAMPLINGPORTS_MESSAGE, &SAMPLINGPORTS_MESSAGE_LENGTH, false, &RETURN_CODE);
				}
			}
		}

		// Causes cooperative scheduling
		TIMED_WAIT(0, &RETURN_CODE);
		if (RETURN_CODE != NO_ERROR) {
			RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR YIELDING", 14, &RETURN_CODE);
			return;
		}
		// SNIPPET_END

		// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_SAMPLINGPORTS_PROCESS_AFTER_SUSPENSION_CODE
		// SNIPPET_END
	}
}

// SAMPLINGPORTS process attributes
static PROCESS_ATTRIBUTE_TYPE SAMPLINGPORTS_PROCESS_ATTRIBUTE = {
/*NAME*/(PROCESS_NAME_TYPE) "SAMPLINGPORTS",
/*ENTRY_POINT*/(SYSTEM_ADDRESS_TYPE) &SAMPLINGPORTS,
/*STACK_SIZE*/(STACK_SIZE_TYPE) 256,
/*BASE_PRIORITY*/(PRIORITY_TYPE) 10,
/*PERIOD*/(SYSTEM_TIME_TYPE) APERIODIC_PERIOD_VALUE,
/*TIME_CAPACITY*/(SYSTEM_TIME_TYPE) INFINITE_TIME_VALUE,
/*DEADLINE*/(DEADLINE_TYPE) SOFT };

// -------------------- SAMPLINGPORTS PROCESS END --------------------

// -------------------- QUEUINGPORTS PROCESS START --------------------

// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_QUEUINGPORTS_PROCESS_GLOBAL_VARIABLES
// Queuing ports message
static CHARACTER_TYPE QUEUINGPORTS_MESSAGE[PORT_INTERPARTITIONCOMMUNICATION_QUEUINGPORT_MAXIMUMMESSAGESIZE];

// Queuing ports message length
static MESSAGE_SIZE_TYPE QUEUINGPORTS_MESSAGE_LENGTH;

// Queuing ports ethernet buffer
static CHARACTER_TYPE QUEUINGPORTS_ETHERNET_BUFFER[ETHERNET_PACKETLENGTH_MAXIMUM];

// Queuing ports ethernet buffer length
static MESSAGE_SIZE_TYPE QUEUINGPORTS_ETHERNET_BUFFER_LENGTH;
// SNIPPET_END

// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_QUEUINGPORTS_PROCESS_FUNCTIONS
// SNIPPET_END

// QUEUINGPORTS process
static void QUEUINGPORTS(void) {
	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_QUEUINGPORTS_PROCESS_VARIABLES
	MODULE_INFORMATION_TYPE *MODULE_INFORMATION;
	RETURN_CODE_TYPE RETURN_CODE;
	MODULE_CONFIGURATION_CONNECTIONTABLE_TYPE *MODULE_CONFIGURATION_CONNECTIONTABLE;
	INDEX_TYPE QUEUINGPORT_INDEX;
	MODULE_CONFIGURATION_CONNECTIONTABLE_QUEUINGPORTMAPPING_TYPE *SOURCE;
	PARTITION_ID_TYPE SOURCE_PARTITION_INTERNAL_IDENTIFIER;
	QUEUING_PORT_ID_TYPE SOURCE_QUEUINGPORT_IDENTIFIER;
	MODULE_CONFIGURATION_CONNECTIONTABLE_QUEUINGPORTMAPPING_TYPE *DESTINATION;
	PARTITION_ID_TYPE DESTINATION_PARTITION_INTERNAL_IDENTIFIER;
	QUEUING_PORT_ID_TYPE DESTINATION_QUEUINGPORT_IDENTIFIER;
	portCHARACTER *DESTINATION_PHYSICAL_ADDRESS;
	portCHARACTER DESTINATION_MAC_ADDRESS[6];
	portUINT32 DESTINATION_IP_ADDRESS;
	portUINT16 DESTINATION_UDP_PORT;
	INDEX_TYPE INDEX;
	BOOLEAN_TYPE COMPLETED;
	// SNIPPET_END

	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_QUEUINGPORTS_PROCESS_INITIALIZATION
	// Gets module information
	MODULE_INFORMATION = _CORE_MODULE_INFORMATION;

	// Gets module configuration connection table
	MODULE_CONFIGURATION_CONNECTIONTABLE = MODULE_INFORMATION->MODULE_CONFIGURATION->MODULE_CONFIGURATION_CONNECTIONTABLE;

	// Verifies module configuration connection table
	if (MODULE_CONFIGURATION_CONNECTIONTABLE == null) {
		STOP_SELF();
		return;
	}

	// Verifies queuing ports
	if (MODULE_CONFIGURATION_CONNECTIONTABLE->QUEUINGPORT_SIZE == 0) {
		STOP_SELF();
		return;
	}
	// SNIPPET_END

	// Main loop
	while (true) {

		// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_QUEUINGPORTS_PROCESS_BEFORE_SUSPENSION_CODE
		// Iterates queuing ports
		for (QUEUINGPORT_INDEX = 0; QUEUINGPORT_INDEX < MODULE_CONFIGURATION_CONNECTIONTABLE->QUEUINGPORT_SIZE; QUEUINGPORT_INDEX++) {

			// Gets source mapping
			SOURCE = MODULE_CONFIGURATION_CONNECTIONTABLE->QUEUINGPORT[QUEUINGPORT_INDEX].SOURCE;

			// Verifies if source is not a standard partition
			if (SOURCE->STANDARD_PARTITION == null) {
				continue;
			}

			// Gets partition internal identifier
			GET_PARTITION_INTERNAL_IDENTIFIER(SOURCE->STANDARD_PARTITION->PARTITION_EXTERNAL_IDENTIFIER, &SOURCE_PARTITION_INTERNAL_IDENTIFIER, &RETURN_CODE);
			if (RETURN_CODE != NO_ERROR) {
				continue;
			}

			// Gets partition queuing port identifier
			GET_PARTITION_QUEUING_PORT_ID(SOURCE_PARTITION_INTERNAL_IDENTIFIER, SOURCE->STANDARD_PARTITION->QUEUINGPORT_NAME, &SOURCE_QUEUINGPORT_IDENTIFIER, &RETURN_CODE);
			if (RETURN_CODE != NO_ERROR) {
				continue;
			}

			// Receives partition queuing message
			RECEIVE_PARTITION_QUEUING_MESSAGE(SOURCE_PARTITION_INTERNAL_IDENTIFIER, SOURCE_QUEUINGPORT_IDENTIFIER, QUEUINGPORTS_MESSAGE, &QUEUINGPORTS_MESSAGE_LENGTH, true, &RETURN_CODE);
			if (RETURN_CODE == NO_ERROR) {

				// Sets completed flag
				COMPLETED = true;

				// Gets destination mapping
				DESTINATION = MODULE_CONFIGURATION_CONNECTIONTABLE->QUEUINGPORT[QUEUINGPORT_INDEX].DESTINATION;

				// Verifies if destination is a standard partition
				if (DESTINATION->STANDARD_PARTITION != null) {

					// Gets partition internal identifier
					GET_PARTITION_INTERNAL_IDENTIFIER(DESTINATION->STANDARD_PARTITION->PARTITION_EXTERNAL_IDENTIFIER, &DESTINATION_PARTITION_INTERNAL_IDENTIFIER, &RETURN_CODE);
					if (RETURN_CODE != NO_ERROR) {

						// Sets completed flag
						COMPLETED = false;
						continue;
					}

					// Gets partition queuing port identifier
					GET_PARTITION_QUEUING_PORT_ID(DESTINATION_PARTITION_INTERNAL_IDENTIFIER, DESTINATION->STANDARD_PARTITION->QUEUINGPORT_NAME, &DESTINATION_QUEUINGPORT_IDENTIFIER, &RETURN_CODE);
					if (RETURN_CODE != NO_ERROR) {

						// Sets completed flag
						COMPLETED = false;
						continue;
					}

					// Sends partition queuing message
					SEND_PARTITION_QUEUING_MESSAGE(DESTINATION_PARTITION_INTERNAL_IDENTIFIER, DESTINATION_QUEUINGPORT_IDENTIFIER, QUEUINGPORTS_MESSAGE, QUEUINGPORTS_MESSAGE_LENGTH, &RETURN_CODE);
					if (RETURN_CODE != NO_ERROR) {

						// Sets completed flag
						COMPLETED = false;
						continue;
					}
				} else if (DESTINATION->PSEUDO_PARTITION != null) {

					// Verifies ethernet availability
					if (ETHERNET_AVAILABLE) {

						// Gets destination physical address
						DESTINATION_PHYSICAL_ADDRESS = DESTINATION->PSEUDO_PARTITION->PHYSICAL_ADDRESS;

						// Sets destination MAC address
						for (INDEX = 0; INDEX < 6; INDEX++) {
							DESTINATION_MAC_ADDRESS[INDEX] = DESTINATION_PHYSICAL_ADDRESS[INDEX];
						}

						// Sets destination IP addres
						DESTINATION_IP_ADDRESS = (DESTINATION_PHYSICAL_ADDRESS[6 + 0] << 24) | (DESTINATION_PHYSICAL_ADDRESS[6 + 1] << 16) | (DESTINATION_PHYSICAL_ADDRESS[6 + 2] << 8) | (DESTINATION_PHYSICAL_ADDRESS[6 + 3] << 0);

						// Sets destination UDP port
						DESTINATION_UDP_PORT = (DESTINATION_PHYSICAL_ADDRESS[10 + 0] << 8) | (DESTINATION_PHYSICAL_ADDRESS[10 + 1] << 0);

						// Prepares packet
						QUEUINGPORTS_ETHERNET_BUFFER_LENGTH = PREPARE_ETHERNET_IP_UDP_PACKET(QUEUINGPORTS_ETHERNET_BUFFER, sizeof(QUEUINGPORTS_ETHERNET_BUFFER), ETHERNET_MODULEMACADDRESS, DESTINATION_MAC_ADDRESS, ETHERNET_MODULEIPADDRESS, DESTINATION_IP_ADDRESS, DESTINATION_UDP_PORT, DESTINATION_UDP_PORT, QUEUINGPORTS_MESSAGE, 0, QUEUINGPORTS_MESSAGE_LENGTH);

						// Verifies packet
						if (QUEUINGPORTS_ETHERNET_BUFFER_LENGTH <= 0) {

							// Sets completed flag
							COMPLETED = false;
							continue;
						}

						// Sends packet
						if (!ETHERNET_SENDPACKET(ETHERNET_DEFAULTINSTANCE, ETHERNET_DEFAULTINSTANCEPORT, QUEUINGPORTS_ETHERNET_BUFFER, QUEUINGPORTS_ETHERNET_BUFFER_LENGTH, true)) {

							// Sets completed flag
							COMPLETED = false;
							continue;
						}
					}
				}

				// Verifies completed flag
				if (COMPLETED) {

					// Receives partition queuing message
					RECEIVE_PARTITION_QUEUING_MESSAGE(SOURCE_PARTITION_INTERNAL_IDENTIFIER, SOURCE_QUEUINGPORT_IDENTIFIER, QUEUINGPORTS_MESSAGE, &QUEUINGPORTS_MESSAGE_LENGTH, false, &RETURN_CODE);
				}
			}
		}

		// Causes cooperative scheduling
		TIMED_WAIT(0, &RETURN_CODE);
		if (RETURN_CODE != NO_ERROR) {
			RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR YIELDING", 14, &RETURN_CODE);
			return;
		}
		// SNIPPET_END

		// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_QUEUINGPORTS_PROCESS_AFTER_SUSPENSION_CODE
		// SNIPPET_END
	}
}

// QUEUINGPORTS process attributes
static PROCESS_ATTRIBUTE_TYPE QUEUINGPORTS_PROCESS_ATTRIBUTE = {
/*NAME*/(PROCESS_NAME_TYPE) "QUEUINGPORTS",
/*ENTRY_POINT*/(SYSTEM_ADDRESS_TYPE) &QUEUINGPORTS,
/*STACK_SIZE*/(STACK_SIZE_TYPE) 256,
/*BASE_PRIORITY*/(PRIORITY_TYPE) 10,
/*PERIOD*/(SYSTEM_TIME_TYPE) APERIODIC_PERIOD_VALUE,
/*TIME_CAPACITY*/(SYSTEM_TIME_TYPE) INFINITE_TIME_VALUE,
/*DEADLINE*/(DEADLINE_TYPE) SOFT };

// -------------------- QUEUINGPORTS PROCESS END --------------------

// -------------------- ETHERNET PROCESS START --------------------

// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_ETHERNET_PROCESS_GLOBAL_VARIABLES
// Ethernet receive variables
static portUINT32 ETHERNET_RECEIVE_PORT;
static portCHARACTER ETHERNET_RECEIVE_BUFFER[ETHERNET_PACKETLENGTH_MAXIMUM];
static portSIZE ETHERNET_RECEIVE_PACKET_LENGTH;
static portCHARACTER ETHERNET_RECEIVE_SOURCEMACADDRESS[6];
static portCHARACTER ETHERNET_RECEIVE_DESTINATIONMACADDRESS[6];
static portUINT32 ETHERNET_RECEIVE_SOURCEADDRESS;
static portUINT32 ETHERNET_RECEIVE_DESTINATIONADDRESS;
static portUINT16 ETHERNET_RECEIVE_SOURCEPORT;
static portUINT16 ETHERNET_RECEIVE_DESTINATIONPORT;
static portINDEX ETHERNET_RECEIVE_DATA_INDEX;
static portSIZE ETHERNET_RECEIVE_DATA_LENGTH;
// SNIPPET_END

// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_ETHERNET_PROCESS_FUNCTIONS
// Ethernet initialization hook
unsigned char ETHERNET_HOOK_INITIALIZE(void) {
	return true;
}

// Delay hook
void ETHERNET_HOOK_DELAY(unsigned int MILLISECONDS) {
	RETURN_CODE_TYPE RETURN_CODE;

	// Delays
	TIMED_WAIT(MILLISECONDS, &RETURN_CODE);
}

// Yield hook
void ETHERNET_HOOK_YIELD(void) {
	RETURN_CODE_TYPE RETURN_CODE;

	// Causes cooperative scheduling
	TIMED_WAIT(0, &RETURN_CODE);
}

// Enter critical section hook
void ETHERNET_HOOK_ENTERCRITICALSECTION(void) {

	// Enters core
	ENTER_CORE();
}

// Exit critical section hook
void ETHERNET_HOOK_EXITCRITICALSECTION(void) {

	// Exits core
	EXIT_CORE();
}

// Buffer allocation hook
unsigned int ETHERNET_HOOK_ALLOCATEBUFFER(unsigned int SIZE) {
	MODULE_INFORMATION_TYPE *MODULE_INFORMATION;
	heapPOINTER PTR_POINTER = null;

	// Enters core
	ENTER_CORE();

	// Gets module information
	MODULE_INFORMATION = _CORE_MODULE_INFORMATION;

	// Allocates into partition heap
	HEAP_ALLOCATE(&_CURRENT_PARTITION_INFORMATION->REC_HEAP, &PTR_POINTER, (heapSIZE) SIZE, false);

	// Exits core
	EXIT_CORE();
	return (unsigned int) PTR_POINTER;
}

// Packet reception hook
void ETHERNET_HOOK_RECEIVEPACKET(unsigned int INSTANCE, unsigned int PORT, unsigned char *PACKET, unsigned int LENGTH) {
	// Not used in polling mode
}
// SNIPPET_END

// ETHERNET process
static void ETHERNET(void) {
	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_ETHERNET_PROCESS_VARIABLES
	MODULE_INFORMATION_TYPE *MODULE_INFORMATION;
	MODULE_CONFIGURATION_TYPE *MODULE_CONFIGURATION;
	RETURN_CODE_TYPE RETURN_CODE;
	MODULE_CONFIGURATION_CONNECTIONTABLE_TYPE *MODULE_CONFIGURATION_CONNECTIONTABLE;
	PORT_MODULE_CONFIGURATION_TYPE *PORT_MODULE_CONFIGURATION;
	// Sampling port variables
	INDEX_TYPE SAMPLINGPORT_INDEX;
	MODULE_CONFIGURATION_CONNECTIONTABLE_SAMPLINGPORTMAPPING_TYPE *SAMPLINGPORT_SOURCE;
	MODULE_CONFIGURATION_CONNECTIONTABLE_SAMPLINGPORTMAPPING_TYPE *SAMPLINGPORT_DESTINATION;
	SAMPLING_PORT_ID_TYPE DESTINATION_SAMPLINGPORT_IDENTIFIER;
	// Queuing port variables
	INDEX_TYPE QUEUINGPORT_INDEX;
	MODULE_CONFIGURATION_CONNECTIONTABLE_QUEUINGPORTMAPPING_TYPE *QUEUINGPORT_SOURCE;
	MODULE_CONFIGURATION_CONNECTIONTABLE_QUEUINGPORTMAPPING_TYPE *QUEUINGPORT_DESTINATION;
	QUEUING_PORT_ID_TYPE DESTINATION_QUEUINGPORT_IDENTIFIER;
	// General variables
	portCHARACTER *SOURCE_PHYSICAL_ADDRESS;
	portUINT16 SOURCE_UDP_PORT;
	INDEX_TYPE DESTINATION_INDEX;
	PARTITION_ID_TYPE DESTINATION_PARTITION_INTERNAL_IDENTIFIER;
	INDEX_TYPE INDEX;
	BOOLEAN_TYPE ACCEPTED;
	BOOLEAN_TYPE DELIVERED;
	// SNIPPET_END

	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_ETHERNET_PROCESS_INITIALIZATION
	// Sets ethernet available flag
	ETHERNET_AVAILABLE = false;

	// Gets module information
	MODULE_INFORMATION = _CORE_MODULE_INFORMATION;

	// Gets module configuration
	MODULE_CONFIGURATION = MODULE_INFORMATION->MODULE_CONFIGURATION;

	// Gets module configuration connection table
	MODULE_CONFIGURATION_CONNECTIONTABLE = MODULE_INFORMATION->MODULE_CONFIGURATION->MODULE_CONFIGURATION_CONNECTIONTABLE;

	// Verifies module configuration connection table
	if (MODULE_CONFIGURATION_CONNECTIONTABLE == null) {
		STOP_SELF();
		return;
	}

	// Verifies ports
	if ((MODULE_CONFIGURATION_CONNECTIONTABLE->SAMPLINGPORT_SIZE == 0) && (MODULE_CONFIGURATION_CONNECTIONTABLE->QUEUINGPORT_SIZE == 0)) {
		STOP_SELF();
		return;
	}

	// Gets port module configuration
	PORT_MODULE_CONFIGURATION = MODULE_CONFIGURATION->PORT_MODULE_CONFIGURATION;

	// Sets module MAC address
	for (INDEX = 0; INDEX < 6; INDEX++) {
		ETHERNET_MODULEMACADDRESS[INDEX] = PORT_MODULE_CONFIGURATION->MODULE_NETWORK_PHYSICAL_ADDRESS[INDEX];
	}

	// Sets module IP addres
	ETHERNET_MODULEIPADDRESS = (PORT_MODULE_CONFIGURATION->MODULE_NETWORK_PHYSICAL_ADDRESS[6 + 0] << 24) | (PORT_MODULE_CONFIGURATION->MODULE_NETWORK_PHYSICAL_ADDRESS[6 + 1] << 16) | (PORT_MODULE_CONFIGURATION->MODULE_NETWORK_PHYSICAL_ADDRESS[6 + 2] << 8) | (PORT_MODULE_CONFIGURATION->MODULE_NETWORK_PHYSICAL_ADDRESS[6 + 3] << 0);

	// Starts up ethernet
	if (!ETHERNET_STARTUP(true)) {
		STOP_SELF();
		return;
	}

	// Initializes ethernet instance
	if (!ETHERNET_INITIALIZE_INSTANCE(ETHERNET_DEFAULTINSTANCE)) {
		STOP_SELF();
		return;
	}

	// Initializes ethernet default instance default port
	if (!ETHERNET_INITIALIZE_INSTANCE_PORT(ETHERNET_DEFAULTINSTANCE, ETHERNET_DEFAULTINSTANCEPORT, false, ETHERNET_MODULEMACADDRESS)) {
		STOP_SELF();
		return;
	}
	// SNIPPET_END

	// Main loop
	while (true) {

		// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_ETHERNET_PROCESS_BEFORE_SUSPENSION_CODE
		// Auto-negotiates ethernet default instance default port
		if (!ETHERNET_AUTONEGOTIATE_INSTANCE_PORT(ETHERNET_DEFAULTINSTANCE, ETHERNET_DEFAULTINSTANCEPORT, 0)) {
			STOP_SELF();
			return;
		}

		// Handling loop
		while (true) {

			// Sets ethernet available flag
			ETHERNET_AVAILABLE = ETHERNET_INSTANCE_PORT_ISLINKUP(ETHERNET_DEFAULTINSTANCE, ETHERNET_DEFAULTINSTANCEPORT, 10);

			// Verifies if link is up
			if (!ETHERNET_AVAILABLE) {
				break;
			}

			// Receives packet
			ETHERNET_RECEIVE_PACKET_LENGTH = ETHERNET_RECEIVEPACKET(ETHERNET_DEFAULTINSTANCE, &ETHERNET_RECEIVE_PORT, ETHERNET_RECEIVE_BUFFER, sizeof(ETHERNET_RECEIVE_BUFFER), 250, false);

			// Verifies received packet
			if (ETHERNET_RECEIVE_PACKET_LENGTH > 0) {

				// Interprets packet
				if (INTERPRET_ETHERNET_IP_UDP_PACKET(ETHERNET_RECEIVE_BUFFER, ETHERNET_RECEIVE_PACKET_LENGTH, ETHERNET_RECEIVE_SOURCEMACADDRESS, ETHERNET_RECEIVE_DESTINATIONMACADDRESS, &ETHERNET_RECEIVE_SOURCEADDRESS, &ETHERNET_RECEIVE_DESTINATIONADDRESS, &ETHERNET_RECEIVE_SOURCEPORT, &ETHERNET_RECEIVE_DESTINATIONPORT, &ETHERNET_RECEIVE_DATA_INDEX, &ETHERNET_RECEIVE_DATA_LENGTH)) {

					// Sets accepted flag
					ACCEPTED = true;

					// Compares MAC addresses
					for (INDEX = 0; INDEX < 6; INDEX++) {
						if (ETHERNET_RECEIVE_DESTINATIONMACADDRESS[INDEX] != ETHERNET_MODULEMACADDRESS[INDEX]) {
							ACCEPTED = false;
							break;
						}
					}

					// Compares IP addresses
					if (ETHERNET_RECEIVE_DESTINATIONADDRESS != ETHERNET_MODULEIPADDRESS) {
						ACCEPTED = false;
					}

					// Verifies accepted flag
					if (ACCEPTED) {

						// Sets delivered flag
						DELIVERED = false;

						// Iterates sampling ports
						for (SAMPLINGPORT_INDEX = 0; SAMPLINGPORT_INDEX < MODULE_CONFIGURATION_CONNECTIONTABLE->SAMPLINGPORT_SIZE; SAMPLINGPORT_INDEX++) {

							// Gets source mapping
							SAMPLINGPORT_SOURCE = MODULE_CONFIGURATION_CONNECTIONTABLE->SAMPLINGPORT[SAMPLINGPORT_INDEX].SOURCE;

							// Verifies if source is not a pseudo partition
							if (SAMPLINGPORT_SOURCE->PSEUDO_PARTITION == null) {
								continue;
							}

							// Gets source physical address
							SOURCE_PHYSICAL_ADDRESS = SAMPLINGPORT_SOURCE->PSEUDO_PARTITION->PHYSICAL_ADDRESS;

							// Sets source UDP port
							SOURCE_UDP_PORT = (SOURCE_PHYSICAL_ADDRESS[0] << 8) | (SOURCE_PHYSICAL_ADDRESS[1] << 0);

							// Compares UDP ports
							if (ETHERNET_RECEIVE_DESTINATIONPORT != SOURCE_UDP_PORT) {
								continue;
							}

							// Iterates destinations
							for (DESTINATION_INDEX = 0; DESTINATION_INDEX < MODULE_CONFIGURATION_CONNECTIONTABLE->SAMPLINGPORT[SAMPLINGPORT_INDEX].DESTINATION_SIZE; DESTINATION_INDEX++) {

								// Gets destination mapping
								SAMPLINGPORT_DESTINATION = &MODULE_CONFIGURATION_CONNECTIONTABLE->SAMPLINGPORT[SAMPLINGPORT_INDEX].DESTINATION[DESTINATION_INDEX];

								// Verifies if destination is a standard partition
								if (SAMPLINGPORT_DESTINATION->STANDARD_PARTITION != null) {

									// Gets partition internal identifier
									GET_PARTITION_INTERNAL_IDENTIFIER(SAMPLINGPORT_DESTINATION->STANDARD_PARTITION->PARTITION_EXTERNAL_IDENTIFIER, &DESTINATION_PARTITION_INTERNAL_IDENTIFIER, &RETURN_CODE);
									if (RETURN_CODE != NO_ERROR) {
										continue;
									}

									// Gets partition sampling port identifier
									GET_PARTITION_SAMPLING_PORT_ID(DESTINATION_PARTITION_INTERNAL_IDENTIFIER, SAMPLINGPORT_DESTINATION->STANDARD_PARTITION->SAMPLINGPORT_NAME, &DESTINATION_SAMPLINGPORT_IDENTIFIER, &RETURN_CODE);
									if (RETURN_CODE != NO_ERROR) {
										continue;
									}

									// Writes partition sampling message
									WRITE_PARTITION_SAMPLING_MESSAGE(DESTINATION_PARTITION_INTERNAL_IDENTIFIER, DESTINATION_SAMPLINGPORT_IDENTIFIER, (MESSAGE_ADDR_TYPE) &ETHERNET_RECEIVE_BUFFER[ETHERNET_RECEIVE_DATA_INDEX], (MESSAGE_SIZE_TYPE) ETHERNET_RECEIVE_DATA_LENGTH, &RETURN_CODE);
									if (RETURN_CODE != NO_ERROR) {
										continue;
									}
								}
							}

							// Sets delivered flag
							DELIVERED = true;
							break;
						}

						// Verifies delivered flag
						if (DELIVERED) {
							continue;
						}

						// Iterates queuing ports
						for (QUEUINGPORT_INDEX = 0; QUEUINGPORT_INDEX < MODULE_CONFIGURATION_CONNECTIONTABLE->QUEUINGPORT_SIZE; QUEUINGPORT_INDEX++) {

							// Gets source mapping
							QUEUINGPORT_SOURCE = MODULE_CONFIGURATION_CONNECTIONTABLE->QUEUINGPORT[QUEUINGPORT_INDEX].SOURCE;

							// Verifies if source is not a pseudo partition
							if (QUEUINGPORT_SOURCE->PSEUDO_PARTITION == null) {
								continue;
							}

							// Gets source physical address
							SOURCE_PHYSICAL_ADDRESS = QUEUINGPORT_SOURCE->PSEUDO_PARTITION->PHYSICAL_ADDRESS;

							// Sets source UDP port
							SOURCE_UDP_PORT = (SOURCE_PHYSICAL_ADDRESS[0] << 8) | (SOURCE_PHYSICAL_ADDRESS[1] << 0);

							// Compares UDP ports
							if (ETHERNET_RECEIVE_DESTINATIONPORT != SOURCE_UDP_PORT) {
								continue;
							}

							// Gets destination mapping
							QUEUINGPORT_DESTINATION = MODULE_CONFIGURATION_CONNECTIONTABLE->QUEUINGPORT[QUEUINGPORT_INDEX].DESTINATION;

							// Verifies if destination is a standard partition
							if (QUEUINGPORT_DESTINATION->STANDARD_PARTITION != null) {

								// Gets partition internal identifier
								GET_PARTITION_INTERNAL_IDENTIFIER(QUEUINGPORT_DESTINATION->STANDARD_PARTITION->PARTITION_EXTERNAL_IDENTIFIER, &DESTINATION_PARTITION_INTERNAL_IDENTIFIER, &RETURN_CODE);
								if (RETURN_CODE != NO_ERROR) {
									continue;
								}

								// Gets partition queuing port identifier
								GET_PARTITION_QUEUING_PORT_ID(DESTINATION_PARTITION_INTERNAL_IDENTIFIER, QUEUINGPORT_DESTINATION->STANDARD_PARTITION->QUEUINGPORT_NAME, &DESTINATION_QUEUINGPORT_IDENTIFIER, &RETURN_CODE);
								if (RETURN_CODE != NO_ERROR) {
									continue;
								}

								// Sends partition queuing message
								SEND_PARTITION_QUEUING_MESSAGE(DESTINATION_PARTITION_INTERNAL_IDENTIFIER, DESTINATION_QUEUINGPORT_IDENTIFIER, (MESSAGE_ADDR_TYPE) &ETHERNET_RECEIVE_BUFFER[ETHERNET_RECEIVE_DATA_INDEX], (MESSAGE_SIZE_TYPE) ETHERNET_RECEIVE_DATA_LENGTH, &RETURN_CODE);
								if (RETURN_CODE != NO_ERROR) {
									continue;
								}
							}

							// Sets delivered flag
							DELIVERED = true;
							break;
						}

						// Verifies delivered flag
						if (DELIVERED) {
							continue;
						}
					}
				}
			}

			// Causes cooperative scheduling
			TIMED_WAIT(0, &RETURN_CODE);
			if (RETURN_CODE != NO_ERROR) {
				RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR YIELDING", 14, &RETURN_CODE);
				return;
			}
		}
		// SNIPPET_END

		// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_ETHERNET_PROCESS_AFTER_SUSPENSION_CODE
		// SNIPPET_END
	}
}

// ETHERNET process attributes
static PROCESS_ATTRIBUTE_TYPE ETHERNET_PROCESS_ATTRIBUTE = {
/*NAME*/(PROCESS_NAME_TYPE) "ETHERNET",
/*ENTRY_POINT*/(SYSTEM_ADDRESS_TYPE) &ETHERNET,
/*STACK_SIZE*/(STACK_SIZE_TYPE) 256,
/*BASE_PRIORITY*/(PRIORITY_TYPE) 10,
/*PERIOD*/(SYSTEM_TIME_TYPE) APERIODIC_PERIOD_VALUE,
/*TIME_CAPACITY*/(SYSTEM_TIME_TYPE) INFINITE_TIME_VALUE,
/*DEADLINE*/(DEADLINE_TYPE) SOFT };

// -------------------- ETHERNET PROCESS END --------------------

// SYSTEMPARTITION_IO partition error handler
static void ERRORHANDLER(void) {
	RETURN_CODE_TYPE RETURN_CODE;
	ERROR_STATUS_TYPE ERROR_STATUS;
	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_ERRORHANDLER_VARIABLES
	// SNIPPET_END

	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_ERRORHANDLER_INITIALIZATION
	// SNIPPET_END

	// Error handling loop
	while (true) {

		// Gets error status
		GET_ERROR_STATUS(&ERROR_STATUS, &RETURN_CODE);
		if (RETURN_CODE == NO_ACTION) {
			break;
		} else if (RETURN_CODE != NO_ERROR) {
			// SNIPPET_START GET_ERROR_STATUS_ERROR_HANDLING_CODE
			// SNIPPET_END
		}

		// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_ERRORHANDLER_CODE
		// Stops failed process
		STOP(ERROR_STATUS.FAILED_PROCESS_ID, &RETURN_CODE);

		// Restarts failed process
		START(ERROR_STATUS.FAILED_PROCESS_ID, &RETURN_CODE);
		// SNIPPET_END
	}

	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_ERRORHANDLER_TERMINATION
	// SNIPPET_END

	// Stops
	STOP_SELF();
}

// SYSTEMPARTITION_IO partition health monitoring callback
void SYSTEMPARTITION_IO_HMCALLBACK(SYSTEM_STATE_TYPE SYSTEM_STATE, ERROR_IDENTIFIER_TYPE ERROR_IDENTIFIER) {
	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_HEALTHMONITORINGCALLBACK_VARIABLES
	RETURN_CODE_TYPE RETURN_CODE;
	// SNIPPET_END

	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_HEALTHMONITORINGCALLBACK_CODE
	// Restarts partition
	SET_PARTITION_MODE(COLD_START, &RETURN_CODE);
	// SNIPPET_END
}

// SYSTEMPARTITION_IO partition default process
void SYSTEMPARTITION_IO(void) {
	RETURN_CODE_TYPE RETURN_CODE;
	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_DEFAULTPROCESS_VARIABLES
	// SNIPPET_END

	// Creates SAMPLINGPORTS process
	CREATE_PROCESS(&SAMPLINGPORTS_PROCESS_ATTRIBUTE, &SAMPLINGPORTS_PROCESS_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START CREATE_PROCESS_ERROR_HANDLING_CODE
		RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR CREATING PROCESS", 22, &RETURN_CODE);
		return;
		// SNIPPET_END
	}

	// Creates QUEUINGPORTS process
	CREATE_PROCESS(&QUEUINGPORTS_PROCESS_ATTRIBUTE, &QUEUINGPORTS_PROCESS_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START CREATE_PROCESS_ERROR_HANDLING_CODE
		RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR CREATING PROCESS", 22, &RETURN_CODE);
		return;
		// SNIPPET_END
	}

	// Creates ETHERNET process
	CREATE_PROCESS(&ETHERNET_PROCESS_ATTRIBUTE, &ETHERNET_PROCESS_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START CREATE_PROCESS_ERROR_HANDLING_CODE
		RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR CREATING PROCESS", 22, &RETURN_CODE);
		return;
		// SNIPPET_END
	}

	// Starts SAMPLINGPORTS process
	START(SAMPLINGPORTS_PROCESS_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START START_ERROR_HANDLING_CODE
		RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR STARTING PROCESS", 22, &RETURN_CODE);
		return;
		// SNIPPET_END
	}

	// Starts QUEUINGPORTS process
	START(QUEUINGPORTS_PROCESS_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START START_ERROR_HANDLING_CODE
		RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR STARTING PROCESS", 22, &RETURN_CODE);
		return;
		// SNIPPET_END
	}

	// Starts ETHERNET process
	START(ETHERNET_PROCESS_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START START_ERROR_HANDLING_CODE
		RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR STARTING PROCESS", 22, &RETURN_CODE);
		return;
		// SNIPPET_END
	}

	// Creates error handler
	CREATE_ERROR_HANDLER(&ERRORHANDLER, 256, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START CREATE_ERROR_HANDLER_ERROR_HANDLING_CODE
		RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR CREATING ERROR HANDLER", 28, &RETURN_CODE);
		return;
		// SNIPPET_END
	}

	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_DEFAULTPROCESS_INITIALIZATION
	// SNIPPET_END

	// Sets partition mode
	SET_PARTITION_MODE(NORMAL, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START SET_PARTITION_MODE_ERROR_HANDLING_CODE
		RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR SETTING PARTITION MODE", 28, &RETURN_CODE);
		return;
		// SNIPPET_END
	}

	// Start of idle process

	// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_IDLEPROCESS_INITIALIZATION
	// SNIPPET_END

	// Main loop
	while (true) {

		// SNIPPET_START SYSTEMPARTITION_IO_PARTITION_IDLEPROCESS_CODE
		// SNIPPET_END
	}
}
