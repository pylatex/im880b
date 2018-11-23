/*
 * File:   TestGPS.c
 * Author: Alex Francisco Bustos Pinzon - GITUD
 *
 * Unidad de Pruebas para el modulo GPS
 *
 * Created on 30 de junio de 2018, 01:04 PM
 */

//#define SOFTWARE_REDIRECTION    //GPS redirection to debug port by software

#include "mcc.h"
#ifndef SOFTWARE_REDIRECTION
#include <stdio.h>
#include "nmeaCayenne.h"
#include "SerialDevice.h"
#include "serialSwitch.h"
#include "pylatex.h"
#endif

void procesaRx (void);

void main (void) {
    SYSTEM_Initialize();

    #ifdef SOFTWARE_REDIRECTION
    while(true){
        //VCC_SENS_LAT = GNS_TX_PORT; //DEBUG
        GNS_RX_LAT = GNS_TX_PORT; //GPS
    }
    #else
    EUSART_SetRxInterruptHandler(procesaRx);
    cambiaSerial(GPS);
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    NMEAdata_t NMEA;
    initNC(&NMEA);

    while (1) {
        /*
        enviaGPS("estoy vivo\r\n",0);
        __delay_ms(1000);
        // */
        //*
        if (NCupdated()) {
            uint8_t buff[50];
            sprintf(buff, "Lat: %li, Lon: %li, Height: %li\r\n",
                    (long int)NMEA.latitude, (long int)NMEA.longitude, (long int)NMEA.height );
            enviaDebug(buff,0);
            //cambiaSerial(DEBUG1);
        }

        // */
    }
    #endif

}

#ifndef SOFTWARE_REDIRECTION
void procesaRx (void) {
    NCinputSerial(RC1REG);
}
#endif
