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
#include "WiMOD_LoRaWAN_API.h"
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
//  Declarations, Function Prototypes and Variables
//------------------------------------------------------------------------------

void ms100 (unsigned char q);    //A (100*q) ms delay
void ProcesaHCI();   //Procesamiento de HCI entrante

volatile unsigned char rx_err; //Relacionados con el receptor
volatile unsigned char buffer[20]; //Buffer de salida
volatile static HCIMessage_t    TxMessage;
volatile static HCIMessage_t    RxMessage;
volatile unsigned bool prender;

//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

//Demora con Timer 1 y CCP 1
void StartTimerDelayMs(unsigned char cant)
{
    //CCPR1=cant*250;
    CCPR1=(unsigned int)((cant<<8)-(cant<<2)-(cant<<1));
    CCP1IF=false;   //Restablecer comparador
    CCP1CON=0x0B;   //Modulo CCP en Comparacion a (representado) cant/ms
    T1CON=0x30;     //Prescale 1:8
    TMR1ON=true;
}

void blink (unsigned char cant) {
    while (cant--) {
        LED=true;
        ms100(1);
        LED=false;
        ms100(1);
    }
}

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
    TRISA=0xFE; //RA0 as output
    ms100(1);   //Delay for stabilization of power supply
    
    InitHCI(ProcesaHCI,&RxMessage);  //Full Duplex UART and Rx interruptions enabled
    //WiMOD_LoRaWAN_Init("");
    PEIE=true;  //Peripheral Interrupts Enable
    GIE=true;   //Global Interrupts Enable
    prender=false;

    //LW STATUS AND CONNECTION
    
    //1. Wait for connection between im880 and MCU
    do {
        WiMOD_LoRaWAN_SendPing();
        __delay_ms(20); //small delay to allow the processing of the HCI message
    } while (!prender);
    prender=false;

    while (true) blink(10); //Uncomment to see if comm OK between MCU and WiMOD LW module
    
    //2. Check/Wait for LoRaWAN connection
    
    //  1. Desactivar el dispositivo
    //  2. Solicitar conexion (join req)
    
    //MAIN LOOP
    while (true) {

        //------------------------------------------
        // Serial Device Test
        
        //--Through the function
        //Don't forget to include SerialDevice.h
        //SerialDevice_SendData("Hello",5);

        /*//--Each byte by appart
        //Don't forget to include SerialDevice.h
        SerialDevice_SendByte('H');
        SerialDevice_SendByte('e');
        SerialDevice_SendByte('l');
        SerialDevice_SendByte('l');
        SerialDevice_SendByte('o');
        //*/

        //------------------------------------------
        // HCI Send Test

        /*// with the function
        buffer[0]=DEVMGMT_SAP_ID;
        buffer[1]=DEVMGMT_MSG_PING_REQ;
        SendHCI(buffer,0);
        //*/
        
        //with the API
        WiMOD_LoRaWAN_SendPing();

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
        
        LED=true;
        ms100(1);
        LED=false;
        
        if (prender) {
            prender=false;
            ms100(5);
        } else ms100(20);
        //*/
    }
}

void __interrupt ISR (void) {
    if (RCIE && RCIF) {
        //Error reading
        rx_err=RCSTA;
        //As RCREG is argument, their reading implies the RCIF erasing
        IncomingHCIpacker(RCREG);
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

//Handler for (pre)processing of an incoming HCI message
void ProcesaHCI() {
    if (RxMessage.SapID==DEVMGMT_SAP_ID && RxMessage.MsgID==DEVMGMT_MSG_PING_RSP && RxMessage.check)
    {
        prender=true;
    }
}