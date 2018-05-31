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
//#define TEST_4      //Medicion ADC y envio por UART

//------------------------------------------------------------------------------
//  Definitions and Setup
//------------------------------------------------------------------------------
#include <xc.h>
#include <string.h>
#include "SerialDevice.h"
#include "pylatex.h"

#if defined TEST_1 || defined TEST_3 || defined TEST_4
#include <string.h>
#include <stdio.h>
#endif
#if defined TEST_2
void ProcesaHCI();
volatile unsigned bool pendingmsg;
#endif
#if defined TEST_1 || defined TEST_3 || defined TEST_4
#include "SerialDevice.h"
#endif
#if defined SMACH || defined TEST_3
#include "i2c.h"
#include "T67xx.h"
#include "hdc1010.h"
#include "iaq.h"
#include "bh1750fvi.h"
#include "bmp280.h"
#endif
#if defined SMACH || defined TEST_4
#include "ADC.h"
#endif

#define _XTAL_FREQ 8000000  //May be either Internal RC or external oscillator.
//#define _XTAL_FREQ 7372800  //External Quartz Crystal to derivate common UART speeds

#ifdef _18F2550 //PIC18F2550
#pragma config PLLDIV = 1, CPUDIV = OSC1_PLL2, USBDIV = 1
#if _XTAL_FREQ == 8000000
#pragma config FOSC = INTOSCIO_EC,  FCMEN = ON, IESO = OFF // INTOSC @ 8MHz
#elif _XTAL_FREQ == 7372800
#pragma config FOSC = HS,  FCMEN = ON, IESO = OFF // HS @ 7.3728MHz
#endif
#pragma config PWRT = ON, BOR = OFF, BORV = 3, VREGEN = OFF
#pragma config WDT = OFF, WDTPS = 32768
#pragma config CCP2MX = ON, PBADEN = OFF, LPT1OSC = ON, MCLRE =	ON
#pragma config STVREN = OFF, LVP = OFF, DEBUG=OFF, XINST = OFF

#define LED LATA1 //Para las pruebas de parpadeo y ping
//#define PIN RC0   //Para probar en un loop con LED=PIN
#define DVI LATA2   //Pin DVI del sensor BH1750FVI
#endif

#ifdef _16F1769
#pragma config FOSC = INTOSC, WDTE = OFF, PWRTE = ON, MCLRE = ON, CP = OFF, BOREN = OFF, CLKOUTEN = OFF, IESO = ON, FCMEN = ON
#pragma config WRT = OFF, PPS1WAY =	OFF, ZCD = OFF, PLLEN =	OFF, STVREN = ON, BORV = HI, LPBOR =	OFF, LVP = OFF

#define LED LATC0   //Para las pruebas de parpadeo y ping
//#define PIN RB5     //Para probar en un loop con LED=PIN
#define DVI LATC1   //Pin DVI del sensor BH1750FVI
#endif

typedef enum {MODEM_LW,GPS,HPM,DEBUG1,DEBUG2} serial_t;

//------------------------------------------------------------------------------
//  Declarations, Function Prototypes and Variables
//------------------------------------------------------------------------------
static serial_t modoSerial;     //Elemento Serial que se esta controlando
volatile unsigned char rx_err;  //Error de recepcion en UART
static volatile bool delrun;    //Bandera para las demoras por TIMER1

//Demoras
void ms100(unsigned char q);    //Demora de (100*q) ms
void msdelay (unsigned char cantidad);  //Demora activa en ms
void StartTimerDelayMs(unsigned char cant); //Demora por TIMER 1
void blink (unsigned char cant,unsigned char high,unsigned char low); //Parpadeo

//Utiidades Serial
void cambiaSerial (serial_t serial);    //Para cambiar el elemento a controlar
void enviaIMST(char *arreglo,unsigned char largo);
void enviaGPS(char *arreglo,unsigned char largo);

//Utilidades Sistema
void ppsLock (bool state);      //Especifica el estado de bloqueo de PPS
void enableInterrupts (void);   //Habilita las interrupciones
//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

/**
 * Inicializacion de la mayoria de perifericos y otras
 * opciones del microcontrolador
 */
void setup (void) {

    //OSCILLATOR
#ifdef _18F2550
    OSCCON=0x73;    //Internal at 8 MHz
    while(!IOFS);   //Waits for stabilization
#endif
#ifdef _16F1769
    OSCCON=0x70;    //Internal at 8 MHz
    while(!HFIOFS); //May be either PLLR or HFIOFS. See OSCSTAT register for specific case
#endif

    //PINS SETUP
#ifdef _16F1769
    PORTC=0;
    LATC=0;
    ANSELA=0;       //All pins as digital
    ANSELC=0;       //All pins as digital
    ODCONC = 2;    //Enable Open Drain to drive DVI
    TRISC=0xFC; //RC0 and RC1 as outputs
#endif
#ifdef _18F2550
    PORTA=0;
    LATA=0;
    ADCON1=0x0E;
    ADCON2=0x83;
    TRISA=0xFD; //RA1 as output
#endif

    //UART & I2C
#ifdef _16F1769

    //Entradas y salidas I2C se dejan en los pines por defecto: SCK:RB6, SDA:RB4
    //(unicos totalmente compatibles con I2C/SMBus, segun seccion 12.3 del datasheet)
    RB6PPS=0x12;
    RB4PPS=0x13;
    SSPDATPPS = 0x0C;   //RB4->MSSP:SDA;
    SSPCLKPPS = 0x0E;   //RB6->MSSP:SCL;
    ANSELB=0;       //Todos los pines son digitales.

    ppsLock(true);
#endif

    ms100(2);   //Delay for stabilization of power supply

    #ifdef _I2C_H
    I2C_Initialize();
    #endif
}

/**
 * Programa Principal
 */
void main(void)
{
    setup();
    cambiaSerial (MODEM_LW);

    #ifndef TEST_4
    enableInterrupts();
    #endif

    #ifdef HDC1010_H
    unsigned short temp,hum;
    HDCinit (&temp,&hum,msdelay);
    #endif

    #ifdef BH1750FVI_H
    unsigned short light;
    DVI=false;
    __delay_us(5);
    DVI=true;
    BHinit(false);
    BHwrite(BH1750_RESET);
    BHwrite(BH1750_PWR_ON);
    BHwrite(BH1750_CONTINOUS | BH1750_LR);
    #endif

    #ifdef BMP280_H
    char mem[30];
    BMP280init(false);
    BMP280writeCtlMeas(BMPnormalMode | BMPostX1 | BMPospX1);
    #endif

    while (true) {
        //State Machine Description
        #ifdef SMACH

        LED=true;
        initLW((serialTransmitHandler)enviaIMST);
        LED=false;
        registerDelayFunction(StartTimerDelayMs,&delrun);
        for (char cnt=0;cnt<60;cnt++) {
        LED=true;

        #ifdef T6700_H //T6713 reading though I2C
        unsigned short rsp;
        if (T67xx_CO2(&rsp))
            AppendMeasure(PY_CO2,short2charp(rsp));
        #endif
        //AppendMeasure(PY_GAS,short2charp(valorPropano()));
        #ifdef BMP280_H //Pruebas con BMP280
        if (BMP280readTrimming(&mem[0]) && BMP280readValues(&mem[24])){
            AppendMeasure(PY_PRESS1,mem);
        }
        #endif
        #ifdef BH1750FVI_H
        if (BHread(&light)) {
            AppendMeasure(PY_ILUM1,short2charp(light));
        }
        #endif
        SendMeasures(PY_UNCONFIRMED);
        ms100(1);
        LED=false;
        ms100(49);  //Approx. each 5 sec ((49+1)x100ms)
        const char largo=14,test[]="Terminal GPS\n\r";
        enviaGPS((char *)test,largo);
        }
        #endif

        //Prueba 1: Verificacion UART y Reloj ~ 1 Hz
        #ifdef TEST_1
        LED = true;
        SerialDevice_SendData((char *)"estoy vivo\r\n",0);
        ms100(5);
        LED = false;
        ms100(5);
        #endif

        //Prueba 2: Envio de Ping hacia iM880B (puede verse con WiMOD LoRaWAN DevTool)
        #ifdef TEST_2
        WiMOD_LoRaWAN_SendPing();
        ms100(10);
        #endif

        //Prueba 4: Medicion ADC y envio por UART
        #ifdef TEST_4
        unsigned char phrase[15],phlen;
        phlen=sprintf(phrase,"Propano:%u\r\n",valorPropano());
        SerialDevice_SendData(phrase,phlen);
        ms100(10);
        #endif
    }
}

/**
 * Cambia la asignacion interna del EUSART
 * @param serial: El elemento con que se desea conectar (vease serial_t arriba)
 */
void cambiaSerial (serial_t serial){
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
            RB5PPS=0;    //RB5 recibe el Latch de su puerto
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
            break;

        case GPS:
            //Entradas y salidas UART
            RXPPS=0x16;     //Rx viene de RC6
            RB5PPS=0x16;    //Tx va hacia RB5
            break;

        default:
            break;
    }

    ppsLock(true);
    #endif
    SerialDevice_Open("",115200,8,0);
}

void enviaIMST(char *arreglo,unsigned char largo) {
    if (modoSerial != MODEM_LW)
        cambiaSerial(MODEM_LW);
    SerialDevice_SendData(arreglo,largo);
}

void enviaGPS(char *arreglo,unsigned char largo) {
    if (modoSerial != GPS)
        cambiaSerial(GPS);
    SerialDevice_SendData(arreglo,largo);
}

volatile bool libre = true;
volatile bool lleno = false;
char minibuff,HCIbuff[20],rxcnt;

/**
 * Rutina de atencion de interrupciones
 */
void __interrupt ISR (void) {
    if (RCIE && RCIF) {
        //Error reading
        rx_err=RCSTA;
        //As RCREG is argument, their reading implies the RCIF erasing
        switch (modoSerial) {
            case MODEM_LW:
                pylatexRx(RCREG);
                break;
            case HPM:
                libre = false;
            case GPS:
            default:
                minibuff = RCREG;
                break;
        }
    } else
    if (CCP1IE && CCP1IF) {
        //TimeOut
        CCP1IF = false;
        TMR1ON = false;
        delrun = false;
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
 * Habilita las interrupciones
 */
void enableInterrupts (void) {
    CCP1IE = true; //Comparison, for timeouts.
    PEIE = true; //Peripheral Interrupts Enable
    GIE = true; //Global Interrupts Enable
}

/**
 * Para hacer parpadear el led externo (definido anteriormente)
 * @param cant: Cantidad de parpadeos
 * @param high: Tiempo en alto (centenas de ms)
 * @param low:  Tiempo en bajo (centenas de ms)
 */
void blink (unsigned char cant,unsigned char high,unsigned char low) {
    while (cant--) {
        LED=true;
        ms100(high);
        LED=false;
        ms100(low);
    }
}

/**
 * Demora activa
 * @param cantidad: Duracion en milisegundos
 */
void msdelay (unsigned char cantidad) {
    StartTimerDelayMs(cantidad);
    while (TMR1ON);
}

/**
 * Demora con Timer 1 y CCP 1
 * @param cant: Duracion de la demora, en ms.
 */
void StartTimerDelayMs(unsigned char cant)
{
    //CCPR1=cant*250; //250*A = (256-6)*A = (256-4-2)*A
    CCPR1=(unsigned short)((cant<<8)-(cant<<2)-(cant<<1));
    CCP1IF=false;   //Restablecer comparador
    CCP1CON=0x8B;   //Modulo CCP en Comparacion a (representado) cant/ms
    T1CON=0x30;     //Prescale 1:8
    TMR1=0;
    TMR1ON=true;
    delrun=true;
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
 * (Pendiente) Rutina para leer valores del UART
 * @param rxBuffer:     arreglo en el cual escribir los valores
 * @param rxBufferSize: Limite del arreglo especificado
 * @return: Bytes leidos
 */
int SerialDevice_ReadData(UINT8 *rxBuffer, int rxBufferSize) {
    while (1) {
        StartTimerDelayMs(5);
        while (TMR1ON || libre);
        if (!libre) {
            rxBuffer[rxcnt++] = minibuff;
            if (rxcnt >= rxBufferSize)
                lleno=true;
        } else break;
    }
}

/**
 * Inhabilita/Habilita el cambio de perifericos
 * @param lock: 1 para bloquear los cambios a registros PPS, 0 para desbloquear.
 */
void ppsLock (bool lock){
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = lock?1u:0u; // lock PPS
}
