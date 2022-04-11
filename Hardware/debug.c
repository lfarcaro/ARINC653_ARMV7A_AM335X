//*****************************************************************************
//
// Function that is called when an invalid argument is passed to an API.
// This is only used when doing a DEBUG build.
//
//*****************************************************************************
void __error__(char *pcFilename, unsigned int ulLine) {
    while(1) {}
}
