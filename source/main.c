/*
 * Archivo de ejemplo, para implementacion en microcontroladores.
 * Se pretende que se requieran interrupciones para el funcionamiento.
 * 
 * La mayoria de archivos vienen del comprimido
 * WiMOD_LoRaWAN_ExampleCode_HCI_C_V0_1.zip
 * y han sido ligeramente modificados para soportar tipos estandar.
 */
#include <xc.h>
//#include <string.h>
#include <stdbool.h>
#include "globaldefs.h"
#include "lorawan_hci.h"

#ifdef UC_PIC8

volatile bool ping;
volatile unsigned char rx_err,rx_val; //Relacionados con el receptor

volatile unsigned char estado_rx;   //Reflejo del ultimo estado HCI del receptor
volatile unsigned char buffer1[20]; //Buffer de salida
//volatile unsigned char buffer2[20]; //Buffer de llegada

#define _XTAL_FREQ 8000000  //RC interno
//#define _XTAL_FREQ 7372800  //Cristal externo
//*
//Configuracion valida para PIC18F2550, con INTOSC en 8 MHz
#pragma config PLLDIV = 1, CPUDIV = OSC1_PLL2, USBDIV = 1
#pragma config FOSC = INTOSCIO_EC,  FCMEN = ON, IESO = OFF
#pragma config PWRT = ON, BOR = OFF, BORV = 3, VREGEN = OFF
#pragma config WDT = OFF, WDTPS = 32768
#pragma config CCP2MX = ON, PBADEN = OFF, LPT1OSC = ON, MCLRE =	ON
#pragma config STVREN = OFF, LVP = OFF, DEBUG=OFF, XINST = OFF
//*/

#define LED LATA0 //Para las pruebas de parpadeo y ping
#define PIN RC0 //Para prueba LED=PIN

void cienmilis (unsigned char cant) {
    while (cant--)
        __delay_ms(100);
}
#endif

#ifdef Q_OS_WIN
int main(int argc, char *argv[])
#endif
#ifdef UC_PIC8
void main(void) 
#endif
{
#ifdef Q_OS_WIN
    ////////////////////////////////////////////////////////////////////////////
    //Codigo del main() del ejemplo de IMST:
    ////////////////////////////////////////////////////////////////////////////

    bool run = true;

    char comPort[40];

    // comport parameter attached ?
    if (argc >= 2)
    {
        strncpy(comPort, argv[1], 40);
    }
    else
    {
        strcpy(comPort, defPORT);
        printf("usage: WiMOD_LoRaWAN_HCI_C_ExampleCode COMxy");
    }

    // init interface:
    if(!WiMOD_LoRaWAN_Init(comPort))
    {
        printf("error - couldn't open interface on comport %s\r\n",comPort);
        printf("try: iMOD_LoRaWAN_HCI_C_ExampleCode COMxy to select another comport\n\r");

        return -1;
    }

    // show menu
    ShowMenu(comPort);

    // main loop
    while(run) {
        // handle receiver process
        WiMOD_LoRaWAN_Process();

        // keyboard pressed ?
        if(kbhit())
        {
            // get command
            char cmd = getch();

            printf("%c\n\r",cmd);

            // handle commands
            switch(cmd)
            {
                case    'e':
                case    'x':
                        run = false;
                        break;

                case    'i':
                        // get device info
                        GetDeviceInfo();
                        break;


                case    'p':
                        // ping device
                        Ping();
                        break;

                case    'j':
                        // join network
                        Join();
                        break;

                case    'u':
                        // send u-data
                        SendUData();
                        break;

                case    'c':
                        // send c-data
                        SendCData();
                        break;

                case    ' ':
                        ShowMenu(comPort);
                        break;
            }
        }
    }
    return 0;
    //Fin Codigo IMST.
    ////////////////////////////////////////////////////////////////////////////
#endif
#ifdef UC_PIC8
    ////////////////////////////////////////////////////////////////////////////
    //Codigo para PIC de 8 bits.

    //INICIALIZACION
    OSCCON=0x73;    //Interno a 8 MHz (aplica a: 18F2550)
    while(!IOFS);   //Espera que se estabilice
    
    //Parpadeo de LED (idealmente) en RC0
    ADCON1=0x0F;    //Todos los pines son digitales
    PORTA=0;
    LATA=0;
    TRISA=0xFE; //Para RC0 como salida.
    
    //UART, Ajustes comunes a Rx y Tx. Inicializado de acuerdo a datasheet 16F2550
    //Se prueba con 8(interno) y 7.3728(externo) MHz
    ////1: (En reset,SPBRG=0). Usar BRG16=1 y BRGH=1. Velocidades despues de PLL (si lo hay)
    SPBRG=16;   //Fosc=8 MHz (ejm,interno)
    //SPBRG=15;   //Fosc=7.3728 MHz (externo)
    SYNC=0; //2. Modo Asincrono
    SPEN=1; //2. Habilita Puerto Serie
    TXEN=1; //6,Tx. Habilita transmisor
    //BUCLE
    ping=false;
    while (true) {
        //Enviar Ping
        //SendHCI();    //TXREG=0x69;
        //Booleano indicando respuesta pendiente = true
        //Habilitar interrupcion por recepcion
        
        LED=ping;
        cienmilis(5);
        ping=false;
        LED=false;
        cienmilis(5);
        
    }
    //Fin Codigo PIC 8 bits
    ////////////////////////////////////////////////////////////////////////////
#endif
}

#ifdef UC_PIC8
void interrupt ISR (void) {
    if (RCIE && RCIF) {
        //Borrado de Bandera
        rx_err=RCSTA;
        rx_val=RCREG; //Esto borra RCIF (PIR1)
        //Procesamiento
        estado_rx=ProcessHCI(rx_val);
    }
}
#endif