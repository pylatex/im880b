//------------------------------------------------------------------------------
//
//	File:		SerialDevice.cpp
//	Abstract:	Serial Device Abstraction
//	Version:	0.1
//	Date:		18.05.2016
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
#include <xc.h>
#include <string.h>
#include "SerialDevice.h"

//------------------------------------------------------------------------------
//  Section RAM and Function Prototypes
//------------------------------------------------------------------------------
#define SerialSentIsOpen() (bool)(SPEN && TXEN)
static baudrate_t br;

//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

// open serial device
bool
SerialDevice_Open(const char   *comPort,
                  baudrate_t    baudRate,
                  int           dataBits,
                  UINT8         parity)
{
    //UART, Ajustes comunes a Rx y Tx. Inicializado de acuerdo a datasheet 18F2550
    //Se prueba con 8(interno) y 7.3728(externo) MHz
    ////1: (En reset,SPBRG=0). Usar BRG16=1 y BRGH=1. Velocidades despues de PLL (si lo hay)

    //* Para Fosc=8 MHz (ejm,interno)
    switch (baudRate) {
        case B9600:
            SPBRG=207;
            break;
        case B19200:
            SPBRG=103;
            break;
        case B115200:
            SPBRG=16;
            break;
    } // */

    #ifdef _18F2550
    SYNC=false; //2. Modo Asincrono
    #elif defined _16F1769
    TX1STAbits.SYNC=false; //2. Modo Asincrono
    #endif

    BRG16=true;
    BRGH=true;
    SPEN=true; //2. Habilita Puerto Serie
    //TRANSMISOR
    TXEN=true; //6,Tx. Habilita transmisor
    //RECEPTOR
    CREN=true;  //6,Rx. Habilita receptor
    RCIE=true;  //7,Rx. Interrupcion por recepcion habilitada
    return SerialSentIsOpen();
}

// close serial device
bool
SerialDevice_Close()
{
    SPEN=false;
    TXEN=false;
    CREN=false;
    return true;
}

// send data
int
SerialDevice_SendData(UINT8 *txBuffer, UINT8 txLength)
{
    unsigned char aux=0;
    if (!txLength)
        txLength=strlen(txBuffer);

    while (aux<txLength)
        if (!(SerialDevice_SendByte(*(txBuffer+(aux++)))))
            return false;

    return true;
}

// send single byte
int
SerialDevice_SendByte(UINT8 txByte)
{
    if (SerialSentIsOpen()) {
        while (!TRMT);  //Wait for a pending transmision, due to TSR busy
        TXREG=txByte;
        while (!TXIF);  //Wait while TXREG value are transferred to TSR
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
