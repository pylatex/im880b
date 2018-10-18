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
#include <stdio.h>
#include "SerialDevice.h"
#include "pylatex.h"
#include "nucleoPIC.h"

#if defined TEST_2
void ProcesaHCI();
volatile unsigned bool pendingmsg;
#endif

#if defined SMACH || defined TEST_3
#include "i2c.h"
#include "T67xx.h"
#include "hdc1010.h"
#include "iaq.h"
#include "bh1750fvi.h"
#include "bmp280.h"
#include "nmeaCayenne.h"
#endif
#if defined SMACH || defined TEST_4
#include "ADC.h"
#endif

//------------------------------------------------------------------------------
//  Declarations, Function Prototypes and Variables
//------------------------------------------------------------------------------
extern serial_t modoSerial;     //Elemento Serial que se esta controlando
volatile unsigned char rx_err;  //Error de recepcion en UART
static volatile bool delrun;    //Bandera para las demoras por TIMER1

//Demoras
void ms100(unsigned char q);    //Demora de (100*q) ms
void msdelay (unsigned char cantidad);  //Demora activa en ms
void StartTimerDelayMs(unsigned char cant); //Demora por TIMER 1
void blink (unsigned char cant,unsigned char high,unsigned char low); //Parpadeo

//Utilidades Serial
extern void cambiaSerial (serial_t serial);    //Para cambiar el elemento a controlar
extern void enviaIMST(char *arreglo,unsigned char largo);
extern void enviaGPS(char *arreglo,unsigned char largo);
extern void enviaDebug(char *arreglo,unsigned char largo);

//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

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

    #ifdef NMEACAYENNE_H
    NMEAdata_t NMEA;
    initNC(&NMEA);
    #endif

    while (true) {
        //State Machine Description
        #ifdef SMACH

        LED=true;
        LWstat LWstatus;

        initLW((serialTransmitHandler)enviaIMST,&LWstatus);
        do {
            flag_t catch;
            WiMOD_LoRaWAN_nextRequest(&catch);
            StartTimerDelayMs(20);
            while (delrun && !catch); //Waits for timeout or HCI response/event
        } while (LWstatus != ACTIVE);
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
        if (BMP280readTrimming(&mem[0]) && BMP280readValues(&mem[24]))
            AppendMeasure(PY_COMP1,mem);
        #endif
        #ifdef BH1750FVI_H
        if (BHread(&light))
            AppendMeasure(PY_ILUM1,short2charp(light));
        #endif
        #ifdef NMEACAYENNE_H
        if (NCupdated()) {
            uint8_t buff[9];
            buff[0] = (NMEA.latitude >> 16) & 0xFF;
            buff[1] = (NMEA.latitude >> 8) & 0xFF;
            buff[2] = NMEA.latitude & 0xFF;
            buff[3] = (NMEA.longitude >> 16) & 0xFF;
            buff[4] = (NMEA.longitude >> 8) & 0xFF;
            buff[5] = NMEA.longitude & 0xFF;
            buff[6] = (NMEA.height >> 16) & 0xFF;
            buff[7] = (NMEA.height >> 8) & 0xFF;
            buff[8] = NMEA.height & 0xFF;
            AppendMeasure(PY_GPS,buff);
            ms100(1);
            LED=false;
            ms100(1);
            LED=true;
        }
        #endif

        SendMeasures(PY_UNCONFIRMED);
        ms100(1);
        LED=false;
        ms100(24); //Waits 2.5 seconds (LoRaWAN Rx Windows) prior changing to GPS
        cambiaSerial(GPS);
        ms100(75);  //Approx. each 10 sec ((75+24+1)x100ms)
        cambiaSerial(MODEM_LW);
        const char largo=14,test[]="Terminal GPS\n\r";
        enviaDebug((char *)test,largo);
        }
        #endif

        //Prueba 1: Verificacion UART y Reloj ~ 1 Hz
        #ifdef TEST_1
        LED = true;
        enviaDebug((char *)"estoy vivo\r\n",0);
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
            case GPS:
                NCinputSerial(RCREG);
                break;
            case HPM:
                libre = false;
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
