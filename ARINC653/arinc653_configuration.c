// ARINC653 includes
#include "arinc653_core.h"

// Get partition configuration method
PARTITION_CONFIGURATION_TYPE *CONFIGURATION_GETPARTITIONCONFIGURATION(PARTITION_NAME_TYPE PARTITION_NAME) {
	MODULE_INFORMATION_TYPE *MODULE_INFORMATION;
	INDEX_TYPE CONFIGURATION_PARTITION_INDEX;

	// Gets module information
	MODULE_INFORMATION = _CORE_MODULE_INFORMATION;

	// Iterates partitions
	for (CONFIGURATION_PARTITION_INDEX = 0; CONFIGURATION_PARTITION_INDEX < MODULE_INFORMATION->MODULE_CONFIGURATION->PARTITION_CONFIGURATION_SIZE; CONFIGURATION_PARTITION_INDEX++) {

		// Compares names
		if (COMMON_COMPARESTRINGS(PARTITION_NAME, MODULE_INFORMATION->MODULE_CONFIGURATION->PARTITION_CONFIGURATION[CONFIGURATION_PARTITION_INDEX].NAME, MAX_NAME_LENGTH)) {

			// Returns partition configuration
			return &MODULE_INFORMATION->MODULE_CONFIGURATION->PARTITION_CONFIGURATION[CONFIGURATION_PARTITION_INDEX];
		}
	}

	// Partition not found
	return null;
}

// Get process configuration method
PROCESS_CONFIGURATION_TYPE *CONFIGURATION_GETPROCESSCONFIGURATION(PARTITION_CONFIGURATION_TYPE *PARTITION_CONFIGURATION, PROCESS_NAME_TYPE PROCESS_NAME) {
	INDEX_TYPE CONFIGURATION_PROCESS_INDEX;

	// Iterates processes
	for (CONFIGURATION_PROCESS_INDEX = 0; CONFIGURATION_PROCESS_INDEX < PARTITION_CONFIGURATION->PROCESS_CONFIGURATION_SIZE; CONFIGURATION_PROCESS_INDEX++) {

		// Compares names
		if (COMMON_COMPARESTRINGS(PROCESS_NAME, PARTITION_CONFIGURATION->PROCESS_CONFIGURATION[CONFIGURATION_PROCESS_INDEX].NAME, MAX_NAME_LENGTH)) {

			// Returns process configuration
			return &PARTITION_CONFIGURATION->PROCESS_CONFIGURATION[CONFIGURATION_PROCESS_INDEX];
		}
	}

	// Process not found
	return null;
}
