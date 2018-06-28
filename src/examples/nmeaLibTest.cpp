#include <cstring>
#include <cstdio>
#include "nmea.h"

#define IT1 0

int main () {
    NMEAuser_t statreg;
    NMEAinit(&statreg);

    const uint8_t fr1[]="$GPGLL,4916.45,N,12311.12,W,225444,A,*1D";
    NMEAload(fr1);
    while (statreg.completeFields < IT1+1);
    printf("%s\n\r",NMEAselect(IT1));
    NMEArelease();

    const uint8_t fr2[]="$GPGSV,3,2,10,02,30,309,,09,20,088,,30,10,157,,13,07,206,*79";
    NMEAload(fr2);
    while (statreg.completeFields < IT1+1);
    printf("%s\n\r",NMEAselect(IT1));
    NMEArelease();

    const uint8_t frase[]="7,,13,07,206,*79bc$!puto,el que lo,,compiled*4c";
    NMEAload(frase);
    while (statreg.completeFields < IT1+1);
    printf("%s\n\r",NMEAselect(IT1));
    NMEArelease();

    return 0;
}

