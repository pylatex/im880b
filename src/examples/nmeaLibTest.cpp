#include <cstring>
#include <cstdio>
#include "nmeaCayenne.h"

#define IT1 0

static NMEAdata_t NMEA;

static void tryprint();
static void load(char *phr);

int main () {

    initNC(&NMEA);

    load("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47");
    if (NCupdated())
        tryprint();

    load("$GPRMC,023042,A,3907.3837,N,12102.4684,W,0.0,156.1,131102,15.3,E,A*36");
    if (NCupdated())
        tryprint();

    load("7,,13,07,206,*79bc$!puto,el que lo,,compiled*4c");
    if (NCupdated())
        tryprint();

    load("$GPGGA,011832.000,0442.8941,N,07407.4114,W,1,8,0.97,2575.5,M,3.2,M,,*42");
    if (NCupdated())
        tryprint();

    load("$GPRMC,011832.000,A,0442.8941,N,07407.4114,W,0.77,224.41,101018,,,A*7F");
    if (NCupdated())
        tryprint();

    return 0;
}

static void tryprint(){
    printf("Latitud: %f, Longitud: %f, Altitud:%f\n\r",NMEA.latitude/1000.0,NMEA.longitude/1000.0,NMEA.height/100.0);
}

static void load(char *phr) {
    while (*phr) {
        NCinputSerial(*phr);
        phr++;
    }
}
