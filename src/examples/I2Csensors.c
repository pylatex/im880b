/*
 * Archivo de ejemplo, para implementacion en microcontroladores.
 * Se pretende que se requieran interrupciones para el funcionamiento.
 *
 * La mayoria de archivos vienen del comprimido
 * WiMOD_LoRaWAN_ExampleCode_HCI_C_V0_1.zip
 * y han sido ligeramente modificados para soportar tipos estandar.
 */

#include <stdio.h>
#include "SerialDevice.h"
#include "nucleoPIC.h"

#include "mcc.h"
#include "T67xx.h"
#include "hdc1010.h"
#include "iaq.h"
#include "bh1750fvi.h"
#include "bmp280.h"

//------------------------------------------------------------------------------
//  Definitions and Setup
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Declarations, Function Prototypes and Variables
//------------------------------------------------------------------------------
volatile unsigned char rx_err;  //Error de recepcion en UART

//Demoras
void ms100(unsigned char q);    //Demora de (100*q) ms
void msdelay (unsigned char cantidad);  //Demora activa en ms
void StartTimerDelayMs(unsigned char cant); //Demora por TIMER 1
//Utilidades Sistema
void enableInterrupts (void);   //Habilita las interrupciones
//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

/**
 * Programa Principal
 */
void main(void)
{
    SYSTEM_Initialize();
    SerialDevice_Open("",B115200,8,0);
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    #ifdef HDC1010_H
    unsigned short temp,hum;
    HDCinit (&temp,&hum,msdelay);
    #endif

    #ifdef BH1750FVI_H
    unsigned short light;
    BH_DVI_SetLow();
    __delay_us(5);
    BH_DVI_SetHigh();
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
        LED_SetHigh();
        char buff[30],phlen;

        //Pruebas con T6713
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
        LED_SetHigh();
        ms100(5);
        LED_SetLow();
        ms100(45);
    }
}

volatile bool libre = true;
volatile bool lleno = false;
char minibuff,HCIbuff[20],rxcnt;

/**
 * Demora activa
 * @param cantidad: Duracion en milisegundos
 */
void msdelay (unsigned char cantidad) {
    StartTimerDelayMs(cantidad);
    while (TMR1ON);
}

/**
 * Generates a delay of q*100 ms
 * @param q: multiplier.
 */
void ms100 (unsigned char q) {
    while (q--)
        __delay_ms(100);    //XC8 compiler
}
