source PATHS.sh
mkdir $PATH_PROJECT/Release
cd $PATH_PROJECT/Release
$PATH_CCS/utils/bin/gmake -k -j 8 all