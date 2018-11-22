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
#include "pylatex.h"
extern serial_t modoSerial;     //Elemento Serial que se esta controlando
extern void cambiaSerial (serial_t serial);    //Para cambiar el elemento a controlar
extern void enviaDebug(char *arreglo,unsigned char largo);
extern void enviaGPS(char *arreglo,unsigned char largo);
#endif

void main (void) {
    SYSTEM_Initialize();

    #ifdef SOFTWARE_REDIRECTION
    while(true){
        //VCC_SENS_LAT = GNS_TX_PORT; //DEBUG
        GNS_RX_LAT = GNS_TX_PORT; //GPS
    }
    #else
    cambiaSerial(DEBUG1);
    //INTERRUPT_GlobalInterruptEnable();
    //INTERRUPT_PeripheralInterruptEnable();

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

/*
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
// */
