/*

 */

//#define SOFTWARE_REDIRECTION    //GPS redirection to debug port by software

#include "nucleoPIC.h"
#ifndef SOFTWARE_REDIRECTION
#include <stdio.h>
#include "hpm.h"
#include "SerialDevice.h"
#include "pylatex.h"
extern serial_t modoSerial;     //Elemento Serial que se esta controlando
extern void cambiaSerial (serial_t serial);    //Para cambiar el elemento a controlar
extern void enviaDebug(char *arreglo,unsigned char largo);
extern void enviaHPM(char *arreglo,unsigned char largo);
#endif

void main (void) {
    setup();

    #ifdef SOFTWARE_REDIRECTION
    /*
    LATC = 0xC0;
    TRISC = 0x7F;   //Salida solo C7
    while(true){
        RC7=RB5;
    }
    // */
    //*
    LATC = 0x40;
    TRISC = 0xBF;   //Salida solo en C6
    while(true){
        RC6=RB5;
    }
    #else
    cambiaSerial(DEBUG1);
    enableInterrupts();

    InicializacionHPM(enviaHPM);

    /*while (1) {
        /*
        enviaGPS("estoy vivo\r\n",0);
        __delay_ms(1000);
        // 

        processPending();
        if (NCupdated()) {
            uint8_t buff[50];
            sprintf(buff, "Lat: %li, Lon: %li, Height: %li\r\n",
                    (long int)NMEA.latitude, (long int)NMEA.longitude, (long int)NMEA.height );
            enviaDebug(buff,0);
            //cambiaSerial(DEBUG1);
        }

    }*/
    #endif

}

#ifndef SOFTWARE_REDIRECTION
void __interrupt ISR (void) {
    uint8_t rx_err;  //Error de recepcion en UART
    if (RCIE && RCIF) {
        //Error reading
        rx_err=RCSTA;
        //As RCREG is argument, their reading implies the RCIF erasing
        HPMinput(RCREG);
    }
    else {
        //Unhandled Interrupt
    }
}
#endif
