/*------------------------------------------------------------------------------
  File:		main.cpp
  Abstract:	main module
	Version:	0.1
	Date:		18.05.2016
  Disclaimer:	This example code is provided by IMST GmbH on an "AS IS"
              basis without any warranties.

------------------------------------------------------------------------------*/
#define DEBUG
//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "target.h"   //Open and modify to change the target system (WIN/UX)
#include "SerialDevice.h"
#ifdef DEBUG
#include <time.h>
#endif // DEBUG
//------------------------------------------------------------------------------
//  Declarations and Definitions
//------------------------------------------------------------------------------

// forward declarations
static void     ShowMenu(const char*);
static void     Ping();
static void     GetDeviceInfo();
static void     Join();
static void     SendUData();
static void     SendCData();
static void     GetTime();
static void     GetLWstatus();

#define DEF_PORT "COM5"  //Default port, in case of empty input

//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

/**
 * Envio de mensaje por UART
 */
void SolicitaSensor() {
    unsigned char orden[] = {0x68,0x01,0x04,0x93};
    SerialDevice_SendData(orden,4);
}

#define MAXIMO_SENSOR 32
/**
 * Procesamiento de mensaje UART entrante
 */
void RespuestaSensor() {
    UINT8   rxBuf[MAXIMO_SENSOR];

    // read small chunk of data
    int rxLength = SerialDevice_ReadData(rxBuf, MAXIMO_SENSOR);

    // data available ?
    //void SLIP_DecodeData(UINT8 *srcData, int srcLength)
    // iterate over all received bytes
    printf("Respuesta:");
    for (unsigned char i=0;i<rxLength;i++) {
        printf(" %2X",rxBuf[i]);
    }
    printf("\n\r");
}

/**
 * Main
 */
int main(int argc, char *argv[])
{
    bool run = true;

    char comPort[40];

    // comport parameter attached ?
    if (argc >= 2)
    {
        strncpy(comPort, argv[1], 40);
    }
    else
    {
        strcpy(comPort, DEF_PORT);
        printf("usage: WiMOD_LoRaWAN_HCI_C_ExampleCode COMxy");
    }

    // init interface:
    if(!SerialDevice_Open(comPort,9600,8,0))
    {
        printf("error - couldn't open interface on comport %s\r\n",comPort);
        printf("try: WiMOD_LoRaWAN_HCI_C_ExampleCode COMxy to select another comport\n\r");

        return -1;
    }

    // show menu
    ShowMenu(comPort);

    // main loop
    while(run) {
        // handle receiver process
        RespuestaSensor();

        // keyboard pressed ?
        if(kbhit())
        {
            // get command
            char cmd = getch();

            printf("%c\n\r",cmd);
            #ifdef DEBUG
            printf("%d\n\r",(int)clock());
            #endif // DEBUG
            // handle commands
            switch(cmd)
            {
                case 'q':
                    run = false;
                    break;

                case 't':
                    // Obtener Temperatura
                    SolicitaSensor();
                    break;

                case ' ':
                    ShowMenu(comPort);
                    break;

                #ifdef DEBUG
                case 'f':
                    printf("CLOCKS_PER_SEC=%d\n\r",(int)CLOCKS_PER_SEC);
                #endif // DEBUG
            }
        }
    }
    return 0;
}

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
    printf("[t]\t: get time from module\n\r");
    printf("[q]\t: exit program\n\r");
    #ifdef DEBUG
    printf("[f]\t: DEBUG - Get CLOCKS_PER_SEC value\n\r");
    #endif // DEBUG
}
//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
