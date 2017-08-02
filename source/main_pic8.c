/*
 * Archivo de ejemplo, para implementacion en microcontroladores.
 * Se pretende que se requieran interrupciones para el funcionamiento.
 * 
 * La mayoria de archivos vienen del comprimido
 * WiMOD_LoRaWAN_ExampleCode_HCI_C_V0_1.zip
 * y han sido ligeramente modificados para soportar tipos estandar.
 */

//------------------------------------------------------------------------------
//  Definitions and Setup
//------------------------------------------------------------------------------
//#include <string.h>
#include "hci_stack.h"
//#include "SerialDevice.h"
#include <xc.h>
#define _XTAL_FREQ 8000000  //May be either Internal RC or external oscillator.
//#define _XTAL_FREQ 7372800  //External Quartz Crystal to derivate common UART speeds

//* Remove a '/' to comment the setup for PIC18F2550 wiht INTOSC@8MHz
#pragma config PLLDIV = 1, CPUDIV = OSC1_PLL2, USBDIV = 1
#pragma config FOSC = INTOSCIO_EC,  FCMEN = ON, IESO = OFF
#pragma config PWRT = ON, BOR = OFF, BORV = 3, VREGEN = OFF
#pragma config WDT = OFF, WDTPS = 32768
#pragma config CCP2MX = ON, PBADEN = OFF, LPT1OSC = ON, MCLRE =	ON
#pragma config STVREN = OFF, LVP = OFF, DEBUG=OFF, XINST = OFF
//*/

#define LED LATA0 //Para las pruebas de parpadeo y ping
//#define PIN RC0 //Para prueba LED=PIN

//------------------------------------------------------------------------------
//  Declarations, Definitions and Variables
//------------------------------------------------------------------------------

void ms100 (unsigned char q);    //A (100*q) ms delay

volatile unsigned char rx_err; //Relacionados con el receptor
volatile unsigned char buffer[20]; //Buffer de salida

//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

/**
 * Main
 */
void main(void) 
{
    //INITIALIZATION
    OSCCON=0x73;    //Internal at 8 MHz (applies to: 18F2550)
    while(!IOFS);   //Waits for stabilization
    
    //LED in RC0, based on datasheet sugestion.
    ADCON1=0x0F;    //All pins as digital (applies to: 18F2550)
    PORTA=0;
    LATA=0;
    TRISA=0xFE; //RC0 as output
    
    InitHCI();  //Full Duplex UART and Rx interruptions enabled
    PEIE=true;  //Peripheral Interrupts Enable
    GIE=true;   //Global Interrupts Enable
    
    //MAIN LOOP
    while (true) {

        //------------------------------------------
        // Serial Device Test
        
        //Using the function
        //Don't forget to include SerialDevice.h
        //SerialDevice_SendData("Hello",5);

        /*//Sending by appart
        //Don't forget to include SerialDevice.h
        SerialDevice_SendByte('H');
        SerialDevice_SendByte('e');
        SerialDevice_SendByte('l');
        SerialDevice_SendByte('l');
        SerialDevice_SendByte('o');
        //*/

        //------------------------------------------
        // HCI Send Test

        //*// with the function
        buffer[0]=DEVMGMT_SAP_ID;
        buffer[1]=DEVMGMT_MSG_PING_REQ;
        SendHCI(buffer,0);
        //*/

        /*// Emulate the function
        //Don't forget to include SerialDevice.h
        SerialDevice_SendByte(SLIP_END);
        SerialDevice_SendByte(DEVMGMT_SAP_ID);
        SerialDevice_SendByte(DEVMGMT_MSG_PING_REQ);
        SerialDevice_SendByte(0xA0);    //CRC
        SerialDevice_SendByte(0xAF);    //CRC
        SerialDevice_SendByte(SLIP_END);
        //*/
        
        //------------------------------------------
        // HCI Message Reception and Validation Test
        
        /*//Comment this line to uncomment:
        //Emulation of the UART succesive reception function
        ProcessHCI(buffer1,SLIP_END);
        ProcessHCI(buffer1,SLIP_END);
        ProcessHCI(buffer1,SLIP_END);
        ProcessHCI(buffer1,DEVMGMT_SAP_ID);
        ProcessHCI(buffer1,DEVMGMT_MSG_PING_RSP);
        ProcessHCI(buffer1,DEVMGMT_STATUS_OK);
        ProcessHCI(buffer1,0xA0);   //CRC
        ProcessHCI(buffer1,0xAF);   //CRC
        if (ProcessHCI(buffer1,SLIP_END)>=0) {
            ping=true;  //This asignation should be done on interrupt time.
        }
        //*/
        
        __delay_ms(10); //small delay to allow the processing of the HCI message
        //The LED should blink on every complete HCI message.
        if (BuffSizeHCI()>=0) {
            //A successfully decoded HCI message are ready to be read
            if (buffer[0]==DEVMGMT_SAP_ID && buffer[1]==DEVMGMT_MSG_PING_RSP) {
                //The response corresponds to the request sent.
                LED=true;
                ms100(1);
                LED=false;
                ms100(5);
            }
            ClearRxHCI();   //Call when the HCI message can be overwrited
        } else ms100(10);
        //*/
    }
}

void __interrupt ISR (void) {
    if (RCIE && RCIF) {
        //Error reading
        rx_err=RCSTA;
        //As RCREG is argument, their reading implies the RCIF erasing
        ProcessHCI(buffer,RCREG);
    }
}

/**
 * Generates a delay of q*100 ms
 * @param q: multiplier.
 */
void ms100 (unsigned char q) {
    while (q--)
        __delay_ms(100);    //XC8 compiler
}
