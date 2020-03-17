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
extern void ppsLock (bool state);      //Especifica el estado de bloqueo de PPS
static baudrate_t br;
serial_t modoSerial;     //Elemento Serial que se esta controlando

//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

// open serial device
bool
SerialDevice_Open(const char   *comPort,
                  baudrate_t    baudRate,
                  int           dataBits,
                  uint8_t         parity)
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
SerialDevice_SendData(uint8_t *txBuffer, uint8_t txLength)
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
SerialDevice_SendByte(uint8_t txByte)
{
    if (SerialSentIsOpen()) {
        while (!TRMT);  //Wait for a pending transmision, due to TSR busy
        TXREG=txByte;
        while (!TXIF);  //Wait while TXREG value are transferred to TSR
        return true;
    }
    return false;
}

/**
 * Cambia la asignacion interna del EUSART
 * @param serial: El elemento con que se desea conectar (vease serial_t arriba)
 */
void cambiaSerial (serial_t serial){
    baudrate_t nuevaTasa;
    SerialDevice_Close();

    #ifdef _16F1769
    ppsLock(false);
    //Desligar las salidas del serial
    switch (modoSerial) {
        case MODEM_LW:
            RC4PPS=0;    //RC4 recibe el Latch de su puerto
            break;

        case GPS:
            //Entradas y salidas UART
            RC6PPS=0;    //RC6 recibe el Latch de su puerto
            break;

        case DEBUG1:
            RC7PPS=0;
            break;

        default:
            break;
    }
    //Ligar las nuevas salidas y entradas al modulo UART
    switch (modoSerial=serial) {
        case MODEM_LW:
            //Entradas y salidas UART
            RXPPS=0x15;     //Rx viene de RC5
            RC4PPS=0x16;    //Tx va hacia RC4
            nuevaTasa = B115200;
            break;

        case GPS:
            //Entradas y salidas UART
            RXPPS=0x0D;     //Rx viene de RB5
            RC6PPS=0x16;    //Tx va hacia RC6
            nuevaTasa = B9600;
            break;

        case DEBUG1:
            //Entradas y salidas UART
            RXPPS=0x0F;     //Rx viene de RB7
            RC7PPS=0x16;    //Tx va hacia RC7
            nuevaTasa = B9600;
            break;

        default:
            break;
    }
    ppsLock(true);
    #endif

    SerialDevice_Open("",nuevaTasa,8,0);
}

void compruebaModo (serial_t modo) {
    if (modoSerial != modo)
        cambiaSerial(modo);
}

void enviaIMST(uint8_t *arreglo,uint8_t largo) {
    compruebaModo(MODEM_LW);
    SerialDevice_SendData(arreglo,largo);
}

void enviaGPS(uint8_t *arreglo,uint8_t largo) {
    compruebaModo(GPS);
    SerialDevice_SendData(arreglo,largo);
}

void enviaDebug(uint8_t *arreglo,uint8_t largo) {
    compruebaModo(DEBUG1);
    SerialDevice_SendData(arreglo,largo);
}
