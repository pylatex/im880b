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
#include <string.h>
#include <xc.h>
#include "WMLW_APIconsts.h"
#include "hci_stack.h"
#include "SerialDevice.h"
#include "i2c.h"

#define _XTAL_FREQ 8000000  //May be either Internal RC or external oscillator.
//#define _XTAL_FREQ 7372800  //External Quartz Crystal to derivate common UART speeds

//* Remove a '/' to comment the setup for PIC18F2550 with INTOSC@8MHz
#pragma config PLLDIV = 1, CPUDIV = OSC1_PLL2, USBDIV = 1
#pragma config FOSC = INTOSCIO_EC,  FCMEN = ON, IESO = OFF
#pragma config PWRT = ON, BOR = OFF, BORV = 3, VREGEN = OFF
#pragma config WDT = OFF, WDTPS = 32768
#pragma config CCP2MX = ON, PBADEN = OFF, LPT1OSC = ON, MCLRE =	ON
#pragma config STVREN = OFF, LVP = OFF, DEBUG=OFF, XINST = OFF
//*/

#ifdef SERIAL_DEVICE_H
#define EUSART_Write(x) SerialDevice_SendByte(x)
#endif
#define MEM_ADDR        0x50    //Testing with I2C Memory 24AA00
#define MEM_MAX_TRIES   30

#define LED LATA0 //Para las pruebas de parpadeo y ping
//#define PIN RC0 //Para prueba LED=PIN

//------------------------------------------------------------------------------
//  Declarations, Function Prototypes and Variables
//------------------------------------------------------------------------------

void ms100(unsigned char q); //A (100*q) ms delay
void ProcesaHCI(); //Procesamiento de HCI entrante
void enviaMsgSerie(const unsigned char *arreglo,unsigned char largo);

volatile unsigned char rx_err; //Relacionados con el receptor
volatile unsigned char buffer[20]; //Buffer de salida
volatile static HCIMessage_t TxMessage;
volatile static HCIMessage_t RxMessage;
volatile unsigned bool pendingmsg;
volatile unsigned int lengthrx;
volatile unsigned char timeouts;

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
    unsigned char buffsal[20],tries;
    I2C_MESSAGE_STATUS estadoi2c;
    
    enum {
        RESET, TestUART, GetNwkStatus, NWKinactive, NWKjoining, NWKactive, 
        NWKaccept
    } status;

    //INITIALIZATION
    OSCCON=0x73;    //Internal at 8 MHz (applies to: 18F2550)
    while(!IOFS);   //Waits for stabilization
    ms100(1);   //Delay for stabilization of power supply

    //LED in RC0, initialization based on datasheet sugestion.
    ADCON1=0x0F;    //All pins as digital (applies to: 18F2550)
    PORTA=0;
    LATA=0;
    TRISA=0xFE; //RA0 as output

    pendingmsg = false;
    status = RESET; //Initial State for the machine of Main.
    timeouts=0;
    
    I2C_Initialize();
    SerialDevice_Open("",8,0);

    CCP1IE = true; //Comparison, for timeouts.
    PEIE = true; //Peripheral Interrupts Enable
    GIE = true; //Global Interrupts Enable

    /*
    //STATE MACHINE
    while (true) {
        switch (status) {
            case RESET:
                if (InitHCI(ProcesaHCI, &RxMessage))
                    status = TestUART; //Full Duplex UART and Rx interruptions enabled
                break;
            case TestUART:
                //Message Setup
                TxMessage.SapID = DEVMGMT_ID;
                TxMessage.MsgID = DEVMGMT_MSG_PING_REQ;
                TxMessage.size = 0;
                //Send Message
                SendHCI(&TxMessage);
                //Wait for TimeOut or HCI message and identify the situation:
                StartTimerDelayMs(20);
                while (TMR1ON && !pendingmsg); //Escapes at timeout or HCI received.
                if (pendingmsg) {
                    timeouts=0;
                    if ((RxMessage.SapID == DEVMGMT_ID) && (RxMessage.MsgID == DEVMGMT_MSG_PING_RSP)) {
                        //The incoming HCI message is a response of PING
                        status = GetNwkStatus;
                        pendingmsg = false;
                    }
                }
                break;
            case GetNwkStatus:
                //Message Setup
                TxMessage.SapID = LORAWAN_ID;
                TxMessage.MsgID = LORAWAN_MSG_GET_NWK_STATUS_REQ;
                TxMessage.size = 0;
                //Send Message
                SendHCI(&TxMessage);
                //Wait for TimeOut or HCI message and identify the situation:
                StartTimerDelayMs(20);
                while (TMR1ON && !pendingmsg); //Escapes at timeout or HCI received.
                if (pendingmsg) {
                    timeouts=0;
                    if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_GET_NWK_STATUS_RSP)) {
                        pendingmsg = false;
                        //The incoming HCI message is a response of NWK STATUS
                        switch (RxMessage.Payload[1]) {
                            case 0: //Inactive
                            case 1: //Active (ABP)
                                status = NWKinactive;
                                break;
                            case 2: //Active (OTAA)
                                status = NWKactive;
                                break;
                            case 3: //Accediendo (OTAA)
                                status = NWKjoining;
                                break;
                            default:
                                break;
                        }
                        
                    }
                }
                break;
            case NWKinactive:
                //Message Setup
                TxMessage.SapID = LORAWAN_ID;
                TxMessage.MsgID = LORAWAN_MSG_JOIN_NETWORK_REQ;
                TxMessage.size = 0;
                //Send Message
                SendHCI(&TxMessage);
                //Wait for TimeOut or HCI message and identify the situation:
                StartTimerDelayMs(20);
                while (TMR1ON && !pendingmsg); //Escapes at timeout or HCI received.
                if (pendingmsg) {
                    timeouts=0;
                    if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_JOIN_NETWORK_RSP)) {
                        //The incoming HCI message is a response of NWK STATUS
                        if (RxMessage.Payload[0] == LORAWAN_STATUS_OK) {
                            pendingmsg = false;
                            status = NWKjoining;
                        }
                    }
                }
                break;
            case NWKjoining:
                //Wait for TimeOut or HCI message and identify the situation:
                StartTimerDelayMs(20);
                while (TMR1ON && !pendingmsg); //Escapes at timeout or HCI received.
                if (pendingmsg) {
                    timeouts=0;
                    if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_JOIN_NETWORK_IND)) {
                        //The incoming HCI message is a join event
                        switch (RxMessage.Payload[0]) {
                            case 0x00:  //device successfully activated
                            case 0x01:  //device successfully activated, Rx Channel Info attached
                                status = NWKaccept;
                            default:
                                break;
                        }
                    }
                }
                break;
            case NWKaccept:
                //Wait for TimeOut or HCI message and identify the situation:
                StartTimerDelayMs(20);
                while (TMR1ON && !pendingmsg); //Escapes at timeout or HCI received.
                if (pendingmsg) {
                    timeouts=0;
                    if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_RECV_NO_DATA_IND)) {
                        //The incoming HCI message is an indication of reception with no data
                        if (RxMessage.Payload[0] == LORAWAN_STATUS_OK) {
                            pendingmsg = false;
                            status = NWKactive;
                        }
                    }
                }
                break;
            case NWKactive:
                //Create more states to attend other HCI messages or modify this state
                LED=true;
                while (true) {
                    SLEEP();
                }
                break;
            default:
                break;
        }
    }

    //*/ //State Machine Description
    
    buffsal[0]=0x00;    //Registro
    while (true)
    {
        //*
        buffsal[1]=0xCA;    //Valor a escribir
        estadoi2c=I2C_MESSAGE_PENDING;
        tries=0;
        while(estadoi2c != I2C_MESSAGE_FAIL)
        {
            // write one byte to EEPROM (3 is the number of bytes to write)
            I2C_MasterWrite(buffsal,2,MEM_ADDR,&estadoi2c);

            // wait for the message to be sent or status has changed.
            while(estadoi2c == I2C_MESSAGE_PENDING);

            if (estadoi2c == I2C_MESSAGE_COMPLETE)
                break;

            // if status is  I2C_MESSAGE_ADDRESS_NO_ACK,
            //               or I2C_DATA_NO_ACK,
            // The device may be busy and needs more time for the last
            // write so we can retry writing the data, this is why we
            // use a while loop here

            // check for max retry and skip this byte
            if (tries == MEM_MAX_TRIES)
                break;
            else
                tries++;
        }

        if (tries == MEM_MAX_TRIES) {
            enviaMsgSerie("Se excedieron los intentos\n\r",0);
        } else
        switch (estadoi2c) {
            case I2C_MESSAGE_COMPLETE:
                enviaMsgSerie("Mensaje I2C Completo\n\r",0);
                break;
            case I2C_MESSAGE_FAIL:
                break;
        }
        if (estadoi2c == I2C_MESSAGE_FAIL)
        {
            enviaMsgSerie("Fallo al enviar I2C\n\r",0);
        }
        //*/

        /*
        for (unsigned char aux=0;aux<12;aux++)
            EUSART_Write(frase[aux]);
        //*/
        //enviaMsgSerie("Estoy vivo\n\r",0);
        
        LED=true;
        ms100(5);
        LED=false;
        ms100(5);
    }
}

void __interrupt ISR (void) {
    if (RCIE && RCIF) {
        //Error reading
        rx_err=RCSTA;
        //As RCREG is argument, their reading implies the RCIF erasing
        IncomingHCIpacker(RCREG);
    } else if (CCP1IE && CCP1IF) {
        //TimeOut
        CCP1IF = false;
        TMR1ON = false;
        timeouts++;
    } else if (INTCONbits.PEIE == 1 && PIE2bits.BCLIE == 1 && PIR2bits.BCLIF == 1) {
        I2C_BusCollisionISR();
    } else if (INTCONbits.PEIE == 1 && PIE1bits.SSPIE == 1 && PIR1bits.SSPIF == 1) {
        I2C_ISR();
    } else {
        //Unhandled Interrupt
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

/**
 * Handler for (pre)processing of an incoming HCI message. Once the user exits
 * from this handler function, the RxMessage.size variable gets cleared!
 */
void ProcesaHCI() {
    if (RxMessage.check) {
        pendingmsg = true;
        lengthrx = RxMessage.size;
    }
}

void enviaMsgSerie(const unsigned char *arreglo,unsigned char largo) {
    unsigned char aux=0;
    if (!largo)
        largo=strlen(arreglo);
    
    while (aux<largo)
        EUSART_Write(*(arreglo+(aux++)));
}