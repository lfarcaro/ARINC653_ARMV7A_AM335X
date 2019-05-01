// Reset handler
-e PORT_RESETHANDLER

// Suppresses #10063-D warning
--diag_suppress=10063

// Suppresses #10068-D warning
--diag_suppress=10068

MEMORY
{

	/* Internal SRAM */

	/* Vector table region */
	SRAM_VECTORTABLE: o = 0x40300000 l = 0x00001000

	/* Peripherals */

	/* Peripherals region */
	PERIPHERALS: o = 0x44000000 l = 0x3C000000

	/* External DDR Bank 0 */

	/* System regions */
	DDR0_SYSTEM_BOOT: o = 0x80000000 l = 0x00001000 /* Boot region */
	DDR0_SYSTEM_CODE: o = 0x80001000 l = 0x00020000 /* Code region */
	DDR0_SYSTEM_DATA: o = 0x80021000 l = 0x00002000 /* Data region */

	/* Module regions */
	DDR0_MODULE_CODE: o = 0x80023000 l = 0x00001000 /* Code region */
	DDR0_MODULE_DATA: o = 0x80024000 l = 0x00001000 /* Data region */

	/* PARTITION1 partition regions */
	DDR0_PARTITION1_PARTITION_CODE: o = 0x80025000 l = 0x00001000 /* Code region */
	DDR0_PARTITION1_PARTITION_DATA: o = 0x80026000 l = 0x00001000 /* Data region */

	/* PARTITION2 partition regions */
	DDR0_PARTITION2_PARTITION_CODE: o = 0x80027000 l = 0x00001000 /* Code region */
	DDR0_PARTITION2_PARTITION_DATA: o = 0x80028000 l = 0x00001000 /* Data region */

	/* PARTITION3 partition regions */
	DDR0_PARTITION3_PARTITION_CODE: o = 0x80029000 l = 0x00001000 /* Code region */
	DDR0_PARTITION3_PARTITION_DATA: o = 0x8002A000 l = 0x00001000 /* Data region */

	/* Stack regions */
	/* o = 0x8002B000 l = 0x00001000 - Stack area start separator region */
	DDR0_SYSTEM_STACK: o = 0x8002C000 l = 0x00004000 /* System stack region */
	DDR0_PARTITION1_PARTITION_DEFAULTPROCESS_STACK: o = 0x80030000 l = 0x00001000 /* PARTITION1 partition default process stack region */
	DDR0_PARTITION2_PARTITION_DEFAULTPROCESS_STACK: o = 0x80031000 l = 0x00001000 /* PARTITION2 partition default process stack region */
	DDR0_PARTITION3_PARTITION_DEFAULTPROCESS_STACK: o = 0x80032000 l = 0x00001000 /* PARTITION3 partition default process stack region */
	/* o = 0x80033000 l = 0x00001000 - Stack area end separator region */

	/* Heap regions */
	DDR0_SYSTEM_HEAP: o = 0x80034000 l = 0x00100000 /* System heap region */
	DDR0_PARTITION1_HEAP: o = 0x80134000 l = 0x00001000 /* PARTITION1 partition heap region */
	DDR0_PARTITION2_HEAP: o = 0x80135000 l = 0x00001000 /* PARTITION2 partition heap region */
	DDR0_PARTITION3_HEAP: o = 0x80136000 l = 0x00001000 /* PARTITION3 partition heap region */

	/* Translation table regions */
	DDR0_SYSTEM_FLTRANSLATIONTABLE: o = 0x80138000 l = 0x00100000 /* System FL translation table region */
	DDR0_SYSTEM_SLTRANSLATIONTABLE: o = 0x80238000 l = 0x00100000 /* System SL translation table region */

	/* Data regions image regions */
	DDR0_PARTITION1_PARTITION_DATA_IMAGE: o = 0x80338000 l = 0x00001000 /* PARTITION1 partition data region image */
	DDR0_PARTITION2_PARTITION_DATA_IMAGE: o = 0x80339000 l = 0x00001000 /* PARTITION2 partition data region image */
	DDR0_PARTITION3_PARTITION_DATA_IMAGE: o = 0x8033A000 l = 0x00001000 /* PARTITION3 partition data region image */
}

SECTIONS
{

	/* External DDR Bank 0 */

	/* System region */
	.boot > DDR0_SYSTEM_BOOT
	.stack > DDR0_SYSTEM_STACK
	.text > DDR0_SYSTEM_CODE
	.bss > DDR0_SYSTEM_DATA
		RUN_START(DDR0_SYSTEM_DATA_BSS_START)
		RUN_END(DDR0_SYSTEM_DATA_BSS_END)
	.const > DDR0_SYSTEM_DATA
	.data > DDR0_SYSTEM_DATA

	/* Module region */
	.module_code { module*.obj(.text), *<module*.obj>(.text), configuration*.obj(.text), *<configuration*.obj>(.text) } > DDR0_MODULE_CODE
	.module_data { module*.obj(.bss, .const, .data), *<module*.obj>(.bss, .const, .data), configuration*.obj(.bss, .const, .data), *<configuration*.obj>(.bss, .const, .data) } > DDR0_MODULE_DATA

	/* PARTITION1 partition region */
	.partition1_code { partition1*.obj(.text), *<partition1*.obj>(.text) } > DDR0_PARTITION1_PARTITION_CODE
	.partition1_data { partition1*.obj(.bss, .const, .data), *<partition1*.obj>(.bss, .const, .data) } > DDR0_PARTITION1_PARTITION_DATA

	/* PARTITION2 partition region */
	.partition2_code { partition2*.obj(.text), *<partition2*.obj>(.text) } > DDR0_PARTITION2_PARTITION_CODE
	.partition2_data { partition2*.obj(.bss, .const, .data), *<partition2*.obj>(.bss, .const, .data) } > DDR0_PARTITION2_PARTITION_DATA

	/* PARTITION3 partition region */
	.partition3_code { partition3*.obj(.text), *<partition3*.obj>(.text) } > DDR0_PARTITION3_PARTITION_CODE
	.partition3_data { partition3*.obj(.bss, .const, .data), *<partition3*.obj>(.bss, .const, .data) } > DDR0_PARTITION3_PARTITION_DATA
}
