call PATHS.bat
mkdir %PATH_PROJECT%\Release
cd %PATH_PROJECT%\Release
%PATH_CCS%\utils\bin\gmake.exe -k clean