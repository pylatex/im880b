/*------------------------------------------------------------------------------
  File:		main.cpp
  Abstract:	main module
	Version:	0.1
	Date:		18.05.2016
  Disclaimer:	This example code is provided by IMST GmbH on an "AS IS"
              basis without any warranties.

------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------

#include "main.h"   //Open and modify to change the target system (WIN/UX)
#include "WiMOD_LoRaWAN_API.h"
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
        strcpy(comPort, defPORT);
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
    printf(">> ");

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
            printf(">> ");
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
//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
