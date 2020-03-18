/*
 * Archivo de ejemplo, para implementacion en microcontroladores.
 * Se pretende que se requieran interrupciones para el funcionamiento.
 *
 * La mayoria de archivos vienen del comprimido
 * WiMOD_LoRaWAN_ExampleCode_HCI_C_V0_1.zip
 * y han sido ligeramente modificados para soportar tipos estandar.
 * Para Debug
 * Rx viene de RB7
 * Tx va hacia RC7
 * El Ultra sonido funciona a 5 V
 *  Pin Trigger HC-SR04
 *  Pin Echo HC-SR04
 * Inicia Medicion del ACD en AN7, osea pin RC3
 * 
 */

//MODOS DE COMPILACION. Descomentar el que se quiera probar:
//#define SMACH         //Maquina de estados (principal)
//#define TEST_1        //Verificacion UART/Reloj
//#define TEST_2        //Verificacion comunicacion PIC-WiMOD
#define TEST_3        //Medicion Distancia y ACD para envio por UART
//#define TEST_4        //Tarjeta RFID
//------------------------------------------------------------------------------
//  Definitions and Setup
//------------------------------------------------------------------------------
#include <xc.h>
#include <string.h>
#include <stdio.h>
#include "SerialDevice.h"
#include "pylatex.h"
#include "nucleoPIC.h"
#include "spi.h"


#if defined TEST_2
void ProcesaHCI();
volatile bool pendingmsg;
#endif

#if defined SMACH || defined TEST_3
#include "iaq.h"
#include "hcsr04.h"
#include "ADC.h"
#include "iaq.h"
#include "nmeaCayenne.h"
#endif

#if defined SMACH || defined TEST_4
#include "RC522.h"
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
extern void enviaIMST(uint8_t *arreglo,uint8_t largo);
extern void enviaGPS(uint8_t *arreglo,uint8_t largo);
extern void enviaDebug(uint8_t *arreglo,uint8_t largo);

/**
 * Programa Principal
 */
void main(void)
{
    setup();
    enableInterrupts();
    cambiaSerial (MODEM_LW);
    
    #if defined TEST_3 || defined TEST_1
    LED = true;
    char phrase[20];
    unsigned char phlen;
    phlen = sprintf(&phrase,"Iniciamos:\r\n");
    enviaDebug(phrase,phlen);
    #endif
    
    #if defined HCSR04_H
    int distance;//Variable donde se guarda la distancia
    HCSinit();
    #endif
    
    #if defined TEST_4
    unsigned char phrase[20];
    unsigned char i;
    sprintf(phrase,"Iniciamos\r\n");
    enviaDebug(phrase,0);
    #endif

    #if defined RC522_H
    PCD_Init();
    #endif

    #ifdef NMEACAYENNE_H
    NMEAdata_t NMEA;
    initNC(&NMEA);
    #endif

    #ifdef SMACH
    LWstat LWstatus;
    initLW((serialTransmitHandler)enviaIMST,&LWstatus);
    #endif

    while (true) {
        //State Machine Description
        #if defined SMACH
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
        
        const uint8_t CNT_BEFORE_REAUTH=60;
        
        for (char cnt=0;cnt<CNT_BEFORE_REAUTH;cnt++) {
            if (HCSread(&distance)){//le indica la direccion de la variable tipo entero distancia
                AppendMeasure(PY_DISTANCE,short2charp(distance));
            }
            AppendMeasure(PY_GAS,short2charp(ADC_Get_Data()));   
            SendMeasures(PY_UNCONFIRMED);
            ms100(50);  //Approx. each 5 sec ((49+1)x100ms)
            char buff[20],len;
            len=sprintf(buff,"Intento %2i/%2i\n\r",cnt+1,CNT_BEFORE_REAUTH);
            enviaDebug(buff,len);
        }
        #endif

        //Prueba 1: Verificacion UART y Reloj ~ 1 Hz
        #if defined TEST_1
        LED = true;
        ms100(10);
        LED = false;
        ms100(10);
        phlen = sprintf(&phrase,"Estoy Vivo\r\n");
        enviaDebug(&phrase,phlen);
        #endif

        //Prueba 2: Envio de Ping hacia iM880B (puede verse con WiMOD LoRaWAN DevTool)
        #if defined TEST_2
        WiMOD_LoRaWAN_SendPing();
        ms100(10);
        #endif

        //Prueba 4: Medicion ADC y envio por UART
        #if defined TEST_3
        LED = true;
        ms100(10);
        LED = false;
        ms100(10);
        phlen = sprintf(&phrase,"ADC DATO: %u\r\n",ADC_Get_Data());
        enviaDebug(&phrase,phlen);
        if (HCSread(&distance)) {//le indica la direccion de la variable tipo entero distancia
            phlen = sprintf(phrase,"Distancia:%i\r\n",distance);// construye la cadena y la guarda phrase y el tamaño en phlen
        }else{
            phlen = sprintf(phrase,"Distancia: No se envio nada\r\n"); 
        }  
        enviaDebug(&phrase,phlen);
        ms100(1);
        #endif

        //Prueba de tarjeta RFID
        #if defined TEST_4    
        
        if (PICC_IsNewCardPresent()){
            sprintf(phrase,"Te encontramos alelulla\n");
            enviaDebug(phrase,0);
             if (PICC_ReadCardSerial() )
            {
                  // Enviamos serialemente su UID
                  sprintf(phrase,"Card UID:");
                  enviaDebug(phrase,0);
                  for (i = 0; i < uid.size; i++) {
                          sprintf(phrase,uid.uidByte[i] < 0x10 ? " " : " ");
                          enviaDebug(phrase,0);
                          sprintf(phrase,"%02X",uid.uidByte[i]);   
                          enviaDebug(phrase,0);
                  } 
                  // Terminamos la lectura de la tarjeta  actual
                  PICC_HaltA(); 
             }
        }else{
            sprintf(phrase,"\n No Detectada\r\n"); 
            enviaDebug(phrase,0);  }
        ms100(10);//1 s
        #endif
        
    }
}

volatile bool libre = true;
volatile bool lleno = false;
char minibuff,HCIbuff[20],rxcnt;

/**
 * Rutina de atencion de interrupciones
 */
void __interrupt() ISR (void) 
{
    if (RCIE && RCIF) {
        //Error reading
        rx_err=RCSTA;
        //As RCREG is argument, their reading implies the RCIF erasing
        switch (modoSerial) {
            case MODEM_LW:
                pylatexRx(RCREG);
                break;
            case GPS:
                #ifdef NMEACAYENNE_H
                    NCinputSerial(RCREG);
                #endif
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
void blink (unsigned char cant,unsigned char high,unsigned char low) 
{
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
void msdelay (unsigned char cantidad) 
{
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
void ms100 (unsigned char q) 
{
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
