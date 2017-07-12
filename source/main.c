/*
 * Archivo de ejemplo, para implementacion en microcontroladores.
 * Se pretende que se requieran interrupciones para el funcionamiento.
 * 
 * La mayoria de archivos vienen del comprimido
 * WiMOD_LoRaWAN_ExampleCode_HCI_C_V0_1.zip
 * y han sido ligeramente modificados para soportar tipos estandar.
 */

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
#include <string.h>
#include "globaldefs.h"
#include "lorawan_hci.h"
#include "SerialDevice.h"

#ifdef UC_PIC8
#include <xc.h>
#endif

//------------------------------------------------------------------------------
//  Declarations and Definitions
//------------------------------------------------------------------------------

#ifdef Q_OS_WIN
// forward declarations
static void     ShowMenu(const char*);
static void     Ping();
static void     GetDeviceInfo();
static void     Join();
static void     SendUData();
static void     SendCData();
#endif

//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------
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

/**
 * Main
 */
#ifdef Q_OS_WIN
int main(int argc, char *argv[])
#endif
#ifdef UC_PIC8
void main(void) 
#endif
#ifdef Q_OS_WIN
{
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
}
#endif
#ifdef UC_PIC8
{
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
    
    SerialDevice_Open(0,0,0);
    //BUCLE
    ping=false;
    while (true) {
        //Enviar Ping
        //SendHCI();    //TXREG=0x69;
        //Booleano indicando respuesta pendiente = true
        //Habilitar interrupcion por recepcion
        buffer1[0]=DEVMGMT_SAP_ID;
        buffer1[1]=DEVMGMT_MSG_PING_RSP;
        buffer1[2]=0;
        SendHCI(buffer1,1);
        /*
        //Hacer creer que es el modulo y se esta enviando DEVMGMT_MSG_PING_RSP:
        SerialDevice_SendByte(0xC0);
        SerialDevice_SendByte(DEVMGMT_SAP_ID);
        SerialDevice_SendByte(DEVMGMT_MSG_PING_RSP);
        SerialDevice_SendByte(0x00);
        SerialDevice_SendByte(0xA0);
        SerialDevice_SendByte(0xAF);
        SerialDevice_SendByte(0xC0);
        //*/
        /*
        SerialDevice_SendByte('H');
        SerialDevice_SendByte('o');
        SerialDevice_SendByte('l');
        SerialDevice_SendByte('a');
        //*/
        //SerialDevice_SendData("holi",4);
        cienmilis(10); //Espera un segundo
        
        /*
        LED=ping;
        cienmilis(5);
        ping=false;
        LED=false;
        cienmilis(5);
        */
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
        estado_rx=ProcessHCI(buffer1,rx_val);
    }
}
#endif

#ifdef Q_OS_WIN
/**
 * ShowMenu
 * @brief: show main menu
 */
void ShowMenu(const char* comPort) {
    printf("\n\r");
    printf("------------------------------\n\r");
    printf("Using comport: %s\r\n", comPort);
    printf("------------------------------\n\r");
    printf("[SPACE]\t: show this menu\n\r");
    printf("[p]\t: ping device\n\r");
    printf("[i]\t: get device information\n\r");
    printf("[j]\t: join network request\n\r");
    printf("[u]\t: send unconfirmed radio message\n\r");
    printf("[c]\t: send confirmed radio message\n\r");
    printf("[e|x]\t: exit program\n\r");

}

/**
 * Ping
 * @brief: ping device
 */
void Ping() {
    printf("ping request\n\r");

    WiMOD_LoRaWAN_SendPing();
}

/**
 * GetDeviceInfo
 * @brief: get device information
 */
void GetDeviceInfo() {
    printf("get firmware version\n\r");

    WiMOD_LoRaWAN_GetFirmwareVersion();
}

/**
 * Join
 * @brief: ping device
 */
void Join() {
    printf("join network request\n\r");

    WiMOD_LoRaWAN_JoinNetworkRequest();
}

/**
 * SendUData
 * @brief: send unconfirmed radio message
 */
void SendUData() {
    printf("send U-Data\n\r");

    // port 0x21
    UINT8 port = 0x21;

    UINT8 data[4];

    data[0] = 0x01;
    data[1] = 0x02;
    data[2] = 0x03;
    data[3] = 0x04;

    // send unconfirmed radio message
    WiMOD_LoRaWAN_SendURadioData(port, data, 4);
}

/**
 * SendCData
 * @brief: send confirmed radio message
 */
void SendCData() {
    printf("send C-Data\n\r");

    // port 0x21
    UINT8 port = 0x23;

    UINT8 data[6];

    data[0] = 0x0A;
    data[1] = 0x0B;
    data[2] = 0x0C;
    data[3] = 0x0D;
    data[4] = 0x0E;
    data[5] = 0x0F;

    // send unconfirmed radio message
    WiMOD_LoRaWAN_SendCRadioData(port, data, 6);
}
#endif