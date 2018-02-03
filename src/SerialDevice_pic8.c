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
#include "SerialDevice.h"

#define SerialSentIsOpen() SPEN && TXEN

//------------------------------------------------------------------------------
//  Section RAM and Function Prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

// open serial device
bool
SerialDevice_Open(const char   *comPort,
                  UINT32        baudRate,   //115200, siempre.
                  int           dataBits,   //8, siempre.
                  UINT8         parity)     //Ninguna, siempre.
{
    //UART, Ajustes comunes a Rx y Tx. Inicializado de acuerdo a datasheet 16F2550
    //Se prueba con 8(interno) y 7.3728(externo) MHz
    ////1: (En reset,SPBRG=0). Usar BRG16=1 y BRGH=1. Velocidades despues de PLL (si lo hay)
    SPBRG=16;   //Fosc=8 MHz (ejm,interno)
    //SPBRG=15;   //Fosc=7.3728 MHz (externo)
    //SPBRG=207;   //Fosc=8 MHz (ejm,interno), 9600 Baud
    #ifdef _18F2550
    SYNC=false; //2. Modo Asincrono
    #endif
    #ifdef _16F1769
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
    for (UINT8 i=0;i<txLength;i++) {
        if (!SerialDevice_SendByte(txBuffer[i])) {
            //Escapes the error
            return false;
        }
    }
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

// read data
int
SerialDevice_ReadData(UINT8 *rxBuffer, int rxBufferSize)
{
    // Todo : add your own platform specific code here
    return -1;
}
//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
