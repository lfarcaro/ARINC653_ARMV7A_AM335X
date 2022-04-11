// Includes
#include "mmu.h"

// MMU information (one per core)
MMU_INFORMATION_TYPE CORE_MMU_INFORMATION[MMU_CORECOUNT];

// FL translation table allocation method
unsigned char MMU_FLTRANSLATIONTABLE_ALLOCATE(unsigned int **PTR_FLTRANSLATIONTABLE) {
	MMU_INFORMATION_TYPE *MMU_INFORMATION;
	unsigned int *PTR_CLEAR;
	unsigned int *PTR_SLTRANSLATIONTABLE;

	// Gets MMU information
	MMU_INFORMATION = &CORE_MMU_INFORMATION[0];

	// Verifies pointer
	if (*PTR_FLTRANSLATIONTABLE != null) {

		// Fills FL and SL translation tables with fault entries
		for (PTR_CLEAR = *PTR_FLTRANSLATIONTABLE; PTR_CLEAR < (*PTR_FLTRANSLATIONTABLE + MMU_FLTRANSLATIONTABLE_ENTRYCOUNT); PTR_CLEAR++) {

			// Verifies SL translation table entry
			if ((*PTR_CLEAR & 0x00000003) == 0x1) {

				// Gets SL translation table address
				PTR_SLTRANSLATIONTABLE = (unsigned int *) (*PTR_CLEAR & 0xFFFFFC00);

				// Clears SL translation table
				if (!MMU_SLTRANSLATIONTABLE_ALLOCATE(&PTR_SLTRANSLATIONTABLE)) {
					return 0;
				}
			} else {

				// Clears non translation table entry
				*PTR_CLEAR = 0x00000000;
			}
		}

		// Already allocated
		return 1;
	}

	// Verifies full size
	if (MMU_INFORMATION->SIZ_FLTRANSLATIONTABLE_FULL + (MMU_FLTRANSLATIONTABLE_ENTRYCOUNT * MMU_FLTRANSLATIONTABLE_ENTRYSIZE) - 1 >= MMU_INFORMATION->SIZ_FLTRANSLATIONTABLE_SIZE) {
		return 0;
	}

	// Fills FL translation table with fault entries
	for (PTR_CLEAR = MMU_INFORMATION->PTR_FLTRANSLATIONTABLE_POINTER; PTR_CLEAR < (MMU_INFORMATION->PTR_FLTRANSLATIONTABLE_POINTER + MMU_FLTRANSLATIONTABLE_ENTRYCOUNT); PTR_CLEAR++) {
		*PTR_CLEAR = 0x00000000;
	}

	// Prepares address
	*PTR_FLTRANSLATIONTABLE = MMU_INFORMATION->PTR_FLTRANSLATIONTABLE_POINTER;

	// Moves pointer
	MMU_INFORMATION->PTR_FLTRANSLATIONTABLE_POINTER += MMU_FLTRANSLATIONTABLE_ENTRYCOUNT;

	// Calculates new full size
	MMU_INFORMATION->SIZ_FLTRANSLATIONTABLE_FULL += (MMU_FLTRANSLATIONTABLE_ENTRYCOUNT * MMU_FLTRANSLATIONTABLE_ENTRYSIZE);
	return 1;
}

// Map FL translation table section (1MB) method
unsigned char MMU_FLTRANSLATIONTABLE_MAPSECTION(unsigned int *PTR_FLTRANSLATIONTABLE, unsigned int UINT_ADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SECURITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS) {
	unsigned int *PTR_ENTRY;
	unsigned int UINT_FLAGS;
	unsigned int UINT_ENTRY;

	// Verifies address 1MB alignment
	if ((UINT_ADDRESS & 0xFFF00000) != UINT_ADDRESS) {
		return 0;
	}

	// Points entry
	PTR_ENTRY = PTR_FLTRANSLATIONTABLE + (UINT_ADDRESS >> 20);

	// Verifies entry
	if (*PTR_ENTRY != 0x00000000) {
		return 0;
	}

	// Nulls flags
	UINT_FLAGS = 0;

	// Verifies inner cache policy
	switch (UINT_INNERCACHEPOLICY) {
	case MMU_CACHEPOLICY_NONCACHEABLE:
		UINT_INNERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SECTION_CACHEPOLICY_NONCACHEABLE;
		break;
	case MMU_CACHEPOLICY_WB_WA:
		UINT_INNERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SECTION_CACHEPOLICY_WB_WA;
		break;
	case MMU_CACHEPOLICY_WT_NOWA:
		UINT_INNERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SECTION_CACHEPOLICY_WT_NOWA;
		break;
	case MMU_CACHEPOLICY_WB_NOWA:
		UINT_INNERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SECTION_CACHEPOLICY_WB_NOWA;
		break;
	}

	// Verifies outer cache policy
	switch (UINT_OUTERCACHEPOLICY) {
	case MMU_CACHEPOLICY_NONCACHEABLE:
		UINT_OUTERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SECTION_CACHEPOLICY_NONCACHEABLE;
		break;
	case MMU_CACHEPOLICY_WB_WA:
		UINT_OUTERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SECTION_CACHEPOLICY_WB_WA;
		break;
	case MMU_CACHEPOLICY_WT_NOWA:
		UINT_OUTERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SECTION_CACHEPOLICY_WT_NOWA;
		break;
	case MMU_CACHEPOLICY_WB_NOWA:
		UINT_OUTERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SECTION_CACHEPOLICY_WB_NOWA;
		break;
	}

	// Verifies memory type
	switch (UINT_MEMORYTYPE) {
	case MMU_MEMORYTYPE_STRONGLYORDERED_SHAREABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_STRONGLYORDERED_SHAREABLE;
		break;
	case MMU_MEMORYTYPE_DEVICE_SHAREABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_DEVICE_SHAREABLE;
		break;
	case MMU_MEMORYTYPE_NORMAL_WT_NOWA:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_NORMAL_WT_NOWA;
		break;
	case MMU_MEMORYTYPE_NORMAL_WB_NOWA:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_NORMAL_WB_NOWA;
		break;
	case MMU_MEMORYTYPE_NORMAL_NONCACHEABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_NORMAL_NONCACHEABLE;
		break;
	case MMU_MEMORYTYPE_NORMAL_WB_WA:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_NORMAL_WB_WA;
		break;
	case MMU_MEMORYTYPE_DEVICE_NONSHAREABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_DEVICE_NONSHAREABLE;
		break;
	case MMU_MEMORYTYPE_NORMAL_CACHEABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_NORMAL_CACHEABLE(UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY);
		break;
	}

	// Verifies shareability
	switch (UINT_SHAREABILITY) {
	case MMU_SHAREABILITY_SHAREABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_SHAREABILITY_SHAREABLE;
		break;
	case MMU_SHAREABILITY_NONSHAREABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_SHAREABILITY_NONSHAREABLE;
		break;
	}

	// Verifies security
	switch (UINT_SECURITY) {
	case MMU_SECURITY_SECURE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_SECURITY_SECURE;
		break;
	case MMU_SECURITY_NONSECURE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_SECURITY_NONSECURE;
		break;
	}

	// Verifies scope
	switch (UINT_SCOPE) {
	case MMU_SCOPE_GLOBAL:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_SCOPE_GLOBAL;
		break;
	case MMU_SCOPE_NONGLOBAL:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_SCOPE_NONGLOBAL;
		break;
	}

	// Verifies access
	switch (UINT_ACCESS & 0x0F) {
	case MMU_ACCESS_PL1_NO_PL0_NO:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_ACCESS_PL1_NO_PL0_NO;
		break;
	case MMU_ACCESS_PL1_RW_PL0_NO:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_ACCESS_PL1_RW_PL0_NO;
		break;
	case MMU_ACCESS_PL1_RW_PL0_RW:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_ACCESS_PL1_RW_PL0_RW;
		break;
	case MMU_ACCESS_PL1_RO_PL0_NO:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_ACCESS_PL1_RO_PL0_NO;
		break;
	case MMU_ACCESS_PL1_RO_PL0_RO:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_ACCESS_PL1_RO_PL0_RO;
		break;
	}

	// Verifies access
	if ((UINT_ACCESS & MMU_ACCESS_EXECUTENEVER) == MMU_ACCESS_EXECUTENEVER) {
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SECTION_ACCESS_EXECUTENEVER;
	}

	// Prepares entry
	UINT_ENTRY = 0x00000002 | (UINT_ADDRESS & 0xFFF00000) | UINT_FLAGS;

	// Sets entry
	*PTR_ENTRY = UINT_ENTRY;
	return 1;
}

// Map FL translation table sections (1MB) method
unsigned char MMU_FLTRANSLATIONTABLE_MAPSECTIONS(unsigned int *PTR_FLTRANSLATIONTABLE, unsigned int UINT_STARTADDRESS, unsigned int UINT_ENDADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SECURITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS) {
	unsigned int ADD_ADDRESS;

	// Verifies start address 1MB alignment
	if ((UINT_STARTADDRESS & 0xFFF00000) != UINT_STARTADDRESS) {
		return 0;
	}

	// Verifies end address 1MB alignment
	if ((UINT_ENDADDRESS & 0xFFF00000) != UINT_ENDADDRESS) {
		return 0;
	}

	// Sets start address
	ADD_ADDRESS = UINT_STARTADDRESS;

	// Map loop
	while (ADD_ADDRESS < UINT_ENDADDRESS) {

		// Maps section
		if (!MMU_FLTRANSLATIONTABLE_MAPSECTION(PTR_FLTRANSLATIONTABLE, ADD_ADDRESS, UINT_MEMORYTYPE, UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY, UINT_SHAREABILITY, UINT_SECURITY, UINT_SCOPE, UINT_ACCESS)) {
			return 0;
		}

		// Moves address
		ADD_ADDRESS += 1 * 1024 * 1024;
	}
	return 1;
}

// Map FL translation table supersection (16MB) method
unsigned char MMU_FLTRANSLATIONTABLE_MAPSUPERSECTION(unsigned int *PTR_FLTRANSLATIONTABLE, unsigned int UINT_ADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SECURITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS) {
	unsigned int *PTR_ENTRY;
	unsigned int UINT_FLAGS;
	unsigned int UINT_ENTRY;
	int IDX_INDEX;

	// Verifies address 16MB alignment
	if ((UINT_ADDRESS & 0xFF000000) != UINT_ADDRESS) {
		return 0;
	}

	// Points first entry
	PTR_ENTRY = PTR_FLTRANSLATIONTABLE + (UINT_ADDRESS >> 20);

	// Nulls flags
	UINT_FLAGS = 0;

	// Verifies inner cache policy
	switch (UINT_INNERCACHEPOLICY) {
	case MMU_CACHEPOLICY_NONCACHEABLE:
		UINT_INNERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SUPERSECTION_CACHEPOLICY_NONCACHEABLE;
		break;
	case MMU_CACHEPOLICY_WB_WA:
		UINT_INNERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SUPERSECTION_CACHEPOLICY_WB_WA;
		break;
	case MMU_CACHEPOLICY_WT_NOWA:
		UINT_INNERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SUPERSECTION_CACHEPOLICY_WT_NOWA;
		break;
	case MMU_CACHEPOLICY_WB_NOWA:
		UINT_INNERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SUPERSECTION_CACHEPOLICY_WB_NOWA;
		break;
	}

	// Verifies outer cache policy
	switch (UINT_OUTERCACHEPOLICY) {
	case MMU_CACHEPOLICY_NONCACHEABLE:
		UINT_OUTERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SUPERSECTION_CACHEPOLICY_NONCACHEABLE;
		break;
	case MMU_CACHEPOLICY_WB_WA:
		UINT_OUTERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SUPERSECTION_CACHEPOLICY_WB_WA;
		break;
	case MMU_CACHEPOLICY_WT_NOWA:
		UINT_OUTERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SUPERSECTION_CACHEPOLICY_WT_NOWA;
		break;
	case MMU_CACHEPOLICY_WB_NOWA:
		UINT_OUTERCACHEPOLICY = MMU_FLTRANSLATIONTABLE_SUPERSECTION_CACHEPOLICY_WB_NOWA;
		break;
	}

	// Verifies memory type
	switch (UINT_MEMORYTYPE) {
	case MMU_MEMORYTYPE_STRONGLYORDERED_SHAREABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_STRONGLYORDERED_SHAREABLE;
		break;
	case MMU_MEMORYTYPE_DEVICE_SHAREABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_DEVICE_SHAREABLE;
		break;
	case MMU_MEMORYTYPE_NORMAL_WT_NOWA:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_NORMAL_WT_NOWA;
		break;
	case MMU_MEMORYTYPE_NORMAL_WB_NOWA:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_NORMAL_WB_NOWA;
		break;
	case MMU_MEMORYTYPE_NORMAL_NONCACHEABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_NORMAL_NONCACHEABLE;
		break;
	case MMU_MEMORYTYPE_NORMAL_WB_WA:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_NORMAL_WB_WA;
		break;
	case MMU_MEMORYTYPE_DEVICE_NONSHAREABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_DEVICE_NONSHAREABLE;
		break;
	case MMU_MEMORYTYPE_NORMAL_CACHEABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_NORMAL_CACHEABLE(UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY);
		break;
	}

	// Verifies shareability
	switch (UINT_SHAREABILITY) {
	case MMU_SHAREABILITY_SHAREABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_SHAREABILITY_SHAREABLE;
		break;
	case MMU_SHAREABILITY_NONSHAREABLE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_SHAREABILITY_NONSHAREABLE;
		break;
	}

	// Verifies security
	switch (UINT_SECURITY) {
	case MMU_SECURITY_SECURE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_SECURITY_SECURE;
		break;
	case MMU_SECURITY_NONSECURE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_SECURITY_NONSECURE;
		break;
	}

	// Verifies scope
	switch (UINT_SCOPE) {
	case MMU_SCOPE_GLOBAL:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_SCOPE_GLOBAL;
		break;
	case MMU_SCOPE_NONGLOBAL:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_SCOPE_NONGLOBAL;
		break;
	}

	// Verifies access
	switch (UINT_ACCESS & 0x0F) {
	case MMU_ACCESS_PL1_NO_PL0_NO:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_ACCESS_PL1_NO_PL0_NO;
		break;
	case MMU_ACCESS_PL1_RW_PL0_NO:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_ACCESS_PL1_RW_PL0_NO;
		break;
	case MMU_ACCESS_PL1_RW_PL0_RW:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_ACCESS_PL1_RW_PL0_RW;
		break;
	case MMU_ACCESS_PL1_RO_PL0_NO:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_ACCESS_PL1_RO_PL0_NO;
		break;
	case MMU_ACCESS_PL1_RO_PL0_RO:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_ACCESS_PL1_RO_PL0_RO;
		break;
	}

	// Verifies access
	if ((UINT_ACCESS & MMU_ACCESS_EXECUTENEVER) == MMU_ACCESS_EXECUTENEVER) {
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SUPERSECTION_ACCESS_EXECUTENEVER;
	}

	// Prepares entry
	UINT_ENTRY = 0x00040002 | (UINT_ADDRESS & 0xFF000000) | UINT_FLAGS;

	// Set the entries in the FL translation table for the supersection attributes
	for (IDX_INDEX = 0; IDX_INDEX < 16; IDX_INDEX++) {

		// Verifies entry
		if (*PTR_ENTRY != 0x00000000) {
			return 0;
		}

		// Sets entry
		*PTR_ENTRY++ = UINT_ENTRY;
	}
	return 1;
}

// Map FL translation table supersections (16MB) method
unsigned char MMU_FLTRANSLATIONTABLE_MAPSUPERSECTIONS(unsigned int *PTR_FLTRANSLATIONTABLE, unsigned int UINT_STARTADDRESS, unsigned int UINT_ENDADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SECURITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS) {
	unsigned int ADD_ADDRESS;

	// Verifies start address 16MB alignment
	if ((UINT_STARTADDRESS & 0xFF000000) != UINT_STARTADDRESS) {
		return 0;
	}

	// Verifies end address 16MB alignment
	if ((UINT_ENDADDRESS & 0xFF000000) != UINT_ENDADDRESS) {
		return 0;
	}

	// Sets start address
	ADD_ADDRESS = UINT_STARTADDRESS;

	// Map loop
	while (ADD_ADDRESS < UINT_ENDADDRESS) {

		// Maps supersection
		if (!MMU_FLTRANSLATIONTABLE_MAPSUPERSECTION(PTR_FLTRANSLATIONTABLE, ADD_ADDRESS, UINT_MEMORYTYPE, UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY, UINT_SHAREABILITY, UINT_SECURITY, UINT_SCOPE, UINT_ACCESS)) {
			return 0;
		}

		// Moves address
		ADD_ADDRESS += 16 * 1024 * 1024;
	}
	return 1;
}

// Map FL translation table SL translation table method
unsigned char MMU_FLTRANSLATIONTABLE_MAPSLTRANSLATIONTABLE(unsigned int *PTR_FLTRANSLATIONTABLE, unsigned int UINT_ADDRESS, unsigned int *PTR_SLTRANSLATIONTABLE, unsigned int UINT_SECURITY) {
	unsigned int *PTR_ENTRY;
	unsigned int UINT_FLAGS;
	unsigned int UINT_ENTRY;

	// Verifies address 1MB alignment
	if ((UINT_ADDRESS & 0xFFF00000) != UINT_ADDRESS) {
		return 0;
	}

	// Points entry
	PTR_ENTRY = PTR_FLTRANSLATIONTABLE + (UINT_ADDRESS >> 20);

	// Verifies entry
	if (*PTR_ENTRY != 0x00000000) {
		return 0;
	}

	// Nulls flags
	UINT_FLAGS = 0;

	// Verifies security
	switch (UINT_SECURITY) {
	case MMU_SECURITY_SECURE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SLTRANSLATIONTABLE_SECURITY_SECURE;
		break;
	case MMU_SECURITY_NONSECURE:
		UINT_FLAGS |= MMU_FLTRANSLATIONTABLE_SLTRANSLATIONTABLE_SECURITY_NONSECURE;
		break;
	}

	// Prepares entry
	UINT_ENTRY = 0x00000001 | ((unsigned int) PTR_SLTRANSLATIONTABLE & 0xFFFFFC00) | UINT_FLAGS;

	// Sets entry
	*PTR_ENTRY = UINT_ENTRY;
	return 1;
}

// SL translation table allocation method
unsigned char MMU_SLTRANSLATIONTABLE_ALLOCATE(unsigned int **PTR_SLTRANSLATIONTABLE) {
	MMU_INFORMATION_TYPE *MMU_INFORMATION;
	unsigned int *PTR_CLEAR;

	// Gets MMU information
	MMU_INFORMATION = &CORE_MMU_INFORMATION[0];

	// Verifies pointer
	if (*PTR_SLTRANSLATIONTABLE != null) {

		// Fills SL translation table with fault entries
		for (PTR_CLEAR = *PTR_SLTRANSLATIONTABLE; PTR_CLEAR < (*PTR_SLTRANSLATIONTABLE + MMU_SLTRANSLATIONTABLE_ENTRYCOUNT); PTR_CLEAR++) {
			*PTR_CLEAR = 0x00000000;
		}

		// Already allocated
		return 1;
	}

	// Verifies full size
	if (MMU_INFORMATION->SIZ_SLTRANSLATIONTABLE_FULL + (MMU_SLTRANSLATIONTABLE_ENTRYCOUNT * MMU_SLTRANSLATIONTABLE_ENTRYSIZE) - 1 >= MMU_INFORMATION->SIZ_SLTRANSLATIONTABLE_SIZE) {
		return 0;
	}

	// Fills SL translation table with fault entries
	for (PTR_CLEAR = MMU_INFORMATION->PTR_SLTRANSLATIONTABLE_POINTER; PTR_CLEAR < (MMU_INFORMATION->PTR_SLTRANSLATIONTABLE_POINTER + MMU_SLTRANSLATIONTABLE_ENTRYCOUNT); PTR_CLEAR++) {
		*PTR_CLEAR = 0x00000000;
	}

	// Prepares address
	*PTR_SLTRANSLATIONTABLE = MMU_INFORMATION->PTR_SLTRANSLATIONTABLE_POINTER;

	// Moves pointer
	MMU_INFORMATION->PTR_SLTRANSLATIONTABLE_POINTER += MMU_SLTRANSLATIONTABLE_ENTRYCOUNT;

	// Calculates new full size
	MMU_INFORMATION->SIZ_SLTRANSLATIONTABLE_FULL += (MMU_SLTRANSLATIONTABLE_ENTRYCOUNT * MMU_SLTRANSLATIONTABLE_ENTRYSIZE);
	return 1;
}

// Map SL translation table small page (4KB) method
unsigned char MMU_SLTRANSLATIONTABLE_MAPSMALLPAGE(unsigned int *PTR_SLTRANSLATIONTABLE, unsigned int UINT_ADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS) {
	unsigned int *PTR_ENTRY;
	unsigned int UINT_FLAGS;
	unsigned int UINT_ENTRY;

	// Verifies address 4KB alignment
	if ((UINT_ADDRESS & 0xFFFFF000) != UINT_ADDRESS) {
		return 0;
	}

	// Points entry
	PTR_ENTRY = PTR_SLTRANSLATIONTABLE + ((UINT_ADDRESS >> 12) & 0x000000FF);

	// Verifies entry
	if (*PTR_ENTRY != 0x00000000) {
		return 0;
	}

	// Nulls flags
	UINT_FLAGS = 0;

	// Verifies inner cache policy
	switch (UINT_INNERCACHEPOLICY) {
	case MMU_CACHEPOLICY_NONCACHEABLE:
		UINT_INNERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_SMALLPAGE_CACHEPOLICY_NONCACHEABLE;
		break;
	case MMU_CACHEPOLICY_WB_WA:
		UINT_INNERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_SMALLPAGE_CACHEPOLICY_WB_WA;
		break;
	case MMU_CACHEPOLICY_WT_NOWA:
		UINT_INNERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_SMALLPAGE_CACHEPOLICY_WT_NOWA;
		break;
	case MMU_CACHEPOLICY_WB_NOWA:
		UINT_INNERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_SMALLPAGE_CACHEPOLICY_WB_NOWA;
		break;
	}

	// Verifies outer cache policy
	switch (UINT_OUTERCACHEPOLICY) {
	case MMU_CACHEPOLICY_NONCACHEABLE:
		UINT_OUTERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_SMALLPAGE_CACHEPOLICY_NONCACHEABLE;
		break;
	case MMU_CACHEPOLICY_WB_WA:
		UINT_OUTERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_SMALLPAGE_CACHEPOLICY_WB_WA;
		break;
	case MMU_CACHEPOLICY_WT_NOWA:
		UINT_OUTERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_SMALLPAGE_CACHEPOLICY_WT_NOWA;
		break;
	case MMU_CACHEPOLICY_WB_NOWA:
		UINT_OUTERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_SMALLPAGE_CACHEPOLICY_WB_NOWA;
		break;
	}

	// Verifies memory type
	switch (UINT_MEMORYTYPE) {
	case MMU_MEMORYTYPE_STRONGLYORDERED_SHAREABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_STRONGLYORDERED_SHAREABLE;
		break;
	case MMU_MEMORYTYPE_DEVICE_SHAREABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_DEVICE_SHAREABLE;
		break;
	case MMU_MEMORYTYPE_NORMAL_WT_NOWA:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_NORMAL_WT_NOWA;
		break;
	case MMU_MEMORYTYPE_NORMAL_WB_NOWA:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_NORMAL_WB_NOWA;
		break;
	case MMU_MEMORYTYPE_NORMAL_NONCACHEABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_NORMAL_NONCACHEABLE;
		break;
	case MMU_MEMORYTYPE_NORMAL_WB_WA:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_NORMAL_WB_WA;
		break;
	case MMU_MEMORYTYPE_DEVICE_NONSHAREABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_DEVICE_NONSHAREABLE;
		break;
	case MMU_MEMORYTYPE_NORMAL_CACHEABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_NORMAL_CACHEABLE(UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY);
		break;
	}

	// Verifies shareability
	switch (UINT_SHAREABILITY) {
	case MMU_SHAREABILITY_SHAREABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_SHAREABILITY_SHAREABLE;
		break;
	case MMU_SHAREABILITY_NONSHAREABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_SHAREABILITY_NONSHAREABLE;
		break;
	}

	// Verifies scope
	switch (UINT_SCOPE) {
	case MMU_SCOPE_GLOBAL:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_SCOPE_GLOBAL;
		break;
	case MMU_SCOPE_NONGLOBAL:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_SCOPE_NONGLOBAL;
		break;
	}

	// Verifies access
	switch (UINT_ACCESS & 0x0F) {
	case MMU_ACCESS_PL1_NO_PL0_NO:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_ACCESS_PL1_NO_PL0_NO;
		break;
	case MMU_ACCESS_PL1_RW_PL0_NO:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_ACCESS_PL1_RW_PL0_NO;
		break;
	case MMU_ACCESS_PL1_RW_PL0_RW:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_ACCESS_PL1_RW_PL0_RW;
		break;
	case MMU_ACCESS_PL1_RO_PL0_NO:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_ACCESS_PL1_RO_PL0_NO;
		break;
	case MMU_ACCESS_PL1_RO_PL0_RO:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_ACCESS_PL1_RO_PL0_RO;
		break;
	}

	// Verifies access
	if ((UINT_ACCESS & MMU_ACCESS_EXECUTENEVER) == MMU_ACCESS_EXECUTENEVER) {
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_SMALLPAGE_ACCESS_EXECUTENEVER;
	}

	// Prepares entry
	UINT_ENTRY = 0x00000002 | (UINT_ADDRESS & 0xFFFFF000) | UINT_FLAGS;

	// Sets entry
	*PTR_ENTRY = UINT_ENTRY;
	return 1;
}

// Map SL translation table small pages (4KB) method
unsigned char MMU_SLTRANSLATIONTABLE_MAPSMALLPAGES(unsigned int *PTR_SLTRANSLATIONTABLE, unsigned int UINT_STARTADDRESS, unsigned int UINT_ENDADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS) {
	unsigned int ADD_ADDRESS;

	// Verifies start address 4KB alignment
	if ((UINT_STARTADDRESS & 0xFFFFF000) != UINT_STARTADDRESS) {
		return 0;
	}

	// Verifies end address 4KB alignment
	if ((UINT_ENDADDRESS & 0xFFFFF000) != UINT_ENDADDRESS) {
		return 0;
	}

	// Sets start address
	ADD_ADDRESS = UINT_STARTADDRESS;

	// Map loop
	while (ADD_ADDRESS < UINT_ENDADDRESS) {

		// Maps small page
		if (!MMU_SLTRANSLATIONTABLE_MAPSMALLPAGE(PTR_SLTRANSLATIONTABLE, ADD_ADDRESS, UINT_MEMORYTYPE, UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY, UINT_SHAREABILITY, UINT_SCOPE, UINT_ACCESS)) {
			return 0;
		}

		// Moves address
		ADD_ADDRESS += 4 * 1024;
	}
	return 1;
}

// Map SL translation table large page (64KB) method
unsigned char MMU_SLTRANSLATIONTABLE_MAPLARGEPAGE(unsigned int *PTR_SLTRANSLATIONTABLE, unsigned int UINT_ADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS) {
	unsigned int *PTR_ENTRY;
	unsigned int UINT_FLAGS;
	unsigned int UINT_ENTRY;
	unsigned int IDX_INDEX;

	// Verifies address 64KB alignment
	if ((UINT_ADDRESS & 0xFFFF0000) != UINT_ADDRESS) {
		return 0;
	}

	// Points entry
	PTR_ENTRY = PTR_SLTRANSLATIONTABLE + ((UINT_ADDRESS >> 12) & 0x000000FF);

	// Nulls flags
	UINT_FLAGS = 0;

	// Verifies inner cache policy
	switch (UINT_INNERCACHEPOLICY) {
	case MMU_CACHEPOLICY_NONCACHEABLE:
		UINT_INNERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_LARGEPAGE_CACHEPOLICY_NONCACHEABLE;
		break;
	case MMU_CACHEPOLICY_WB_WA:
		UINT_INNERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_LARGEPAGE_CACHEPOLICY_WB_WA;
		break;
	case MMU_CACHEPOLICY_WT_NOWA:
		UINT_INNERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_LARGEPAGE_CACHEPOLICY_WT_NOWA;
		break;
	case MMU_CACHEPOLICY_WB_NOWA:
		UINT_INNERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_LARGEPAGE_CACHEPOLICY_WB_NOWA;
		break;
	}

	// Verifies outer cache policy
	switch (UINT_OUTERCACHEPOLICY) {
	case MMU_CACHEPOLICY_NONCACHEABLE:
		UINT_OUTERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_LARGEPAGE_CACHEPOLICY_NONCACHEABLE;
		break;
	case MMU_CACHEPOLICY_WB_WA:
		UINT_OUTERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_LARGEPAGE_CACHEPOLICY_WB_WA;
		break;
	case MMU_CACHEPOLICY_WT_NOWA:
		UINT_OUTERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_LARGEPAGE_CACHEPOLICY_WT_NOWA;
		break;
	case MMU_CACHEPOLICY_WB_NOWA:
		UINT_OUTERCACHEPOLICY = MMU_SLTRANSLATIONTABLE_LARGEPAGE_CACHEPOLICY_WB_NOWA;
		break;
	}

	// Verifies memory type
	switch (UINT_MEMORYTYPE) {
	case MMU_MEMORYTYPE_STRONGLYORDERED_SHAREABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_STRONGLYORDERED_SHAREABLE;
		break;
	case MMU_MEMORYTYPE_DEVICE_SHAREABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_DEVICE_SHAREABLE;
		break;
	case MMU_MEMORYTYPE_NORMAL_WT_NOWA:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_NORMAL_WT_NOWA;
		break;
	case MMU_MEMORYTYPE_NORMAL_WB_NOWA:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_NORMAL_WB_NOWA;
		break;
	case MMU_MEMORYTYPE_NORMAL_NONCACHEABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_NORMAL_NONCACHEABLE;
		break;
	case MMU_MEMORYTYPE_NORMAL_WB_WA:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_NORMAL_WB_WA;
		break;
	case MMU_MEMORYTYPE_DEVICE_NONSHAREABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_DEVICE_NONSHAREABLE;
		break;
	case MMU_MEMORYTYPE_NORMAL_CACHEABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_NORMAL_CACHEABLE(UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY);
		break;
	}

	// Verifies shareability
	switch (UINT_SHAREABILITY) {
	case MMU_SHAREABILITY_SHAREABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_SHAREABILITY_SHAREABLE;
		break;
	case MMU_SHAREABILITY_NONSHAREABLE:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_SHAREABILITY_NONSHAREABLE;
		break;
	}

	// Verifies scope
	switch (UINT_SCOPE) {
	case MMU_SCOPE_GLOBAL:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_SCOPE_GLOBAL;
		break;
	case MMU_SCOPE_NONGLOBAL:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_SCOPE_NONGLOBAL;
		break;
	}

	// Verifies access
	switch (UINT_ACCESS & 0x0F) {
	case MMU_ACCESS_PL1_NO_PL0_NO:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_ACCESS_PL1_NO_PL0_NO;
		break;
	case MMU_ACCESS_PL1_RW_PL0_NO:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_ACCESS_PL1_RW_PL0_NO;
		break;
	case MMU_ACCESS_PL1_RW_PL0_RW:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_ACCESS_PL1_RW_PL0_RW;
		break;
	case MMU_ACCESS_PL1_RO_PL0_NO:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_ACCESS_PL1_RO_PL0_NO;
		break;
	case MMU_ACCESS_PL1_RO_PL0_RO:
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_ACCESS_PL1_RO_PL0_RO;
		break;
	}

	// Verifies access
	if ((UINT_ACCESS & MMU_ACCESS_EXECUTENEVER) == MMU_ACCESS_EXECUTENEVER) {
		UINT_FLAGS |= MMU_SLTRANSLATIONTABLE_LARGEPAGE_ACCESS_EXECUTENEVER;
	}

	// Prepares entry
	UINT_ENTRY = 0x00000001 | (UINT_ADDRESS & 0xFFFF0000) | UINT_FLAGS;

	// Set the entries in the SL translation table for the large page attributes
	for (IDX_INDEX = 0; IDX_INDEX < 16; IDX_INDEX++) {

		// Verifies entry
		if (*PTR_ENTRY != 0x00000000) {
			return 0;
		}

		// Sets entry
		*PTR_ENTRY++ = UINT_ENTRY;
	}
	return 1;
}

// Map SL translation table large pages (64KB) method
unsigned char MMU_SLTRANSLATIONTABLE_MAPLARGEPAGES(unsigned int *PTR_SLTRANSLATIONTABLE, unsigned int UINT_STARTADDRESS, unsigned int UINT_ENDADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS) {
	unsigned int ADD_ADDRESS;

	// Verifies address delta
	if (UINT_ENDADDRESS - UINT_STARTADDRESS > 0x00100000) {
		return 0;
	}

	// Sets start address
	ADD_ADDRESS = UINT_STARTADDRESS;

	// Map loop
	while (ADD_ADDRESS < UINT_ENDADDRESS) {

		// Maps large page
		if (!MMU_SLTRANSLATIONTABLE_MAPLARGEPAGE(PTR_SLTRANSLATIONTABLE, ADD_ADDRESS, UINT_MEMORYTYPE, UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY, UINT_SHAREABILITY, UINT_SCOPE, UINT_ACCESS)) {
			return 0;
		}

		// Moves address
		ADD_ADDRESS += 64 * 1024;
	}
	return 1;
}

// Startup method
void MMU_STARTUP(unsigned int *PTR_FLTRANSLATIONTABLE_REGION_POINTER, unsigned int SIZ_FLTRANSLATIONTABLE_REGION_SIZE, unsigned int *PTR_SLTRANSLATIONTABLE_REGION_POINTER, unsigned int SIZ_SLTRANSLATIONTABLE_REGION_SIZE) {
	MMU_INFORMATION_TYPE *MMU_INFORMATION;

	// Gets MMU information
	MMU_INFORMATION = &CORE_MMU_INFORMATION[0];

	// Initializes FL translation table region
	MMU_INFORMATION->PTR_FLTRANSLATIONTABLE_POINTER = PTR_FLTRANSLATIONTABLE_REGION_POINTER;
	MMU_INFORMATION->SIZ_FLTRANSLATIONTABLE_SIZE = SIZ_FLTRANSLATIONTABLE_REGION_SIZE;
	MMU_INFORMATION->SIZ_FLTRANSLATIONTABLE_FULL = 0;

	// Initializes SL translation table region
	MMU_INFORMATION->PTR_SLTRANSLATIONTABLE_POINTER = PTR_SLTRANSLATIONTABLE_REGION_POINTER;
	MMU_INFORMATION->SIZ_SLTRANSLATIONTABLE_SIZE = SIZ_SLTRANSLATIONTABLE_REGION_SIZE;
	MMU_INFORMATION->SIZ_SLTRANSLATIONTABLE_FULL = 0;

	// Disables MMU
	CP15MMUDisable();

	// Sets domain access rights
	CP15DomainAccessClientSet();

	// Disables TEX remapping and alignment check
	CP15ControlFeatureDisable(CP15_CONTROL_TEXREMAP | CP15_CONTROL_ALIGN_CHCK);

	// Enables access flag usage
	CP15ControlFeatureEnable(CP15_CONTROL_ACCESSFLAG);
}

// Map region method
unsigned char MMU_MAPREGION(unsigned int *PTR_FLTRANSLATIONTABLE, unsigned int UINT_STARTADDRESS, unsigned int UINT_ENDADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SECURITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS) {
	unsigned int *PTR_SLTRANSLATIONTABLE;
	unsigned int UINT_PAGESSIZE;
	unsigned int UINT_ADDRESS;
	unsigned int *PTR_ENTRY;
	unsigned int UINT_ENTRY;

	// Verifies start address 4KB alignment
	if ((UINT_STARTADDRESS & 0xFFFFF000) != UINT_STARTADDRESS) {
		return 0;
	}

	// Verifies end address 4KB alignment
	if ((UINT_ENDADDRESS & 0xFFFFF000) != UINT_ENDADDRESS) {
		return 0;
	}

	// Calculates small pages (4KB), limited to large page (64KB)
	UINT_PAGESSIZE = 0x10000 - (UINT_STARTADDRESS & 0xFFFF);

	// Verifies small pages
	if ((UINT_PAGESSIZE >= 0x1000) && (UINT_PAGESSIZE < 0x10000) && (UINT_PAGESSIZE < (UINT_ENDADDRESS - UINT_STARTADDRESS))) {

		// Calculates address
		UINT_ADDRESS = UINT_STARTADDRESS & 0xFFF00000;

		// Points entry
		PTR_ENTRY = PTR_FLTRANSLATIONTABLE + (UINT_ADDRESS >> 20);

		// Reads entry
		UINT_ENTRY = *PTR_ENTRY;

		// Gets SL translation table address
		PTR_SLTRANSLATIONTABLE = (unsigned int *) (UINT_ENTRY & 0xFFFFFC00);

		// Verifies fault entry
		if ((UINT_ENTRY & 0x00000003) == 0x0) {

			// Allocates SL translation table
			if (!MMU_SLTRANSLATIONTABLE_ALLOCATE(&PTR_SLTRANSLATIONTABLE)) {
				return 0;
			}

			// Maps SL translation table
			if (!MMU_FLTRANSLATIONTABLE_MAPSLTRANSLATIONTABLE(PTR_FLTRANSLATIONTABLE, UINT_ADDRESS, PTR_SLTRANSLATIONTABLE, UINT_SECURITY)) {
				return 0;
			}

			// Rereads entry
			UINT_ENTRY = *PTR_ENTRY;

			// Gets SL translation table address
			PTR_SLTRANSLATIONTABLE = (unsigned int *) (UINT_ENTRY & 0xFFFFFC00);
		}

		// Verifies SL translation table entry
		if ((UINT_ENTRY & 0x00000003) != 0x1) {
			return 0;
		}

		// Maps small pages
		if (!MMU_SLTRANSLATIONTABLE_MAPSMALLPAGES(PTR_SLTRANSLATIONTABLE, UINT_STARTADDRESS, UINT_STARTADDRESS + UINT_PAGESSIZE, UINT_MEMORYTYPE, UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY, UINT_SHAREABILITY, UINT_SCOPE, UINT_ACCESS)) {
			return 0;
		}

		// Sets start address
		UINT_STARTADDRESS = UINT_STARTADDRESS + UINT_PAGESSIZE;
	}

	// Calculates large pages (64KB), limited to section (1MB)
	UINT_PAGESSIZE = 0x100000 - (UINT_STARTADDRESS & 0xFFFFF);

	// Verifies large pages
	if ((UINT_PAGESSIZE >= 0x10000) && (UINT_PAGESSIZE < 0x100000) && (UINT_PAGESSIZE < (UINT_ENDADDRESS - UINT_STARTADDRESS))) {

		// Calculates address
		UINT_ADDRESS = UINT_STARTADDRESS & 0xFFF00000;

		// Points entry
		PTR_ENTRY = PTR_FLTRANSLATIONTABLE + (UINT_ADDRESS >> 20);

		// Reads entry
		UINT_ENTRY = *PTR_ENTRY;

		// Gets SL translation table address
		PTR_SLTRANSLATIONTABLE = (unsigned int *) (UINT_ENTRY & 0xFFFFFC00);

		// Verifies fault entry
		if ((UINT_ENTRY & 0x00000003) == 0x0) {

			// Allocates SL translation table
			if (!MMU_SLTRANSLATIONTABLE_ALLOCATE(&PTR_SLTRANSLATIONTABLE)) {
				return 0;
			}

			// Maps SL translation table
			if (!MMU_FLTRANSLATIONTABLE_MAPSLTRANSLATIONTABLE(PTR_FLTRANSLATIONTABLE, UINT_ADDRESS, PTR_SLTRANSLATIONTABLE, UINT_SECURITY)) {
				return 0;
			}

			// Rereads entry
			UINT_ENTRY = *PTR_ENTRY;

			// Gets SL translation table address
			PTR_SLTRANSLATIONTABLE = (unsigned int *) (UINT_ENTRY & 0xFFFFFC00);
		}

		// Verifies SL translation table entry
		if ((UINT_ENTRY & 0x00000003) != 0x1) {
			return 0;
		}

		// Maps large pages
		if (!MMU_SLTRANSLATIONTABLE_MAPLARGEPAGES(PTR_SLTRANSLATIONTABLE, UINT_STARTADDRESS, UINT_STARTADDRESS + UINT_PAGESSIZE, UINT_MEMORYTYPE, UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY, UINT_SHAREABILITY, UINT_SCOPE, UINT_ACCESS)) {
			return 0;
		}

		// Sets start address
		UINT_STARTADDRESS = UINT_STARTADDRESS + UINT_PAGESSIZE;
	}

	// Calculates sections (1MB), limited to supersection (16MB)
	UINT_PAGESSIZE = 0x1000000 - (UINT_STARTADDRESS & 0xFFFFFF);

	// Verifies sections
	if ((UINT_PAGESSIZE >= 0x100000) && (UINT_PAGESSIZE < 0x1000000) && (UINT_PAGESSIZE < (UINT_ENDADDRESS - UINT_STARTADDRESS))) {

		// Maps sections
		if (!MMU_FLTRANSLATIONTABLE_MAPSECTIONS(PTR_FLTRANSLATIONTABLE, UINT_STARTADDRESS, UINT_STARTADDRESS + UINT_PAGESSIZE, UINT_MEMORYTYPE, UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY, UINT_SHAREABILITY, UINT_SECURITY, UINT_SCOPE, UINT_ACCESS)) {
			return 0;
		}

		// Sets start address
		UINT_STARTADDRESS = UINT_STARTADDRESS + UINT_PAGESSIZE;
	}

	// Calculates supersections (16MB)
	UINT_PAGESSIZE = (UINT_ENDADDRESS - UINT_STARTADDRESS) & 0xFF000000;

	// Verifies supersections
	if ((UINT_PAGESSIZE >= 0x1000000) && (UINT_PAGESSIZE > 0)) {

		// Maps supersections
		if (!MMU_FLTRANSLATIONTABLE_MAPSUPERSECTIONS(PTR_FLTRANSLATIONTABLE, UINT_STARTADDRESS, UINT_STARTADDRESS + UINT_PAGESSIZE, UINT_MEMORYTYPE, UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY, UINT_SHAREABILITY, UINT_SECURITY, UINT_SCOPE, UINT_ACCESS)) {
			return 0;
		}

		// Sets start address
		UINT_STARTADDRESS = UINT_STARTADDRESS + UINT_PAGESSIZE;
	}

	// Calculates sections (1MB)
	UINT_PAGESSIZE = (UINT_ENDADDRESS - UINT_STARTADDRESS) & 0xFFF00000;

	// Verifies sections
	if (UINT_PAGESSIZE >= 0x100000) {

		// Maps sections
		if (!MMU_FLTRANSLATIONTABLE_MAPSECTIONS(PTR_FLTRANSLATIONTABLE, UINT_STARTADDRESS, UINT_STARTADDRESS + UINT_PAGESSIZE, UINT_MEMORYTYPE, UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY, UINT_SHAREABILITY, UINT_SECURITY, UINT_SCOPE, UINT_ACCESS)) {
			return 0;
		}

		// Sets start address
		UINT_STARTADDRESS = UINT_STARTADDRESS + UINT_PAGESSIZE;
	}

	// Calculates large pages (64KB)
	UINT_PAGESSIZE = (UINT_ENDADDRESS - UINT_STARTADDRESS) & 0xFFFF0000;

	// Verifies large pages
	if (UINT_PAGESSIZE >= 0x10000) {

		// Calculates address
		UINT_ADDRESS = UINT_STARTADDRESS & 0xFFF00000;

		// Points entry
		PTR_ENTRY = PTR_FLTRANSLATIONTABLE + (UINT_ADDRESS >> 20);

		// Reads entry
		UINT_ENTRY = *PTR_ENTRY;

		// Gets SL translation table address
		PTR_SLTRANSLATIONTABLE = (unsigned int *) (UINT_ENTRY & 0xFFFFFC00);

		// Verifies fault entry
		if ((UINT_ENTRY & 0x00000003) == 0x0) {

			// Allocates SL translation table
			if (!MMU_SLTRANSLATIONTABLE_ALLOCATE(&PTR_SLTRANSLATIONTABLE)) {
				return 0;
			}

			// Maps SL translation table
			if (!MMU_FLTRANSLATIONTABLE_MAPSLTRANSLATIONTABLE(PTR_FLTRANSLATIONTABLE, UINT_ADDRESS, PTR_SLTRANSLATIONTABLE, UINT_SECURITY)) {
				return 0;
			}

			// Rereads entry
			UINT_ENTRY = *PTR_ENTRY;

			// Gets SL translation table address
			PTR_SLTRANSLATIONTABLE = (unsigned int *) (UINT_ENTRY & 0xFFFFFC00);
		}

		// Verifies SL translation table entry
		if ((UINT_ENTRY & 0x00000003) != 0x1) {
			return 0;
		}

		// Maps large pages
		if (!MMU_SLTRANSLATIONTABLE_MAPLARGEPAGES(PTR_SLTRANSLATIONTABLE, UINT_STARTADDRESS, UINT_STARTADDRESS + UINT_PAGESSIZE, UINT_MEMORYTYPE, UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY, UINT_SHAREABILITY, UINT_SCOPE, UINT_ACCESS)) {
			return 0;
		}

		// Sets start address
		UINT_STARTADDRESS = UINT_STARTADDRESS + UINT_PAGESSIZE;
	}

	// Calculates small pages (4KB)
	UINT_PAGESSIZE = (UINT_ENDADDRESS - UINT_STARTADDRESS) & 0xFFFFF000;

	// Verifies small pages
	if (UINT_PAGESSIZE >= 0x1000) {

		// Calculates address
		UINT_ADDRESS = UINT_STARTADDRESS & 0xFFF00000;

		// Points entry
		PTR_ENTRY = PTR_FLTRANSLATIONTABLE + (UINT_ADDRESS >> 20);

		// Reads entry
		UINT_ENTRY = *PTR_ENTRY;

		// Gets SL translation table address
		PTR_SLTRANSLATIONTABLE = (unsigned int *) (UINT_ENTRY & 0xFFFFFC00);

		// Verifies fault entry
		if ((UINT_ENTRY & 0x00000003) == 0x0) {

			// Allocates SL translation table
			if (!MMU_SLTRANSLATIONTABLE_ALLOCATE(&PTR_SLTRANSLATIONTABLE)) {
				return 0;
			}

			// Maps SL translation table
			if (!MMU_FLTRANSLATIONTABLE_MAPSLTRANSLATIONTABLE(PTR_FLTRANSLATIONTABLE, UINT_ADDRESS, PTR_SLTRANSLATIONTABLE, UINT_SECURITY)) {
				return 0;
			}

			// Rereads entry
			UINT_ENTRY = *PTR_ENTRY;

			// Gets SL translation table address
			PTR_SLTRANSLATIONTABLE = (unsigned int *) (UINT_ENTRY & 0xFFFFFC00);
		}

		// Verifies SL translation table entry
		if ((UINT_ENTRY & 0x00000003) != 0x1) {
			return 0;
		}

		// Maps small pages
		if (!MMU_SLTRANSLATIONTABLE_MAPSMALLPAGES(PTR_SLTRANSLATIONTABLE, UINT_STARTADDRESS, UINT_STARTADDRESS + UINT_PAGESSIZE, UINT_MEMORYTYPE, UINT_INNERCACHEPOLICY, UINT_OUTERCACHEPOLICY, UINT_SHAREABILITY, UINT_SCOPE, UINT_ACCESS)) {
			return 0;
		}

		// Sets start address
		UINT_STARTADDRESS = UINT_STARTADDRESS + UINT_PAGESSIZE;
	}

	// Verifies addresses
	if (UINT_STARTADDRESS != UINT_ENDADDRESS) {
		return 0;
	}

	// Success
	return 1;
}

// Enable method
void MMU_ENABLE(portADDRESS PTR_FLTRANSLATIONTABLE0, portADDRESS PTR_FLTRANSLATIONTABLE1, unsigned int UINT_CONTEXTID) {

	// Configures TTB control register
	// TTBR1 covers only addresses 0x80000000 - 0xFFFFFFFF
	CP15TtbCtlConfig(0x1);

	// Sets TTBR0
	CP15Ttbr0Set((unsigned int) PTR_FLTRANSLATIONTABLE0);

	// Sets TTBR1
	CP15Ttbr1Set((unsigned int) PTR_FLTRANSLATIONTABLE1);

	// Invalidates TLB entries
	CP15TlbInvalidate();

	// Sets context identifier
	CP15SetContextID(UINT_CONTEXTID);

	// Enables MMU
	CP15MMUEnable();
}
