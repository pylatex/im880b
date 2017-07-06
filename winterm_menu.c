/**
 * Opciones de menu principales, tomadas del main original en C
 */
#include "globaldefs.h"

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