/*
 * File: nmeaCayenne.c
 * Author: Alex Bustos - GITUD
 *
 * Cayenne GPS Location formatter from NMEA core
 */

#include <string.h>
#include "nmeaCayenne.h"
#include "nmea.h"

static bool pendingWork = false;
static bool updated = false;
static NMEAdata_t *GPS;

static bool strnum2int (NMEAnumber *destination,uint8_t *number);
static bool nmeaCoord2cayenneNumber(NMEAnumber *destination,uint8_t *number,uint8_t *direction);
static void fixDecimals(NMEAnumber *number,uint8_t decimals);

//------------------------------------------------------------------------------
// CORE FUNCTIONS
//------------------------------------------------------------------------------

void initNC(NMEAdata_t *NCobjPtr) {
    GPS=NCobjPtr;
    NMEAinit ();
}

void processPending(void) {
    uint8_t *latnum,*latvec,*lonnum,*lonvec,*hnum=0,*hunit=0;
    bool process = false;
    static enum {
        IDLE,
        waitingGGA,
        waitingRMC,
        DONE
    } stat = IDLE;

    switch (stat) {

        case IDLE:
            if (NMEAgetCompletedFields() >= 1 ) {
                if (strcmp("GPGGA",NMEAselect(0)) == 0) {
                    stat = waitingGGA;
                } else if (strcmp("GPRMC",NMEAselect(0)) == 0) {
                    stat = waitingRMC;
                } else {
                    NMEArelease();
                    stat = IDLE;
                }
            }
            break;

        case waitingGGA:
            if (NMEAgetCompletedFields() >= 10) {
                if (*NMEAselect(6) > '0') {
                    latnum = NMEAselect(2);
                    latvec = NMEAselect(3);
                    lonnum = NMEAselect(4);
                    lonvec = NMEAselect(5);
                    hnum = NMEAselect(9);
                    hunit = NMEAselect(10);
                    process = true;
                }
            }
            break;

        case waitingRMC:
            if (NMEAgetCompletedFields() >= 6 ) {
                if (*NMEAselect(6) > '0') {
                    latnum = NMEAselect(3);
                    latvec = NMEAselect(4);
                    lonnum = NMEAselect(5);
                    lonvec = NMEAselect(6);
                    if (hnum && hunit) process = true;
                }
            }
            break;

        default:
            break;
    }

    if (process) {
        NMEAnumber lat;
        NMEAnumber lon;
        NMEAnumber height;
        if (nmeaCoord2cayenneNumber(&lat,latnum,latvec)
            && nmeaCoord2cayenneNumber(&lon,lonnum,lonvec)
            && strnum2int(&height,hnum) ){
            fixDecimals(&lat,4);
            fixDecimals(&lon,4);
            fixDecimals(&height,2);
            GPS->latitude = lat.mag;
            GPS->longitude = lon.mag;
            GPS->height = height.mag;
            updated = true;
        }
        NMEArelease ();
        stat = IDLE;
    }
}

bool NCupdated(void) {
    if (updated) {
        updated = false;
        return true;
    }
    return false;
}

void NCinputSerial(char rxByte) {
    NMEAinput(rxByte);
}

//------------------------------------------------------------------------------
// AUXILIAR FUNCTIONS
//------------------------------------------------------------------------------

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
        destination->decimals ++;
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
