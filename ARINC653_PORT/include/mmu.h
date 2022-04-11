#ifndef MMU_H_
#define MMU_H_

// Includes
#include "arinc653_port_types.h"
#include "cpu.h"
#include "cp15.h"

// Core count
#define MMU_CORECOUNT CPU_CORECOUNT

// MMU information
struct _MMU_INFORMATION_TYPE {
	unsigned int *PTR_FLTRANSLATIONTABLE_POINTER; // First level translation table pointer
	unsigned int SIZ_FLTRANSLATIONTABLE_SIZE; // First level translation table size
	unsigned int SIZ_FLTRANSLATIONTABLE_FULL; // First level translation table full size
	unsigned int *PTR_SLTRANSLATIONTABLE_POINTER; // Second level translation table pointer
	unsigned int SIZ_SLTRANSLATIONTABLE_SIZE; // Second level translation table size
	unsigned int SIZ_SLTRANSLATIONTABLE_FULL; // Second level translation table full size
};
typedef struct _MMU_INFORMATION_TYPE MMU_INFORMATION_TYPE;

// MMU information (one per core)
extern MMU_INFORMATION_TYPE CORE_MMU_INFORMATION[MMU_CORECOUNT];

// -------------------- FL TRANSLATION TABLE START --------------------

// FL translation table entry count
#define MMU_FLTRANSLATIONTABLE_ENTRYCOUNT 4096

// FL translation table entry size
#define MMU_FLTRANSLATIONTABLE_ENTRYSIZE 4

// FL translation table alignment
#define MMU_FLTRANSLATIONTABLE_ALIGNMENT 16384

// FL translation table allocation method
unsigned char MMU_FLTRANSLATIONTABLE_ALLOCATE(unsigned int **PTR_FLTRANSLATIONTABLE);

// FL TRANSLATION TABLE - SECTION DEFINITIONS

// Define TEX, C and B
// FL translation table - Section - Cache policy - Non-cacheable
#define MMU_FLTRANSLATIONTABLE_SECTION_CACHEPOLICY_NONCACHEABLE 0x0
// FL translation table - Section - Cache policy - Write back, write allocate
#define MMU_FLTRANSLATIONTABLE_SECTION_CACHEPOLICY_WB_WA 0x1
// FL translation table - Section - Cache policy - Write through, no write allocate
#define MMU_FLTRANSLATIONTABLE_SECTION_CACHEPOLICY_WT_NOWA 0x2
// FL translation table - Section - Cache policy - Write back, no write allocate
#define MMU_FLTRANSLATIONTABLE_SECTION_CACHEPOLICY_WB_NOWA 0x3

// Define TEX, C, B and S
// FL translation table - Section - Memory type - Shareable strongly ordered
#define MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_STRONGLYORDERED_SHAREABLE 0x00000000
// FL translation table - Section - Memory type - Shareable device
#define MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_DEVICE_SHAREABLE 0x00000004
// FL translation table - Section - Memory type - Normal write through, no write allocate
#define MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_NORMAL_WT_NOWA 0x00000008
// FL translation table - Section - Memory type - Normal write back, no write allocate
#define MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_NORMAL_WB_NOWA 0x0000000C
// FL translation table - Section - Memory type - Normal non cacheable
#define MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_NORMAL_NONCACHEABLE 0x00001000
// FL translation table - Section - Memory type - Normal write back, write allocate
#define MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_NORMAL_WB_WA 0x0000100C
// FL translation table - Section - Memory type - Non-shareable device
#define MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_DEVICE_NONSHAREABLE 0x00002000
// FL translation table - Section - Memory type - Cacheable normal
#define MMU_FLTRANSLATIONTABLE_SECTION_MEMORYTYPE_NORMAL_CACHEABLE(INNERCACHEPOLICY, OUTERCACHEPOLICY) (0x00004000 | (OUTERCACHEPOLICY << 12) | (INNERCACHEPOLICY << 2))

// Define S
// FL translation table - Section - Shareability - Shareable
#define MMU_FLTRANSLATIONTABLE_SECTION_SHAREABILITY_SHAREABLE 0x00010000
// FL translation table - Section - Shareability - Non-shareable
#define MMU_FLTRANSLATIONTABLE_SECTION_SHAREABILITY_NONSHAREABLE 0x00000000

// Define NS
// FL translation table - Section - Security - Secure
#define MMU_FLTRANSLATIONTABLE_SECTION_SECURITY_SECURE 0x00000000
// FL translation table - Section - Security - Non-secure
#define MMU_FLTRANSLATIONTABLE_SECTION_SECURITY_NONSECURE 0x00080000

// Define nG
// FL translation table - Section - Scope - Global
#define MMU_FLTRANSLATIONTABLE_SECTION_SCOPE_GLOBAL 0x00000000
// FL translation table - Section - Scope - Non-global
#define MMU_FLTRANSLATIONTABLE_SECTION_SCOPE_NONGLOBAL 0x00020000

// Define AP[2:1] and AP[0] assuming SCTLR.AFE = 1
// FL translation table - Section - Access - PL1 none, PL0 none
#define MMU_FLTRANSLATIONTABLE_SECTION_ACCESS_PL1_NO_PL0_NO 0x00000000
// FL translation table - Section - Access - PL1 read/write, PL0 none
#define MMU_FLTRANSLATIONTABLE_SECTION_ACCESS_PL1_RW_PL0_NO 0x00000400
// FL translation table - Section - Access - PL1 read/write, PL0 read/write
#define MMU_FLTRANSLATIONTABLE_SECTION_ACCESS_PL1_RW_PL0_RW 0x00000C00
// FL translation table - Section - Access - PL1 read, PL0 none
#define MMU_FLTRANSLATIONTABLE_SECTION_ACCESS_PL1_RO_PL0_NO 0x00008400
// FL translation table - Section - Access - PL1 read, PL0 read
#define MMU_FLTRANSLATIONTABLE_SECTION_ACCESS_PL1_RO_PL0_RO 0x00008C00
// FL translation table - Section - Access - Execute never (combine with OR)
#define MMU_FLTRANSLATIONTABLE_SECTION_ACCESS_EXECUTENEVER 0x00000010

// Map FL translation table section (1MB) method
unsigned char MMU_FLTRANSLATIONTABLE_MAPSECTION(unsigned int *PTR_FLTRANSLATIONTABLE, unsigned int UINT_STARTADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SECURITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS);

// Map FL translation table sections (1MB) method
unsigned char MMU_FLTRANSLATIONTABLE_MAPSECTIONS(unsigned int *PTR_FLTRANSLATIONTABLE, unsigned int UINT_STARTADDRESS, unsigned int UINT_ENDADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SECURITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS);

// FL TRANSLATION TABLE - SUPERSECTION DEFINITIONS

// Define TEX, C and B
// FL translation table - Supersection - Cache policy - Non-cacheable
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_CACHEPOLICY_NONCACHEABLE 0x0
// FL translation table - Supersection - Cache policy - Write back, write allocate
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_CACHEPOLICY_WB_WA 0x1
// FL translation table - Supersection - Cache policy - Write through, no write allocate
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_CACHEPOLICY_WT_NOWA 0x2
// FL translation table - Supersection - Cache policy - Write back, no write allocate
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_CACHEPOLICY_WB_NOWA 0x3

// Define TEX, C, B and S
// FL translation table - Supersection - Memory type - Shareable strongly ordered
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_STRONGLYORDERED_SHAREABLE 0x00000000
// FL translation table - Supersection - Memory type - Shareable device
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_DEVICE_SHAREABLE 0x00000004
// FL translation table - Supersection - Memory type - Normal write through, no write allocate
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_NORMAL_WT_NOWA 0x00000008
// FL translation table - Supersection - Memory type - Normal write back, no write allocate
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_NORMAL_WB_NOWA 0x0000000C
// FL translation table - Supersection - Memory type - Normal non cacheable
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_NORMAL_NONCACHEABLE 0x00001000
// FL translation table - Supersection - Memory type - Normal write back, write allocate
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_NORMAL_WB_WA 0x0000100C
// FL translation table - Supersection - Memory type - Non-shareable device
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_DEVICE_NONSHAREABLE 0x00002000
// FL translation table - Supersection - Memory type - Cacheable normal
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_MEMORYTYPE_NORMAL_CACHEABLE(INNERCACHEPOLICY, OUTERCACHEPOLICY) (0x00004000 | (OUTERCACHEPOLICY << 12) | (INNERCACHEPOLICY << 2))

// Define S
// FL translation table - Supersection - Shareability - Shareable
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_SHAREABILITY_SHAREABLE 0x00010000
// FL translation table - Supersection - Shareability - Non-shareable
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_SHAREABILITY_NONSHAREABLE 0x00000000

// Define NS
// FL translation table - Supersection - Security - Secure
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_SECURITY_SECURE 0x00000000
// FL translation table - Supersection - Security - Non-secure
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_SECURITY_NONSECURE 0x00080000

// Define nG
// FL translation table - Supersection - Scope - Global
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_SCOPE_GLOBAL 0x00000000
// FL translation table - Supersection - Scope - Non-global
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_SCOPE_NONGLOBAL 0x00020000

// Define AP[2:1] and AP[0] assuming SCTLR.AFE = 1
// FL translation table - Supersection - Access - PL1 none, PL0 none
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_ACCESS_PL1_NO_PL0_NO 0x00000000
// FL translation table - Supersection - Access - PL1 read/write, PL0 none
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_ACCESS_PL1_RW_PL0_NO 0x00000400
// FL translation table - Supersection - Access - PL1 read/write, PL0 read/write
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_ACCESS_PL1_RW_PL0_RW 0x00000C00
// FL translation table - Supersection - Access - PL1 read, PL0 none
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_ACCESS_PL1_RO_PL0_NO 0x00008400
// FL translation table - Supersection - Access - PL1 read, PL0 read
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_ACCESS_PL1_RO_PL0_RO 0x00008C00
// FL translation table - Supersection - Access - Execute never (combine with OR)
#define MMU_FLTRANSLATIONTABLE_SUPERSECTION_ACCESS_EXECUTENEVER 0x00000010

// Map FL translation table supersection (16MB) method
unsigned char MMU_FLTRANSLATIONTABLE_MAPSUPERSECTION(unsigned int *PTR_FLTRANSLATIONTABLE, unsigned int UINT_STARTADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SECURITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS);

// Map FL translation table supersections (16MB) method
unsigned char MMU_FLTRANSLATIONTABLE_MAPSUPERSECTIONS(unsigned int *PTR_FLTRANSLATIONTABLE, unsigned int UINT_STARTADDRESS, unsigned int UINT_ENDADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SECURITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS);

// FL TRANSLATION TABLE - SL TRANSLATION TABLE DEFINITIONS

// Define NS
// FL translation table - SL translation table - Security - Secure
#define MMU_FLTRANSLATIONTABLE_SLTRANSLATIONTABLE_SECURITY_SECURE 0x00000000
// FL translation table - SL translation table - Security - Non-secure
#define MMU_FLTRANSLATIONTABLE_SLTRANSLATIONTABLE_SECURITY_NONSECURE 0x00000008

// Map FL translation table SL translation table method
unsigned char MMU_FLTRANSLATIONTABLE_MAPSLTRANSLATIONTABLE(unsigned int *PTR_FLTRANSLATIONTABLE, unsigned int UINT_ADDRESS, unsigned int *PTR_SLTRANSLATIONTABLE, unsigned int UINT_SECURITY);

// -------------------- FL TRANSLATION TABLE END --------------------

// -------------------- SL TRANSLATION TABLE START --------------------

// SL translation table entry count
#define MMU_SLTRANSLATIONTABLE_ENTRYCOUNT 256

// SL translation table entry size
#define MMU_SLTRANSLATIONTABLE_ENTRYSIZE 4

// SL translation table alignment
#define MMU_SLTRANSLATIONTABLE_ALIGNMENT 512

// SL translation table allocation method
unsigned char MMU_SLTRANSLATIONTABLE_ALLOCATE(unsigned int **PTR_SLTRANSLATIONTABLE);

// SL TRANSLATION TABLE - SMALL PAGE DEFINITIONS

// Define TEX, C and B
// SL translation table - Small page - Cache policy - Non-cacheable
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_CACHEPOLICY_NONCACHEABLE 0x0
// SL translation table - Small page - Cache policy - Write back, write allocate
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_CACHEPOLICY_WB_WA 0x1
// SL translation table - Small page - Cache policy - Write through, no write allocate
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_CACHEPOLICY_WT_NOWA 0x2
// SL translation table - Small page - Cache policy - Write back, no write allocate
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_CACHEPOLICY_WB_NOWA 0x3

// Define TEX, C, B and S
// SL translation table - Small page - Memory type - Shareable strongly ordered
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_STRONGLYORDERED_SHAREABLE 0x00000000
// SL translation table - Small page - Memory type - Shareable device
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_DEVICE_SHAREABLE 0x00000004
// SL translation table - Small page - Memory type - Normal write through, no write allocate
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_NORMAL_WT_NOWA 0x00000008
// SL translation table - Small page - Memory type - Normal write back, no write allocate
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_NORMAL_WB_NOWA 0x0000000C
// SL translation table - Small page - Memory type - Normal non cacheable
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_NORMAL_NONCACHEABLE 0x00000040
// SL translation table - Small page - Memory type - Normal write back, write allocate
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_NORMAL_WB_WA 0x0000004C
// SL translation table - Small page - Memory type - Non-shareable device
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_DEVICE_NONSHAREABLE 0x00000080
// SL translation table - Small page - Memory type - Cacheable normal
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_MEMORYTYPE_NORMAL_CACHEABLE(INNERCACHEPOLICY, OUTERCACHEPOLICY) (0x00000100 | (OUTERCACHEPOLICY << 6) | (INNERCACHEPOLICY << 2))

// Define S
// SL translation table - Small page - Shareability - Shareable
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_SHAREABILITY_SHAREABLE 0x00000400
// SL translation table - Small page - Shareability - Non-shareable
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_SHAREABILITY_NONSHAREABLE 0x00000000

// Define nG
// SL translation table - Small page - Scope - Global
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_SCOPE_GLOBAL 0x00000000
// SL translation table - Small page - Scope - Non-global
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_SCOPE_NONGLOBAL 0x00000800

// Define AP[2:1] and AP[0] assuming SCTLR.AFE = 1
// SL translation table - Small page - Access - PL1 none, PL0 none
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_ACCESS_PL1_NO_PL0_NO 0x00000000
// SL translation table - Small page - Access - PL1 read/write, PL0 none
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_ACCESS_PL1_RW_PL0_NO 0x00000010
// SL translation table - Small page - Access - PL1 read/write, PL0 read/write
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_ACCESS_PL1_RW_PL0_RW 0x00000030
// SL translation table - Small page - Access - PL1 read, PL0 none
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_ACCESS_PL1_RO_PL0_NO 0x00000210
// SL translation table - Small page - Access - PL1 read, PL0 read
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_ACCESS_PL1_RO_PL0_RO 0x00000230
// SL translation table - Small page - Access - Execute never (combine with OR)
#define MMU_SLTRANSLATIONTABLE_SMALLPAGE_ACCESS_EXECUTENEVER 0x00000001

// Map SL translation table small page (4KB) method
unsigned char MMU_SLTRANSLATIONTABLE_MAPSMALLPAGE(unsigned int *PTR_SLTRANSLATIONTABLE, unsigned int UINT_ADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS);

// Map SL translation table small pages (4KB) method
unsigned char MMU_SLTRANSLATIONTABLE_MAPSMALLPAGES(unsigned int *PTR_SLTRANSLATIONTABLE, unsigned int UINT_STARTADDRESS, unsigned int UINT_ENDADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS);

// SL TRANSLATION TABLE - LARGE PAGE DEFINITIONS

// Define TEX, C and B
// SL translation table - Large page - Cache policy - Non-cacheable
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_CACHEPOLICY_NONCACHEABLE 0x0
// SL translation table - Large page - Cache policy - Write back, write allocate
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_CACHEPOLICY_WB_WA 0x1
// SL translation table - Large page - Cache policy - Write through, no write allocate
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_CACHEPOLICY_WT_NOWA 0x2
// SL translation table - Large page - Cache policy - Write back, no write allocate
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_CACHEPOLICY_WB_NOWA 0x3

// Define TEX, C, B and S
// SL translation table - Large page - Memory type - Shareable strongly ordered
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_STRONGLYORDERED_SHAREABLE 0x00000000
// SL translation table - Large page - Memory type - Shareable device
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_DEVICE_SHAREABLE 0x00000004
// SL translation table - Large page - Memory type - Normal write through, no write allocate
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_NORMAL_WT_NOWA 0x00000008
// SL translation table - Large page - Memory type - Normal write back, no write allocate
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_NORMAL_WB_NOWA 0x0000000C
// SL translation table - Large page - Memory type - Normal non cacheable
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_NORMAL_NONCACHEABLE 0x00001000
// SL translation table - Large page - Memory type - Normal write back, write allocate
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_NORMAL_WB_WA 0x0000100C
// SL translation table - Large page - Memory type - Non-shareable device
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_DEVICE_NONSHAREABLE 0x00002000
// SL translation table - Large page - Memory type - Cacheable normal
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_MEMORYTYPE_NORMAL_CACHEABLE(INNERCACHEPOLICY, OUTERCACHEPOLICY) (0x00004000 | (OUTERCACHEPOLICY << 12) | (INNERCACHEPOLICY << 2))

// Define S
// SL translation table - Large page - Shareability - Shareable
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_SHAREABILITY_SHAREABLE 0x00000400
// SL translation table - Large page - Shareability - Non-shareable
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_SHAREABILITY_NONSHAREABLE 0x00000000

// Define nG
// SL translation table - Large page - Scope - Global
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_SCOPE_GLOBAL 0x00000000
// SL translation table - Large page - Scope - Non-global
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_SCOPE_NONGLOBAL 0x00000800

// Define AP[2:1] and AP[0] assuming SCTLR.AFE = 1
// SL translation table - Large page - Access - PL1 none, PL0 none
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_ACCESS_PL1_NO_PL0_NO 0x00000000
// SL translation table - Large page - Access - PL1 read/write, PL0 none
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_ACCESS_PL1_RW_PL0_NO 0x00000010
// SL translation table - Large page - Access - PL1 read/write, PL0 read/write
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_ACCESS_PL1_RW_PL0_RW 0x00000030
// SL translation table - Large page - Access - PL1 read, PL0 none
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_ACCESS_PL1_RO_PL0_NO 0x00000210
// SL translation table - Large page - Access - PL1 read, PL0 read
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_ACCESS_PL1_RO_PL0_RO 0x00000230
// SL translation table - Large page - Access - Execute never (combine with OR)
#define MMU_SLTRANSLATIONTABLE_LARGEPAGE_ACCESS_EXECUTENEVER 0x00008000

// Map SL translation table large page (64KB) method
unsigned char MMU_SLTRANSLATIONTABLE_MAPLARGEPAGE(unsigned int *PTR_SLTRANSLATIONTABLE, unsigned int UINT_ADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS);

// Map SL translation table large pages (64KB) method
unsigned char MMU_SLTRANSLATIONTABLE_MAPLARGEPAGES(unsigned int *PTR_SLTRANSLATIONTABLE, unsigned int UINT_STARTADDRESS, unsigned int UINT_ENDADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS);

// -------------------- SL TRANSLATION TABLE END --------------------

// -------------------- GENERAL START --------------------

// Cache policy - Non-cacheable
#define MMU_CACHEPOLICY_NONCACHEABLE 0
// Cache policy - Write back, write allocate
#define MMU_CACHEPOLICY_WB_WA 1
// Cache policy - Write through, no write allocate
#define MMU_CACHEPOLICY_WT_NOWA 2
// Cache policy - Write back, no write allocate
#define MMU_CACHEPOLICY_WB_NOWA 3

// Shareability - Shareable
#define MMU_SHAREABILITY_SHAREABLE 0
// Shareability - Non-shareable
#define MMU_SHAREABILITY_NONSHAREABLE 1

// Memory type - Shareable strongly ordered
#define MMU_MEMORYTYPE_STRONGLYORDERED_SHAREABLE 0
// Memory type - Shareable device
#define MMU_MEMORYTYPE_DEVICE_SHAREABLE 1
// Memory type - Normal write through, no write allocate
#define MMU_MEMORYTYPE_NORMAL_WT_NOWA 2
// Memory type - Normal write back, no write allocate
#define MMU_MEMORYTYPE_NORMAL_WB_NOWA 3
// Memory type - Normal non cacheable
#define MMU_MEMORYTYPE_NORMAL_NONCACHEABLE 4
// Memory type - Normal write back, write allocate
#define MMU_MEMORYTYPE_NORMAL_WB_WA 5
// Memory type - Non-shareable device
#define MMU_MEMORYTYPE_DEVICE_NONSHAREABLE 6
// Memory type - Cacheable normal
#define MMU_MEMORYTYPE_NORMAL_CACHEABLE 7

// Security - Secure
#define MMU_SECURITY_SECURE 0
// Security - Non-secure
#define MMU_SECURITY_NONSECURE 1

// Scope - Global
#define MMU_SCOPE_GLOBAL 0
// Scope - Non-global
#define MMU_SCOPE_NONGLOBAL 1

// Access - PL1 none, PL0 none
#define MMU_ACCESS_PL1_NO_PL0_NO 0x00
// Access - PL1 read/write, PL0 none
#define MMU_ACCESS_PL1_RW_PL0_NO 0x01
// Access - PL1 read/write, PL0 read/write
#define MMU_ACCESS_PL1_RW_PL0_RW 0x02
// Access - PL1 read, PL0 none
#define MMU_ACCESS_PL1_RO_PL0_NO 0x03
// Access - PL1 read, PL0 read
#define MMU_ACCESS_PL1_RO_PL0_RO 0x04
// Access - Execute never (combine with OR)
#define MMU_ACCESS_EXECUTENEVER 0x10

// Startup method
void MMU_STARTUP(unsigned int *PTR_FLTRANSLATIONTABLE_REGION_POINTER, unsigned int SIZ_FLTRANSLATIONTABLE_REGION_SIZE, unsigned int *PTR_SLTRANSLATIONTABLE_REGION_POINTER, unsigned int SIZ_SLTRANSLATIONTABLE_REGION_SIZE);

// Map region method
unsigned char MMU_MAPREGION(unsigned int *PTR_FLTRANSLATIONTABLE, unsigned int UINT_STARTADDRESS, unsigned int UINT_ENDADDRESS, unsigned int UINT_MEMORYTYPE, unsigned int UINT_INNERCACHEPOLICY, unsigned int UINT_OUTERCACHEPOLICY, unsigned int UINT_SHAREABILITY, unsigned int UINT_SECURITY, unsigned int UINT_SCOPE, unsigned int UINT_ACCESS);

// Enable method
void MMU_ENABLE(portADDRESS PTR_FLTRANSLATIONTABLE0, portADDRESS PTR_FLTRANSLATIONTABLE1, unsigned int UINT_CONTEXTID);

// -------------------- GENERAL END --------------------

#endif
