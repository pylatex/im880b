/*------------------------------------------------------------------------------
  File:       main.cpp
  Abstract:   main module
  Version:    0.1
  Date:       18.05.2016
  Disclaimer: This example code is provided by IMST GmbH on an "AS IS"
              basis without any warranties.
------------------------------------------------------------------------------*/
#define DEBUG

//#define HPM     //Descomentar para hacer pruebas con el sensor HPM directamente
//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
#ifdef  Q_OS_WIN
#include <conio.h>
#endif  //Q_OS_WIN

#ifdef Q_OS_UX
#include <iostream>
#include <sys/select.h>

int kbhit(void)
{
    struct timeval tv;
    fd_set read_fd;

    // Do not wait at all, not even a microsecond
    tv.tv_sec=0;
    tv.tv_usec=0;

    // Must be done first to initialize read_fd
    FD_ZERO(&read_fd);

    // Makes select() ask if input is ready:
    // 0 is the file descriptor for stdin
    FD_SET(0,&read_fd);

    // The first parameter is the number of the
    // largest file descriptor to check + 1.
    if(select(1, &read_fd,NULL, /*No writes*/NULL, /*No exceptions*/&tv) == -1)
    return 0; //An error occured

    // read_fd now holds a bit map of files that are
    // readable. We test the entry for the standard
    // input (file 0).

    if(FD_ISSET(0,&read_fd))
    // Character pending on stdin
    return 1;

    // no characters were pending
    return 0;
}

#define getch() std::cin.get()

#endif  //Q_OS_UX

#include <stdio.h>
#include <string.h>
#ifdef HPM
#include "SerialDevice.h"
#include "hpm.h"
#else
#include "WiMOD_LoRaWAN_API.h"
#endif // HPM
#ifdef DEBUG
#include <time.h>
#endif // DEBUG
//------------------------------------------------------------------------------
//  Declarations, Definitions and Variables
//------------------------------------------------------------------------------

// forward declarations
static void     ShowMenu(const char*);
#ifndef HPM
void            Ping();
void            GetDeviceInfo();
void            Join();
void            SendUData();
void            SendCData();
static void     GetTime();
static void     GetLWstatus();
#endif // HPM

#define DEF_PORT "COM5"  //Default port, in case of empty input

//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

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
    #ifdef HPM
    if(!SerialDevice_Open(comPort,9600,8,0))
    #else
    if(!WiMOD_LoRaWAN_Init(comPort))
    #endif // HPM
    {
        printf("error - couldn't open interface on comport %s\r\n",comPort);
        printf("try: WiMOD_LoRaWAN_HCI_C_ExampleCode COMxy to select another comport\n\r");

        return -1;
    }

    // show menu
    ShowMenu(comPort);

    #ifdef HPM
    InicializacionHPM(SerialDevice_SendData);
    #endif // HPM

    #define MAXIMO_SENSOR 32

    // main loop
    while(run) {
        // handle receiver process
        #ifdef HPM
        unsigned char rxBuf[MAXIMO_SENSOR];
        // read small chunk of data
        int rxLength = SerialDevice_ReadData(rxBuf, MAXIMO_SENSOR);
        if (rxLength) { // data available ?
            RespuestaSensor(rxBuf,MAXIMO_SENSOR);
        }
        #else
        WiMOD_LoRaWAN_Process();
        #endif // HPM

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
                case 'q': //exit
                    run = false;
                    break;
#ifndef HPM
                case 'i': // get device info
                    GetDeviceInfo();
                    break;

                case 'p': // ping device
                    Ping();
                    break;

                case 'j': // join network
                    Join();
                    break;

                case 'u': // send u-data
                    SendUData();
                    break;

                case 'c': // send c-data
                    SendCData();
                    break;

                case 't': // get time from module
                    GetTime();
                    break;

                case 'n': // get network status
                    GetLWstatus();
#else
                case 'm': // Obtener medida
                    SolicitarMedida();
                    break;

                case 'b': // inciar medición
                    InciarMedicion();
                    break;

                case 's': // parar medición
                    PararMedicion();
                    break;
#endif // HPM
                case ' ': //Print the menu
                    ShowMenu(comPort);
                    break;

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
    printf("+----------------------------------------+\n\r");
    printf("|Using comport: %24s |\r\n", comPort);
    printf("+----------------------------------------+\n\r");
    printf("[SPACE]\t: show this menu\n\r");
    #ifndef HPM
    printf("[p]\t: ping device\n\r");
    printf("[i]\t: get device information\n\r");
    printf("[j]\t: join network request\n\r");
    printf("[u]\t: send unconfirmed radio message\n\r");
    printf("[c]\t: send confirmed radio message\n\r");
    printf("[t]\t: get time from module\n\r");
    printf("[n]\t: get network status\n\r");
    #else
    printf("[m]\t: Get measures from module\n\r");
    printf("[b]\t: Begin module measuring\n\r");
    printf("[s]\t: Stop module measuring\n\r");
    #endif // HPM
    printf("[q]\t: exit program\n\r");
    #ifdef DEBUG
    printf("CLOCKS_PER_SEC=%d\n\r",(int)CLOCKS_PER_SEC);
    #endif // DEBUG
}

#ifndef HPM
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
    const UINT8 port = 0x21;
    const UINT8 data[]={0x01,0x02,0x03,0x04};
    // send unconfirmed radio message
    WiMOD_LoRaWAN_SendURadioData(port,(UINT8 *) data, 4);
}

/**
 * SendCData
 * @brief: send confirmed radio message
 */
void SendCData() {
    printf("send C-Data\n\r");

    const UINT8 port = 0x23;
    const UINT8 data[]={0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
    // send confirmed radio message
    WiMOD_LoRaWAN_SendCRadioData(port,(UINT8 *) data, 6);
}

/**
 * GetTime
 * @brief: Prints the time as returned from iM880B RTC
 */
static void GetTime() {
    printf("get Time\n\r");
    WiMOD_LoRaWAN_GetTime();
}

/**
 * GetLWstatus
 * @brief: Get the LoRaWAN status of the iM88x
 */
static void     GetLWstatus() {
    printf("get LoRaWAN Network Status\n\r");
    WiMOD_LoRaWAN_GetNetworkStatus();
}
#endif // HPM
//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
