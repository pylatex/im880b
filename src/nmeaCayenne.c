/*
 * File: nmeaCayenne.c
 * Author: Alex Bustos - GITUD
 *
 * Cayenne GPS Location formatter from NMEA core
 */

#include <string.h>
#include "nmeaCayenne.h"
#include "nmea.h"

static bool updated = false;
static NMEAdata_t *GPS;

static bool strnum2int (NMEAnumber *destination,uint8_t *number);
static bool nmeaCoord2cayenneNumber(NMEAnumber *destination,uint8_t *number,uint8_t *direction);
static void fixDecimals(NMEAnumber *number,uint8_t decimals);
static void nmeaHandler (void);

//------------------------------------------------------------------------------
// CORE FUNCTIONS
//------------------------------------------------------------------------------

void initNC(NMEAdata_t *NCobjPtr) {
    GPS=NCobjPtr;
    regNMEAhandler(nmeaHandler);
}

void NCinputSerial(char rxByte) {
    NMEAinput(rxByte);
}

bool NCupdated(void) {
    if (updated) {
        updated = false;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
// AUXILIAR FUNCTIONS
//------------------------------------------------------------------------------

static void nmeaHandler (void) {

    static uint8_t *latnum,*latvec,*lonnum,*lonvec,*hnum=0,*hunit=0;
    bool proc = false;
    bool hproc = false;

    if (strcmp("GPGGA",NMEAselect(0)) == 0) {
        if (*NMEAselect(6) > '0') {
            latnum = NMEAselect(2);
            latvec = NMEAselect(3);
            lonnum = NMEAselect(4);
            lonvec = NMEAselect(5);
            hnum = NMEAselect(9);
            hunit = NMEAselect(10);
            proc = true;
            hproc = true;
        }
    } else if (strcmp("GPRMC",NMEAselect(0)) == 0) {
        if (*NMEAselect(2) == 'A') {
            latnum = NMEAselect(3);
            latvec = NMEAselect(4);
            lonnum = NMEAselect(5);
            lonvec = NMEAselect(6);
            proc = true;
        }
    }

    if (proc) {
        NMEAnumber lat;
        NMEAnumber lon;
        if (nmeaCoord2cayenneNumber(&lat,latnum,latvec)
            && nmeaCoord2cayenneNumber(&lon,lonnum,lonvec)) {
            fixDecimals(&lat,4);
            fixDecimals(&lon,4);
            GPS->latitude = lat.mag;
            GPS->longitude = lon.mag;
        }
        updated = true;
    }

    if (hproc && *hunit == 'M') {
        NMEAnumber height;
        if (strnum2int(&height,hnum)) {
            fixDecimals(&height,2);
            GPS->height = height.mag;
        }
    }
}

static bool strnum2int (NMEAnumber *destination,uint8_t *number){
    if (!destination || !number)
        return false;

    destination->mag = 0;
    destination->decimals = 0;
    uint8_t aux;
    bool punto = false;
    bool menos = false;

    while (*number) {
        if (*number == '.') {
            if (punto) return false;
            punto = true;
        } else if (*number == '-'){
            if (menos) return false;
            menos = true;
        } else if ((aux = *number - '0') < 10) {
            destination->mag *= 10;
            destination->mag += aux;
            if (punto) destination->decimals++;
        } else return false;
        number++;
    }

    if (menos) destination->mag *= -1;
    return true;
}

static void fixDecimals(NMEAnumber *number,uint8_t decimals){
    if (number->decimals < decimals) {
        while (number->decimals < decimals) {
            number->mag *= 10;
            number->decimals++;
        }
    } else if (number->decimals > decimals) {
        while (number->decimals > decimals) {
            number->mag /= 10;
            number->decimals--;
        }
    }
}

static bool nmeaCoord2cayenneNumber(NMEAnumber *destination,uint8_t *number,uint8_t *direction){
    if (strnum2int(destination,number)) {
        int32_t base=1;
        uint8_t aux=destination->decimals + 2u;
        for (;aux;aux--)
            base *= 10;
        //base is being recycled
        base = destination->mag % base;
        destination->mag -= base;
        destination->mag /= 10;
        base /= 6;
        destination->mag += base;
        destination->mag /= 10;
        fixDecimals(destination,4);

        switch (*direction) {
            case 'S':
            case 'W':
                destination->mag *= -1;
            case 'N':
            case 'E':
                return true;

            default:
                return false;
        }
    }
    return false;
}
