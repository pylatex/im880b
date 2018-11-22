#include "nucleoPIC.h"
#include <string.h>
#include <stdio.h>
#include "SerialDevice.h"

void ppsLock (bool lock){
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = lock?1u:0u; // lock PPS
}
