/*
 * Archivo de ejemplo, para implementacion en microcontroladores.
 * Se pretende que se requieran interrupciones para el funcionamiento.
 *
 * La mayoria de archivos vienen del comprimido
 * WiMOD_LoRaWAN_ExampleCode_HCI_C_V0_1.zip
 * y han sido ligeramente modificados para soportar tipos estandar.
 */

#include <xc.h>
#include <string.h>
#include <stdio.h>
#include "SerialDevice.h"

#include "i2c.h"
#include "T67xx.h"
#include "hdc1010.h"
#include "iaq.h"
#include "bh1750fvi.h"
#include "bmp280.h"

//------------------------------------------------------------------------------
//  Definitions and Setup
//------------------------------------------------------------------------------

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
#define DVI LATC1   //Pin DVI del sensor BH1750FVI
#endif

//------------------------------------------------------------------------------
//  Declarations, Function Prototypes and Variables
//------------------------------------------------------------------------------
volatile unsigned char rx_err;  //Error de recepcion en UART

//Demoras
void ms100(unsigned char q);    //Demora de (100*q) ms
void msdelay (unsigned char cantidad);  //Demora activa en ms
void StartTimerDelayMs(unsigned char cant); //Demora por TIMER 1
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
    ppsLock(false);
    //Entradas y salidas UART
    RXPPS=0x16;     //Rx viene de RC6
    RB5PPS=0x16;    //Tx va hacia RB5

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

    I2C_Initialize();
    SerialDevice_Open("",B115200,8,0);
}

/**
 * Programa Principal
 */
void main(void)
{
    setup();
    enableInterrupts();

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
    BMP280init(false);
    BMP280writeCtlMeas(BMPnormalMode | BMPostX1 | BMPospX1);
    #endif

    while (true) {
        //Pruebas con sensor CO2 Telaire T6713.
        char buff[30],phlen;

        //Pruebas con sensor T6713
        #ifdef T6700_H
        SerialDevice_SendData((char *)"T6713 - ",0);
        unsigned short rsp;
        if (T67xx_CO2(&rsp)) {
            phlen=(char)sprintf(buff,"CO2: %u PPM\r\n",rsp);
            SerialDevice_SendData(buff,phlen);
        } else {
            SerialDevice_SendData((char *)"No hubo lectura\r\n",0);
        }
        #endif

        //Pruebas con BMP280
        #ifdef BMP280_H
        unsigned short val;
        unsigned long val2;
        char i=0,mem[30];
        if (BMP280readTrimming(&mem[0]) && BMP280readValues(&mem[24])){
            SerialDevice_SendData((char *)"BMP - TRIM VALUES (T1 T2 T3 P1 ... P9):\n\r",0);
            while (i<24) {
                val  = (unsigned short)mem[i++] << 8;
                val += mem[i++];
                phlen=(char)sprintf(buff,(i==2 || i==8)?"%u ":"%i ",val);
                SerialDevice_SendData(buff,phlen);
            }
            SerialDevice_SendData((char *)"\n\rBMP - SENS VALUES (PRESS TEMP):\n\r",0);
            while (i<30) {
                val2  = (unsigned long)mem[i++] << 12;
                val2 += (unsigned long)mem[i++] << 4;
                val2 += (unsigned long)mem[i++] >> 4;
                phlen = (char)sprintf(buff,"%lu ",val2);
                SerialDevice_SendData(buff,phlen);
            }
            SerialDevice_SendData((char *)"\n\r",0);
        } else {
            SerialDevice_SendData((char *)"BMP - No hubo lectura\n\r",0);
        }
        #endif

        //Pruebas con BH1750FVI
        #ifdef BH1750FVI_H

        SerialDevice_SendData((char *)"BH1750 - ",0);
        if (BHread(&light)) {
            phlen=(char) sprintf(buff,"Luz: %u\n\r",light);
            SerialDevice_SendData(buff,phlen);
        } else {
            SerialDevice_SendData((char *)"No hubo lectura\n\r",0);
        }
        #endif

        //Pruebas con iAQ-Core
        #ifdef IAQ_H
        const char e_ok[]= "OK";
        const char e_run[]="RUNIN";
        const char e_busy[]="BUSY";
        const char e_rr[]="ERROR";
        char const *rspsens;
        IAQ_T iaq;

        SerialDevice_SendData((char *)"iAQ Core - ",0);
        if (iaq_read(&iaq)) {
            /*
            for (char i=0;i<9;i++){
                phlen=(char) sprintf(phrase,"%#02X ",iaq.raw[i]);
                enviaMsgSerie(phrase,(unsigned char)phlen);
            }
            EUSART_Write('\n');
            EUSART_Write('\r');
            // */
            switch (iaq.status) {
                case IAQ_OK:
                    rspsens=e_ok;
                    break;
                case IAQ_RUNIN:
                    rspsens=e_run;
                    break;
                case IAQ_BUSY:
                    rspsens=e_busy;
                    break;
                default:
                case IAQ_ERROR:
                    rspsens=e_rr;
                    break;
            }
            phlen=(char)sprintf(buff,"CO2: %u PPM, ",iaq.pred);
            SerialDevice_SendData(buff,phlen);
            phlen=(char)sprintf(buff,"Estado: %s, ",rspsens);
            SerialDevice_SendData(buff,phlen);
            phlen=(char)sprintf(buff,"Resistencia: %lu, ",iaq.resistance);
            SerialDevice_SendData(buff,phlen);
            phlen=(char)sprintf(buff,"TVOC: %u PPB\n\r",iaq.tvoc);
            SerialDevice_SendData(buff,phlen);
        } else {
            SerialDevice_SendData((char *)"No hubo lectura\r\n",0);
        }
        #endif

        //Pruebas con HDC1010
        #ifdef HDC1010_H
        SerialDevice_SendData((char *)"HDC1010 - ",0);
        if (HDCboth()) {
            phlen=(char)sprintf(buff,"Temperatura: %u, Humedad: %u\n\r",temp,hum);
            SerialDevice_SendData(buff,phlen);
        } else {
            SerialDevice_SendData((char *)"No hubo lectura\n\r",0);
        }
        #endif

        SerialDevice_SendData((char *)"\n\r",0);
        LED=true;
        ms100(5);
        LED=false;
        ms100(45);
    }
}

volatile bool libre = true;
volatile bool lleno = false;
char minibuff,HCIbuff[20],rxcnt;

/**
 * Rutina de atencion de interrupciones
 */
void __interrupt ISR (void) {
    if (RCIE && RCIF) {
        rx_err=RCSTA;
        rx_err=RCREG; //Reception will not be used
    } else
    if (CCP1IE && CCP1IF) {
        //TimeOut
        CCP1IF = false;
        TMR1ON = false;
        #ifdef TEST_2
        LED=false;
        #endif
    }
    else if (I2C_ISR_COLLISION_CONDITION()) {
        I2C_BusCollisionISR();
    } else if (I2C_ISR_EVENT_CONDITION()) {
        I2C_ISR();
    }
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
 * Inhabilita/Habilita el cambio de perifericos
 * @param lock: 1 para bloquear los cambios a registros PPS, 0 para desbloquear.
 */
#ifdef _16F1769
void ppsLock (bool lock){
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = lock?1u:0u; // lock PPS
}
#endif
