/*
 * File:   TestGPS.c
 * Author: Alex Francisco Bustos Pinzon - GITUD
 *
 * Unidad de Pruebas para el modulo GPS
 *
 * Created on 30 de junio de 2018, 01:04 PM
 */

#include "nucleoPIC.h"
//#include "nmea.h"
//#include "SerialDevice.h"

void main (void) {
    setup();
    LATC = 0xC0;
    PORTC = 0xC0;
    TRISC = 0x7F;   //Salida solo C7
    while(true){
        RC7=RB5;
    }

    /*
    RXPPS=0x0D;     //Rx viene de RB5
    RC7PPS=0x16;    //Tx va hacia RC7
    TRISC &= 0x7F;
    setup();
    SerialDevice_Open("",B9600,8,0);
    enableInterrupts();
    while(true) ;
    // */
}

/*
void __interrupt ISR (void) {
    uint8_t rx_err;  //Error de recepcion en UART
    if (RCIE && RCIF) {
        //Error reading
        rx_err=RCSTA;
        //As RCREG is argument, their reading implies the RCIF erasing
        SerialDevice_SendByte(RCREG);
        //NMEAinput(RCREG);
    }
    else {
        //Unhandled Interrupt
    }
}
// */