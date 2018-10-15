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
#include <stdio.h>
#include "nmeaCayenne.h"
#include "SerialDevice.h"
#include "pylatex.h"
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
    cambiaSerial(GPS);
    enableInterrupts();

    NMEAdata_t NMEA;
    initNC(&NMEA);

    while (1) {
        //enviaDebug("estoy vivo\r\n",0);
        //__delay_ms(1000);
        //*
        //processPending();
        if (NCupdated()) {
            uint8_t buff[50];
            sprintf(buff, "Lat: %li, Lon: %li, Height: %li\r\n",
                    (long int)NMEA.latitude, (long int)NMEA.longitude, (long int)NMEA.height );
            enviaDebug(buff,0);
            cambiaSerial(GPS);
        }

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
        NCinputSerial(RCREG);
    }
    else {
        //Unhandled Interrupt
    }
}
#endif
