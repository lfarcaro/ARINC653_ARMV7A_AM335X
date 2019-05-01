// ARINC653 includes
#include "arinc653_core.h"

// Null-terminated string copy method
BOOLEAN_TYPE COMMON_COPYSTRING(CHARACTER_TYPE *FROM, CHARACTER_TYPE *TO, SIZE_TYPE MAXIMUM_LENGTH) {
	INDEX_TYPE CHARACTER_INDEX;

	// Initializes character index
	CHARACTER_INDEX = 0;

	// Compare loop
	while (CHARACTER_INDEX < MAXIMUM_LENGTH) {

		// Copies character
		TO[CHARACTER_INDEX] = FROM[CHARACTER_INDEX];

		// Verifies string end
		if (FROM[CHARACTER_INDEX++] == 0) {
			return true;
		}
	}

	// Maximum length reached
	return false;
}

// Null-terminated string comparison method
BOOLEAN_TYPE COMMON_COMPARESTRINGS(CHARACTER_TYPE *STRING1, CHARACTER_TYPE *STRING2, SIZE_TYPE MAXIMUM_LENGTH) {
	INDEX_TYPE CHARACTER_INDEX;

	// Initializes character index
	CHARACTER_INDEX = 0;

	// Compare loop
	while (CHARACTER_INDEX < MAXIMUM_LENGTH) {

		// Compares characters
		if (STRING1[CHARACTER_INDEX] != STRING2[CHARACTER_INDEX]) {
			break;
		}

		// Verifies string end
		if (STRING1[CHARACTER_INDEX++] == 0) {
			return true;
		}
	}

	// Different strings
	return false;
}
