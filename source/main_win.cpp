/*
 * File:		main.cpp
 * Abstract:	main module
 * Version:	0.1
 * Date:		18.05.2016
 * Disclaimer:	This example code is provided by IMST GmbH on an "AS IS"
 *              basis without any warranties.
 *
 * La mayoria de archivos vienen del comprimido
 * WiMOD_LoRaWAN_ExampleCode_HCI_C_V0_1.zip
 * y han sido ligeramente modificados para soportar tipos estandar.
 */
#define DEBUG
//------------------------------------------------------------------------------
//  Definitions and Setup
//------------------------------------------------------------------------------
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "WiMOD_LoRaWAN_API.h"
#ifdef DEBUG
#include <time.h>
#endif // DEBUG
//------------------------------------------------------------------------------
//  Declarations, Definitions and Variables
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
    if(!WiMOD_LoRaWAN_Init(comPort))
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
        WiMOD_LoRaWAN_Process();

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
                    break;

                case ' ': //Print the menu
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
    printf("[p]\t: ping device\n\r");
    printf("[i]\t: get device information\n\r");
    printf("[j]\t: join network request\n\r");
    printf("[u]\t: send unconfirmed radio message\n\r");
    printf("[c]\t: send confirmed radio message\n\r");
    printf("[t]\t: get time from module\n\r");
    printf("[n]\t: get network status\n\r");
    printf("[q]\t: exit program\n\r");
    #ifdef DEBUG
    printf("[f]\t: DEBUG - Get CLOCKS_PER_SEC value\n\r");
    #endif // DEBUG
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

    const UINT8 port = 0x21;
    const UINT8 data[]={0x01,0x02,0x03,0x04};

    // send unconfirmed radio message
    WiMOD_LoRaWAN_SendURadioData(port, &data, 4);
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
    WiMOD_LoRaWAN_SendCRadioData(port, data, 6);
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
//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
