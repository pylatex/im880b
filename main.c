/*
 * Archivo de ejemplo, para implementacion en microcontroladores.
 * Se pretende que se requieran interrupciones para el funcionamiento.
 * 
 * La mayoria de archivos vienen del comprimido
 * WiMOD_LoRaWAN_ExampleCode_HCI_C_V0_1.zip
 * y han sido ligeramente modificados para soportar tipos estandar.
 */
#include <xc.h>
#include "globaldefs.h"

#ifdef UC_PIC8

//*
//Configuracion valida para PIC18F2550, con oscilador interno
#define _XTAL_FREQ 8000000
#pragma config PLLDIV = 1, CPUDIV = OSC1_PLL2, USBDIV = 1
#pragma config FOSC = INTOSCIO_EC,  FCMEN = ON, IESO = OFF
#pragma config PWRT = ON, BOR = OFF, BORV = 3, VREGEN = OFF
#pragma config WDT = OFF, WDTPS = 32768
#pragma config CCP2MX = ON, PBADEN = OFF, LPT1OSC = ON, MCLRE =	ON
#pragma config STVREN = OFF, LVP = OFF, DEBUG=OFF, XINST = OFF
//*/

#define LED LATA0 //(Usado para la prueba de parpadeo)
#define PIN RC0

void cienmilis (unsigned char cant) {
    while (cant--)
        __delay_ms(100);
}
#endif

#ifdef Q_OS_WIN

//Todo el codigo del main del ejemplo de IMST...
int main(int argc, char *argv[])
#elif defined UC_PIC8
void main(void) 
#endif
{
#ifdef Q_OS_WIN
    ////////////////////////////////////////////////////////////////////////////
    //Codigo del main del ejemplo de IMST:
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
    OSCCON=0x73;    //Interno a 8 MHz
    while(!IOFS);   //Espera que se estabilize
    
    //Parpadeo de LED (idealmente) en RC0
    ADCON1=0x0F;    //Todos los pines son digitales
    PORTA=0;
    LATA=0;
    TRISA=0xFE; //Para RC0 como salida.
    
    //BUCLE
    while (true) {
        //LED=PIN;
        //*
        LED=1;
        cienmilis(5);
        LED=0;
        cienmilis(5);
        //*/
    }
    //Fin Codigo PIC 8 bits
    ////////////////////////////////////////////////////////////////////////////
#endif
}

#if defined UC_PIC8
void interrupt ISR (void) {
    ;
}
#endif