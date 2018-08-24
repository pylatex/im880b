#include <cstring>
#include <cstdio>
#include "nmea.h"

#define IT1 0

static void tryprint(uint8_t *latnum,uint8_t *latvec,uint8_t *lonnum,uint8_t *lonvec,uint8_t *hnum,uint8_t *hunit);

int main () {
    NMEAuser_t statreg;
    bool update = false;
    NMEAinit(&statreg);

    const uint8_t fr1[]="$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    NMEAload(fr1);
    if (*NMEAselect(6) > '0')
        tryprint(NMEAselect(2),NMEAselect(3),NMEAselect(4),NMEAselect(5),NMEAselect(9),NMEAselect(10));
    NMEArelease();

    const uint8_t fr2[]="$GPRMC,023042,A,3907.3837,N,12102.4684,W,0.0,156.1,131102,15.3,E,A*36";
    NMEAload(fr2);
    if (*NMEAselect(6) > '0')
        tryprint(NMEAselect(3),NMEAselect(4),NMEAselect(5),NMEAselect(6),(uint8_t *)"2620.25",(uint8_t *)"M");
    NMEArelease();

    const uint8_t frase[]="7,,13,07,206,*79bc$!puto,el que lo,,compiled*4c";
    NMEAload(frase);
    while (statreg.completeFields < IT1+1);
    printf("%s\n\r",NMEAselect(IT1));
    NMEArelease();

    return 0;
}

static void tryprint(uint8_t *latnum,uint8_t *latvec,uint8_t *lonnum,uint8_t *lonvec,uint8_t *hnum,uint8_t *hunit){
    NMEAnumber lat,lon,height;
    if (nmeaCoord2cayenneNumber(&lat,latnum,latvec)
    &&  nmeaCoord2cayenneNumber(&lon,lonnum,lonvec)
    &&  strnum2int(&height,hnum) ) {
        //For Cayenne LPP, 0.01 m/bit, Signed MSB
        fixDecimals(&height,2);
        printf("Latitud: %f, Longitud: %f, Altitud:%f\n\r",lat.mag/1000.0,lon.mag/1000.0,height.mag/100.0);
    } else {
        printf("NOPE\r\n");
    }
}
