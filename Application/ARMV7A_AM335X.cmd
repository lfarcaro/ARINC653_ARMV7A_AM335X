// Reset handler
-e PORT_RESETHANDLER

// Suppresses #10063-D warning
--diag_suppress=10063

// Suppresses #10068-D warning
--diag_suppress=10068

// Core #0 module stack region
_core0_module_stack_start = 0x80032000;
_core0_module_stack_end = 0x8002E000;

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
	DDR_SYSTEM_BOOT: o = 0x80000000 l = 0x00001000 /* Boot region */
	DDR_SYSTEM_CODE: o = 0x80001000 l = 0x00020000 /* Code region */
	DDR_SYSTEM_DATA: o = 0x80021000 l = 0x00004000 /* Data region */

	/* MODULE module regions */
	DDR_MODULE_MODULE_CODE: o = 0x80025000 l = 0x00001000 /* Code region */
	DDR_MODULE_MODULE_DATA: o = 0x80026000 l = 0x00001000 /* Data region */

	/* PARTITION1 partition regions */
	DDR_PARTITION1_PARTITION_CODE: o = 0x80027000 l = 0x00001000 /* Code region */
	DDR_PARTITION1_PARTITION_DATA: o = 0x80028000 l = 0x00001000 /* Data region */

	/* PARTITION2 partition regions */
	DDR_PARTITION2_PARTITION_CODE: o = 0x80029000 l = 0x00001000 /* Code region */
	DDR_PARTITION2_PARTITION_DATA: o = 0x8002A000 l = 0x00001000 /* Data region */

	/* PARTITION3 partition regions */
	DDR_PARTITION3_PARTITION_CODE: o = 0x8002B000 l = 0x00001000 /* Code region */
	DDR_PARTITION3_PARTITION_DATA: o = 0x8002C000 l = 0x00001000 /* Data region */

	/* Stack regions */
	/* o = 0x8002D000 l = 0x00001000 - Stack area start separator region */
	DDR_MODULE_MODULE_STACK: o = 0x8002E000 l = 0x00004000 /* MODULE module stack region */
	DDR_PARTITION1_PARTITION_DEFAULTPROCESS_STACK: o = 0x80032000 l = 0x00001000 /* PARTITION1 partition default process stack region */
	DDR_PARTITION1_PARTITION_PROCESS1_PROCESS_STACK: o = 0x80033000 l = 0x00001000 /* PARTITION1 partition PROCESS1 process stack region */
	/* o = 0x80034000 l = 0x00001000 - Process stack separator region */
	DDR_PARTITION1_PARTITION_PROCESS2_PROCESS_STACK: o = 0x80035000 l = 0x00001000 /* PARTITION1 partition PROCESS2 process stack region */
	DDR_PARTITION2_PARTITION_DEFAULTPROCESS_STACK: o = 0x80036000 l = 0x00001000 /* PARTITION2 partition default process stack region */
	DDR_PARTITION3_PARTITION_DEFAULTPROCESS_STACK: o = 0x80037000 l = 0x00001000 /* PARTITION3 partition default process stack region */
	/* o = 0x80038000 l = 0x00001000 - Stack area end separator region */

	/* Heap regions */
	DDR_MODULE_MODULE_HEAP: o = 0x80039000 l = 0x00100000 /* MODULE module heap region */
	DDR_PARTITION1_PARTITION_HEAP: o = 0x80139000 l = 0x00001000 /* PARTITION1 partition heap region */
	DDR_PARTITION2_PARTITION_HEAP: o = 0x8013A000 l = 0x00001000 /* PARTITION2 partition heap region */
	DDR_PARTITION3_PARTITION_HEAP: o = 0x8013B000 l = 0x00001000 /* PARTITION3 partition heap region */

	/* Translation table regions */
	DDR_MODULE_MODULE_FLTRANSLATIONTABLE: o = 0x8013C000 l = 0x00100000 /* MODULE module FL translation table region */
	DDR_MODULE_MODULE_SLTRANSLATIONTABLE: o = 0x8023C000 l = 0x00100000 /* MODULE module SL translation table region */

	/* Data regions' image regions */
	DDR_PARTITION1_PARTITION_DATA_IMAGE: o = 0x8033C000 l = 0x00001000 /* PARTITION1 partition data image region */
	DDR_PARTITION2_PARTITION_DATA_IMAGE: o = 0x8033D000 l = 0x00001000 /* PARTITION2 partition data image region */
	DDR_PARTITION3_PARTITION_DATA_IMAGE: o = 0x8033E000 l = 0x00001000 /* PARTITION3 partition data image region */
}

SECTIONS
{

	/* External DDR Bank 0 */

	/* System region */
	.boot > DDR_SYSTEM_BOOT
	.text > DDR_SYSTEM_CODE
	.bss > DDR_SYSTEM_DATA
		RUN_START(_bss_start)
		RUN_END(_bss_end)
	.const > DDR_SYSTEM_DATA
	.data > DDR_SYSTEM_DATA

	/* System regions */
	.system_code { configuration*.obj(.text), *<configuration*.obj>(.text) } > DDR_SYSTEM_CODE
	.system_data { configuration*.obj(.bss, .const, .data), *<configuration*.obj>(.bss, .const, .data) } > DDR_SYSTEM_DATA

	/* MODULE module regions */
	.module_module_code { module_module*.obj(.text), *<module_module*.obj>(.text) } > DDR_MODULE_MODULE_CODE
	.module_module_data { module_module*.obj(.bss, .const, .data), *<module_module*.obj>(.bss, .const, .data) } > DDR_MODULE_MODULE_DATA

	/* PARTITION1 partition regions */
	.partition1_partition_code { partition1_partition*.obj(.text), *<partition1_partition*.obj>(.text) } > DDR_PARTITION1_PARTITION_CODE
	.partition1_partition_data { partition1_partition*.obj(.bss, .const, .data), *<partition1_partition*.obj>(.bss, .const, .data) } > DDR_PARTITION1_PARTITION_DATA

	/* PARTITION2 partition regions */
	.partition2_partition_code { partition2_partition*.obj(.text), *<partition2_partition*.obj>(.text) } > DDR_PARTITION2_PARTITION_CODE
	.partition2_partition_data { partition2_partition*.obj(.bss, .const, .data), *<partition2_partition*.obj>(.bss, .const, .data) } > DDR_PARTITION2_PARTITION_DATA

	/* PARTITION3 partition regions */
	.partition3_partition_code { partition3_partition*.obj(.text), *<partition3_partition*.obj>(.text) } > DDR_PARTITION3_PARTITION_CODE
	.partition3_partition_data { partition3_partition*.obj(.bss, .const, .data), *<partition3_partition*.obj>(.bss, .const, .data) } > DDR_PARTITION3_PARTITION_DATA
}
