/*
 * Archivo de ejemplo, para implementacion en microcontroladores.
 * Se pretende que se requieran interrupciones para el funcionamiento.
 * 
 * La mayoria de archivos vienen del comprimido
 * WiMOD_LoRaWAN_ExampleCode_HCI_C_V0_1.zip
 * y han sido ligeramente modificados para soportar tipos estandar.
 */

//MODOS DE COMPILACION. Descomentar el que se quiera probar:
#define SMACH       //Maquina de estados (principal)
//#define TEST_1      //Verificacion UART/Reloj
//#define TEST_2      //Verificacion comunicacion PIC-WiMOD
//#define TEST_3      //Verificacion I2C(con sensor CO2 de Telaire)/UART
//#define TEST_4      //Medicion ADC y envio por UART
//------------------------------------------------------------------------------
//  Definitions and Setup
//------------------------------------------------------------------------------
#if defined TEST_1 || defined TEST_3 || defined TEST_4
#include <string.h>
#include <stdio.h>
#endif
#include <xc.h>
#if defined SMACH || defined TEST_2
#include "WMLW_APIconsts.h"
#include "hci_stack.h"
#endif
#if defined TEST_1 || defined TEST_3 || defined TEST_4
#include "SerialDevice.h"
#endif
#if defined SMACH || defined TEST_3
#include "i2c.h"
#include "T67xx.h"
#endif
#if defined TEST_4 || defined SMACH
#include "MQ2.h"
#endif

#define _XTAL_FREQ 8000000  //May be either Internal RC or external oscillator.
//#define _XTAL_FREQ 7372800  //External Quartz Crystal to derivate common UART speeds

#ifdef _18F2550
//PIC18F2550 with INTOSC@8MHz
#pragma config PLLDIV = 1, CPUDIV = OSC1_PLL2, USBDIV = 1
#pragma config FOSC = INTOSCIO_EC,  FCMEN = ON, IESO = OFF
#pragma config PWRT = ON, BOR = OFF, BORV = 3, VREGEN = OFF
#pragma config WDT = OFF, WDTPS = 32768
#pragma config CCP2MX = ON, PBADEN = OFF, LPT1OSC = ON, MCLRE =	ON
#pragma config STVREN = OFF, LVP = OFF, DEBUG=OFF, XINST = OFF

#define LED LATA1 //Para las pruebas de parpadeo y ping
//#define PIN RC0   //Para probar en un loop con LED=PIN
#endif

#ifdef _16F1769
#pragma config FOSC = INTOSC, WDTE = OFF, PWRTE = ON, MCLRE = OFF, CP = OFF, BOREN = OFF, CLKOUTEN = OFF, IESO = ON, FCMEN = ON
#pragma config WRT = OFF, PPS1WAY =	OFF, ZCD = OFF, PLLEN =	OFF, STVREN = ON, BORV = HI, LPBOR =	OFF, LVP = OFF

#define LED LATA0   //Para las pruebas de parpadeo y ping
//#define PIN RB5     //Para probar en un loop con LED=PIN
#endif

#ifdef SERIAL_DEVICE_H
#define EUSART_Write(x) SerialDevice_SendByte(x)
#endif

//------------------------------------------------------------------------------
//  Declarations, Function Prototypes and Variables
//------------------------------------------------------------------------------

void ms100(unsigned char q); //A (100*q) ms delay
void ProcesaHCI(); //Procesamiento de HCI entrante
#ifdef SERIAL_DEVICE_H
void enviaMsgSerie(const unsigned char *arreglo,unsigned char largo);
#endif

volatile unsigned char rx_err; //Relacionados con el receptor
#ifdef LORAWAN_HCI_H
volatile static HCIMessage_t TxMessage;
volatile static HCIMessage_t RxMessage;
#endif
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
    CCPR1=(unsigned short)((cant<<8)-(cant<<2)-(cant<<1));
    CCP1IF=false;   //Restablecer comparador
    CCP1CON=0x8B;   //Modulo CCP en Comparacion a (representado) cant/ms
    T1CON=0x30;     //Prescale 1:8
    TMR1=0;
    TMR1ON=true;
}

void blink (unsigned char cant,unsigned char high,unsigned char low) {
    while (cant--) {
        LED=true;
        ms100(high);
        LED=false;
        ms100(low);
    }
}

void setup (void) {

    #ifdef _18F2550
    OSCCON=0x73;    //Internal at 8 MHz
    while(!IOFS);   //Waits for stabilization
    //ADCON1=0x0F;    //All pins as digital
    PORTA=0;
    LATA=0;
    TRISA=0xFD; //RA0 as output

    //Inicializacion del ADC
    ADCON1=0x0E;
    ADCON2=0x83;
    #endif

    #ifdef _16F1769
    OSCCON=0x70;    //Internal at 8 MHz
    while(!HFIOFS); //May be either PLLR or HFIOFS. See OSCSTAT register for specific case
    PORTA=0;
    LATA=0;
    ANSELA=0;       //All pins as digital
    TRISA=0xFA;     //Pines 2 y 0 son salidas digitales

    //Entradas y salidas UART
    RXPPS=0x05;     //Rx viene de RA5
    RA2PPS=0x16;    //Tx va hacia RA2

    //Entradas y salidas I2C se dejan en los pines por defecto: SCK:RB6, SDA:RB4
    //(unicos totalmente compatibles con I2C/SMBus, segun seccion 12.3 del datasheet)
    RB6PPS=0x12;
    RB4PPS=0x13;
    SSPDATPPS = 0x0C;   //RB4->MSSP:SDA;
    SSPCLKPPS = 0x0E;   //RB6->MSSP:SCL;
    ANSELB=0;       //Todos los pines son digitales.

    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
    #endif

    ms100(2);   //Delay for stabilization of power supply

    #ifdef _I2C_H
    I2C_Initialize();
    #endif
    #ifdef SERIAL_DEVICE_H
    SerialDevice_Open("",8,0);
    #endif
    #ifdef LORAWAN_HCI_H
    InitHCI(ProcesaHCI,&RxMessage);
    #endif
}

void enableInterrupts (void) {
    CCP1IE = true; //Comparison, for timeouts.
    PEIE = true; //Peripheral Interrupts Enable
    GIE = true; //Global Interrupts Enable
}

/**
 * Main
 */
void main(void)
{
    setup();
    #ifdef T6700_H
    unsigned char *respuesta;
    #endif
    #ifdef SMACH
    enum {
        RESET, TestUART, GetNwkStatus, NWKinactive, NWKjoining, NODEidleActive, 
        NWKaccept, SENSprocess
    } status = RESET; //Initial State for the machine of Main.
    #endif

    #ifndef TEST_4
    enableInterrupts();
    #endif

    while (true) {
        //State Machine Description
        #ifdef SMACH
        switch (status) {
            case RESET:
                pendingmsg = false;
                timeouts = 0;
                if (InitHCI(ProcesaHCI,(HCIMessage_t *) &RxMessage))
                    status = TestUART; //Full Duplex UART and Rx interruptions enabled
                break;
            case TestUART:
                //Message Setup
                TxMessage.SapID = DEVMGMT_ID;
                TxMessage.MsgID = DEVMGMT_MSG_PING_REQ;
                TxMessage.size = 0;
                //Send Message
                SendHCI((HCIMessage_t *)&TxMessage);
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
                SendHCI((HCIMessage_t *)&TxMessage);
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
                                status = NODEidleActive;
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
                SendHCI((HCIMessage_t *)&TxMessage);
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
                            status = NODEidleActive;
                        }
                    }
                }
                break;
            case NODEidleActive:
                //Create more states to attend other HCI messages or modify this state
                ms100(49);//Espera ~5 segundos
                status = SENSprocess;
                break;
            case SENSprocess:
                LED=true;
                //Starts an I2C reading and decides upon the response.
                respuesta=T67XX_Read(T67XX_GASPPM_FC,T67XX_GASPPM,4);
                TxMessage.SapID=LORAWAN_ID;
                TxMessage.MsgID=LORAWAN_MSG_SEND_UDATA_REQ;
                TxMessage.Payload[0]=5; //Puerto LoRaWAN
                if (respuesta) {
                    TxMessage.Payload[1]=1; //Lectura: CO2
                    TxMessage.Payload[2]=respuesta[2];  //MSB
                    TxMessage.Payload[3]=respuesta[3];  //LSB
                    TxMessage.size=4;
                } else {
                    TxMessage.Payload[1]=0; //Lectura: Ninguna (Error de conexion con sensor)
                    TxMessage.size=2;
                }
                SendHCI((HCIMessage_t *)&TxMessage);
                status = NODEidleActive;
                ms100(1);   //Completa los 5 segundos...
                LED=false;
                break;
            default:
                break;
        }
        #endif

        //Prueba 1: Verificacion UART y Reloj ~ 1 Hz
        #ifdef TEST_1
        LED = true;
        enviaMsgSerie("estoy vivo\n\r",0);
        ms100(5);
        LED = false;
        ms100(5);
        #endif

        //Prueba 2: Envio de Ping hacia iM880B (puede verse con WiMOD LoRaWAN DevTool)
        #ifdef TEST_2
        TxMessage.SapID = DEVMGMT_ID;           //Message Setup
        TxMessage.MsgID = DEVMGMT_MSG_PING_REQ;
        TxMessage.size = 0;
        SendHCI(&TxMessage);                    //Send Message
        ms100(10);
        #endif

        //Prueba 3: I2C hacia sensor CO2 Telaire T6713.
        #ifdef TEST_3
        respuesta=T67XX_Read(T67XX_GASPPM_FC,T67XX_GASPPM,4);
        unsigned char phrase[30],phlen;

        if (respuesta) {
            unsigned short valor=(unsigned short)((respuesta[2]<<8)|(respuesta[3]));
            phlen=sprintf(phrase,"CO2: %u PPM\n\r",valor);
            enviaMsgSerie((unsigned const char *)phrase,phlen);
        } else {
            enviaMsgSerie("No hubo lectura\n\r",0);
        }

        LED=true;
        ms100(5);
        LED=false;
        ms100(5);
        #endif

        //Prueba 4: Medicion ADC y envio por UART
        #ifdef TEST_4
        unsigned char phrase[15],phlen;
        phlen=sprintf(phrase,"Propano:%u\n\r",valorPropano());
        enviaMsgSerie(phrase,phlen);
        ms100(10);
        #endif
    }
}

void __interrupt ISR (void) {
    #ifdef LORAWAN_HCI_H
    if (RCIE && RCIF) {
        //Error reading
        rx_err=RCSTA;
        //As RCREG is argument, their reading implies the RCIF erasing
        IncomingHCIpacker(RCREG);
    } else
    #endif
    if (CCP1IE && CCP1IF) {
        //TimeOut
        CCP1IF = false;
        TMR1ON = false;
        timeouts++;
        #ifdef TEST_2
        LED=false;
        #endif
    }
    #ifdef _I2C_H
    else if (I2C_ISR_COLLISION_CONDITION()) {
        I2C_BusCollisionISR();
    } else if (I2C_ISR_EVENT_CONDITION()) {
        I2C_ISR();
    }
    #endif
    else {
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

#ifdef LORAWAN_HCI_H
/**
 * Handler for (pre)processing of an incoming HCI message. Once the user exits
 * from this handler function, the RxMessage.size variable gets cleared!
 */
void ProcesaHCI() {
    if (RxMessage.check) {
        pendingmsg = true;
        lengthrx = RxMessage.size;
        #ifdef TEST_2
        LED=true;
        StartTimerDelayMs(100);
        #endif
    }
}
#endif

#ifdef SERIAL_DEVICE_H
void enviaMsgSerie(const unsigned char *arreglo,unsigned char largo) {
    unsigned char aux=0;
    if (!largo)
        largo=strlen(arreglo);

    while (aux<largo)
        EUSART_Write(*(arreglo+(aux++)));
}
#endif
