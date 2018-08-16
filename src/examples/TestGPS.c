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
extern serial_t modoSerial;     //Elemento Serial que se esta controlando
extern void cambiaSerial (serial_t serial);    //Para cambiar el elemento a controlar
void enviaDebug(char *arreglo,unsigned char largo);
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
    cambiaSerial(DEBUG1);
    enableInterrupts();

    NMEAuser_t statreg;
    uint8_t *latnum,*latvec,*lonnum,*lonvec,*hnum=0,*hunit=0;
    NMEAinit(&statreg);

    while (1) {
        //enviaDebug("estoy vivo\r\n",0);
        //__delay_ms(1000);
        //*
        while (statreg.completeFields < 1);

        bool update = false;
        if (strcmp("GPGGA",NMEAselect(0)) == 0) {
            while (statreg.completeFields < 10);
            if (*NMEAselect(6) > '0') {
                latnum = NMEAselect(2);
                latvec = NMEAselect(3);
                lonnum = NMEAselect(4);
                lonvec = NMEAselect(5);
                hnum = NMEAselect(9);
                hunit = NMEAselect(10);
                update = true;
            }
        } else if (strcmp("GPRMC",NMEAselect(0)) == 0) {
            while (statreg.completeFields < 6);
            if (*NMEAselect(6) > '0') {
                latnum = NMEAselect(3);
                latvec = NMEAselect(4);
                lonnum = NMEAselect(5);
                lonvec = NMEAselect(6);
                if (hnum && hunit) update = true;
            }
        }

        NMEAnumber lat,lon,height;
        if (update
        &&  parseCoord2int(&lat,latnum,latvec)
        &&  parseCoord2int(&lon,lonnum,lonvec)
        &&  parseCoord2int(&height,hnum,hunit) ) {
            //For Cayenne LPP, 0.0001 deg/bit, Signed MSB
            fixDecimals(&lat,4);
            //For Cayenne LPP, 0.0001 deg/bit, Signed MSB
            fixDecimals(&lon,4);
            //For Cayenne LPP, 0.01 m/bit, Signed MSB
            fixDecimals(&height,2);
        } else {
            enviaDebug("NOPE\r\n",0);
        }

        NMEArelease();
        // */
    }
    #endif

}

#ifndef SOFTWARE_REDIRECTION
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
#endif
