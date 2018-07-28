/*
 * File:   TestGPS.c
 * Author: Alex Francisco Bustos Pinzon - GITUD
 *
 * Unidad de Pruebas para el modulo GPS
 *
 * Created on 30 de junio de 2018, 01:04 PM
 */

//#define SOFTWARE_REDIRECTION    //GPS redirection to debug port by software

#include "nucleoPIC.h"
#ifndef SOFTWARE_REDIRECTION
#include <string.h>
#include "nmea.h"
#include "SerialDevice.h"
#endif

void main (void) {
    setup();

    #ifdef SOFTWARE_REDIRECTION
    LATC = 0xC0;
    PORTC = 0xC0;
    TRISC = 0x7F;   //Salida solo C7
    while(true){
        RC7=RB5;
    }
    #else
    NMEAuser_t statreg;
    NMEAnumber lat,lon;
    NMEAinit(&statreg);
    while (1) {
        while (statreg.completeFields < 1);
        if (strcmp("GPGGA",NMEAselect(0)) == 0) {
            while (statreg.completeFields < 6);
            if (*NMEAselect(6) > '0') {
                if (parseCoord2int(&lat,NMEAselect(2),NMEAselect(3))) {
                    //TODO: Division / Shifting according to decimals
                }
                if (parseCoord2int(&lon,NMEAselect(4),NMEAselect(5))) {
                    //TODO: Division / Shifting according to decimals
                }
            }
        } else if (strcmp("GPRMC",NMEAselect(0)) == 0) {
            while (statreg.completeFields < 6);
        }
        NMEArelease();
    }    
    #endif

}

//*
void __interrupt ISR (void) {
    uint8_t rx_err;  //Error de recepcion en UART
    if (RCIE && RCIF) {
        //Error reading
        rx_err=RCSTA;
        //As RCREG is argument, their reading implies the RCIF erasing
        NMEAinput(RCREG);
    }
    else {
        //Unhandled Interrupt
    }
}
// */