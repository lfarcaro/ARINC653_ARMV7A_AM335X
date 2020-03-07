call PATHS.bat
mkdir %PATH_PROJECT%\Debug
cd %PATH_PROJECT%\Debug
%PATH_CCS%\utils\bin\gmake.exe -k clean
