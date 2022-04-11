# ARINC653_AM335X
Bare-metal Real-Time Operating System (RTOS) for AM335X processors (BeagleBone / BeagleBone Black) based on the ARINC 653 Part 1 specification.

Compiler version
----------------

The following instructions consider the use of the Code Composer Studio IDE version 10.4.0.00006.

Configuration for compilation
-----------------------------

- Project / Properties / General:
	- Variant = AM3359 [Cortex A]
	- Compiler version = TI v18.12.4.LTS (or equivalent)
	- Output format = eabi (ELF)
	- Device endianness = little
	- Linker command file = ""
	- Runtime support library = "<automatic>"
- Project / Properties / Build / ARM Compiler / Processor Options:
	- Target processor version = 7A8
	- Designate code state = 32
	- Specify floating point support = VFPv3
- Project / Properties / Build / ARM Compiler / Include Options:
	- Add "${workspace_loc:/${ProjName}/Hardware/include}"
	- Add "${workspace_loc:/${ProjName}/ARINC653_PORT/include}"
	- Add "${workspace_loc:/${ProjName}/ARINC653/include}"
	- Add "${workspace_loc:/${ProjName}/Instruments/include}"
	- Add "${workspace_loc:/${ProjName}/Library/usblib/include}" if USB is to be used
- Project / Properties / Build / ARM Compiler / Predefined Symbols:
	- Add "DEBUG"
	- Add "beaglebone"
	- Add "am335x"
	- Add "am3359"
	- Add "MEASURE" if execution times measurement is desired
	- Add "MEASURE_SUPPRESSTICK" if measurements shall not include time consumed by the OS in handling the tick interrupt
	- Add "MEASURE_SUPPRESSOTHERCONTEXTS" if measurements shall not include time consumed in contexts other than the one that requested the measurement (use only together with MEASURE_SUPPRESSTICK)
	- Add "TRACK" if OS-related events' tracking is desired
	- Add "DISABLE_BRANCHPREDICTION" if the branch prediction mechanism shall not be enabled
	- Add "DISABLE_INSTRUCTIONCACHE" if instruction cache shall not be enabled
	- Add "DISABLE_DATACACHE" if data cache shall not be enabled
- Project / Properties / Build / ARM Linker / Advanced Options / Runtime Environment:
	- Set "Initialization model" to "" (none)
- Project / Properties / Build / ARM Compiler / Advanced Options / Diagnostic Options:
	- Add "1557" to "Suppress diagnostic <id> (--diag_suppress, -pds)"
- Project / Properties / CCS Build / Steps / Post-build steps:
	"${CCS_INSTALL_ROOT}/utils/tiobj2bin/tiobj2bin.bat" "${PROJECT_LOC}/${ConfigName}/${ProjName}.out" "${PROJECT_LOC}/${ConfigName}/${ProjName}.bin" "${CG_TOOL_ROOT}/bin/armofd.exe" "${CG_TOOL_ROOT}/bin/armhex.exe" "${CCS_INSTALL_ROOT}/utils/tiobj2bin/mkhex4bin.exe" & "${PROJECT_LOC}/Tools/tiimage.exe" "0x80000000" "NONE" "${PROJECT_LOC}/${ConfigName}/${ProjName}.bin" "${PROJECT_LOC}/${ConfigName}/APP" & copy "${PROJECT_LOC}/${ConfigName}/${ProjName}.bin" "${PROJECT_LOC}/${ConfigName}/APP_XMODEM"
	"${CCS_INSTALL_ROOT}/utils/tiobj2bin/tiobj2bin" "${PROJECT_LOC}/${ConfigName}/${ProjName}.out" "${PROJECT_LOC}/${ConfigName}/${ProjName}.bin" "${CG_TOOL_ROOT}/bin/armofd" "${CG_TOOL_ROOT}/bin/armhex" "${CCS_INSTALL_ROOT}/utils/tiobj2bin/mkhex4bin" && "${PROJECT_LOC}/Tools/tiimage" "0x80000000" "NONE" "${PROJECT_LOC}/${ConfigName}/${ProjName}.bin" "${PROJECT_LOC}/${ConfigName}/APP" && cp "${PROJECT_LOC}/${ConfigName}/${ProjName}.bin" "${PROJECT_LOC}/${ConfigName}/APP_XMODEM"

Debugging on the target platform
--------------------------------

- When the "Launching Debug Session" window is shown:
	- Select *only* the "CortxA8" device
	- Click "OK"
