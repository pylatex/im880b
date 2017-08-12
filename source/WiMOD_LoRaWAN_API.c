//------------------------------------------------------------------------------
//
//	File:		WiMOD_LoRaWAN_API.cpp
//	Abstract:	API Layer of LoRaWAN Host Controller Interface
//	Version:	0.1
//	Date:		18.05.2016
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include Files
//------------------------------------------------------------------------------

#include "WiMOD_LoRaWAN_API.h"
#include "WiMOD_HCI_Layer.h"
#include <string.h>
#include <stdio.h>

#define MAKEWORD(lo,hi) ((lo)|((hi) << 8))
#define MAKELONG(lo,hi) ((lo)|((hi) << 16))

//------------------------------------------------------------------------------
//  Forward Declarations
//------------------------------------------------------------------------------

// HCI Message Receiver callback
static TWiMOD_HCI_Message*
WiMOD_LoRaWAN_Process_RxMessage(TWiMOD_HCI_Message*  rxMessage);

static void
WiMOD_LoRaWAN_Process_DevMgmt_Message(TWiMOD_HCI_Message*  rxMessage);

static void
WiMOD_LoRaWAN_DevMgmt_FirmwareVersion_Rsp(TWiMOD_HCI_Message*  rxMessage);

static void
WiMOD_LoRaWAN_Process_LoRaWAN_Message(TWiMOD_HCI_Message*  rxMessage);

static void
WiMOD_LoRaWAN_Process_JoinTxIndication(TWiMOD_HCI_Message* rxMessage);

static void
WiMOD_LoRaWAN_Process_JoinNetworkIndication(TWiMOD_HCI_Message* rxMessage);

static void
WiMOD_LoRaWAN_Process_U_DataRxIndication(TWiMOD_HCI_Message* rxMessage);

static void
WiMOD_LoRaWAN_Process_C_DataRxIndication(TWiMOD_HCI_Message* rxMessage);

static void
WiMOD_LoRaWAN_ShowResponse(const char* string, const TIDString* statusTable, UINT8 statusID);

//------------------------------------------------------------------------------
//  Section RAM
//------------------------------------------------------------------------------

// reserve one Tx-Message
TWiMOD_HCI_Message TxMessage;

// reserve one Rx-Message
TWiMOD_HCI_Message RxMessage;

//------------------------------------------------------------------------------
//  Section Const
//------------------------------------------------------------------------------

/**
 * Status Codes for DeviceMgmt Response Messages
 */
static const TIDString WiMOD_DeviceMgmt_StatusStrings[] =
{
    { DEVMGMT_STATUS_OK,                   "ok" },
    { DEVMGMT_STATUS_ERROR,                "error" } ,
    { DEVMGMT_STATUS_CMD_NOT_SUPPORTED,    "command not supported" },
    { DEVMGMT_STATUS_WRONG_PARAMETER,      "wrong parameter" },
    { DEVMGMT_STATUS_WRONG_DEVICE_MODE,    "wrong device mode" },

    // end of table
    { 0, 0 }
};

/**
 * Status Codes for LoRaWAN Response Messages
 */
static const TIDString WiMOD_LoRaWAN_StatusStrings[] =
{
    { LORAWAN_STATUS_OK,                    "ok" },
    { LORAWAN_STATUS_ERROR,                 "error" } ,
    { LORAWAN_STATUS_CMD_NOT_SUPPORTED,     "command not supported" },
    { LORAWAN_STATUS_WRONG_PARAMETER,       "wrong parameter" },
    { LORAWAN_STATUS_WRONG_DEVICE_MODE,     "wrong device mode" },
    { LORAWAN_STATUS_NOT_ACTIVATED,         "device not activated" },
    { LORAWAN_STATUS_BUSY,                  "device busy - command rejected" },
    { LORAWAN_STATUS_QUEUE_FULL,            "message queue full - command rejected" },
    { LORAWAN_STATUS_LENGTH_ERROR,          "HCI message length error" },
    { LORAWAN_STATUS_NO_FACTORY_SETTINGS,   "no factory settings available" },
    { LORAWAN_STATUS_CHANNEL_BLOCKED_BY_DC, "error: channel blocked due to duty cycle, please try later again" },
    { LORAWAN_STATUS_CHANNEL_NOT_AVAILABLE, "error: channel not available" },

    // end of table
    { 0, 0 }
};
//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

/**
 * Init
 * @brief: init complete interface
 */
bool
WiMOD_LoRaWAN_Init(const char* comPort)
{
    // init HCI layer
    return WiMOD_HCI_Init(comPort,                  // comPort
                   WiMOD_LoRaWAN_Process_RxMessage, // receiver callback
                   &RxMessage);                     // rx message
}

/**
 * Ping
 * @brief: ping device
 */
int
WiMOD_LoRaWAN_SendPing()
{
    // 1. init header
    TxMessage.SapID     = DEVMGMT_SAP_ID;
    TxMessage.MsgID     = DEVMGMT_MSG_PING_REQ;
    TxMessage.Length    = 0;

    // 2. send HCI message without payload
    return WiMOD_HCI_SendMessage(&TxMessage);
}

/**
 * GetFirmwareVersion
 * @brief: get firmware version
 */
int
WiMOD_LoRaWAN_GetFirmwareVersion()
{
    // 1. init header
    TxMessage.SapID     = DEVMGMT_SAP_ID;
    TxMessage.MsgID     = DEVMGMT_MSG_GET_FW_VERSION_REQ;
    TxMessage.Length    = 0;

    // 2. send HCI message without payload
    return WiMOD_HCI_SendMessage(&TxMessage);
}

/**
 * JoinNetworkRequest
 * @brief: send join radio message
 */
int
WiMOD_LoRaWAN_JoinNetworkRequest()
{
    // 1. init header
    TxMessage.SapID     = LORAWAN_SAP_ID;
    TxMessage.MsgID     = LORAWAN_MSG_JOIN_NETWORK_REQ;
    TxMessage.Length    = 0;

    // 2. send HCI message with payload
    return WiMOD_HCI_SendMessage(&TxMessage);
}

/**
 * SendURadioData
 * @brief: send unconfirmed radio message
 */
int
WiMOD_LoRaWAN_SendURadioData(UINT8  port,       // LoRaWAN Port
                             UINT8* srcData,    // application payload
                             int    srcLength)  // length of application payload
{
    // 1. check length
    if (srcLength > (WIMOD_HCI_MSG_PAYLOAD_SIZE - 1))
    {
        // error
        return -1;
    }

    // 2. init header
    TxMessage.SapID     = LORAWAN_SAP_ID;
    TxMessage.MsgID     = LORAWAN_MSG_SEND_UDATA_REQ;
    TxMessage.Length    = 1 + srcLength;

    // 3.  init payload
    // 3.1 init port
    TxMessage.Payload[0] = port;

    // 3.2 init radio message payload
    memcpy(&TxMessage.Payload[1], srcData, srcLength);

    // 4. send HCI message with payload
    return WiMOD_HCI_SendMessage(&TxMessage);
}

/**
 * SendCRadioData
 * @brief: send confirmed radio message
 */
int
WiMOD_LoRaWAN_SendCRadioData(UINT8  port,       // LoRaWAN Port
                             UINT8* srcData,    // application data
                             int    srcLength)  // length of application data
{
    // 1. check length
    if (srcLength > (WIMOD_HCI_MSG_PAYLOAD_SIZE - 1))
    {
        // error
        return -1;
    }

    // 2. init header
    TxMessage.SapID     = LORAWAN_SAP_ID;
    TxMessage.MsgID     = LORAWAN_MSG_SEND_CDATA_REQ;
    TxMessage.Length    = 1 + srcLength;

    // 3.  init payload
    // 3.1 init port
    TxMessage.Payload[0] = port;

    // 3.2 init radio message payload
    memcpy(&TxMessage.Payload[1], srcData, srcLength);

    // 4. send HCI message with payload
    return WiMOD_HCI_SendMessage(&TxMessage);
}

/**
 * Process
 * @brief: handle receiver process
 */
void
WiMOD_LoRaWAN_Process()
{
    // call HCI process
    WiMOD_HCI_Process();
}

/**
 * Process
 * @brief: handle receiver process
 */
static TWiMOD_HCI_Message*
WiMOD_LoRaWAN_Process_RxMessage(TWiMOD_HCI_Message*  rxMessage)
{
    switch(rxMessage->SapID)
    {
        case DEVMGMT_SAP_ID:
            WiMOD_LoRaWAN_Process_DevMgmt_Message(rxMessage);
            break;


        case LORAWAN_SAP_ID:
            WiMOD_LoRaWAN_Process_LoRaWAN_Message(rxMessage);
            break;
    }
    return &RxMessage;
}

/**
 * Process_DevMgmt_Message
 * @brief: handle received Device Management SAP messages
 */
static void
WiMOD_LoRaWAN_Process_DevMgmt_Message(TWiMOD_HCI_Message*  rxMessage)
{
    switch(rxMessage->MsgID)
    {
        case    DEVMGMT_MSG_PING_RSP:
                WiMOD_LoRaWAN_ShowResponse("ping response", WiMOD_DeviceMgmt_StatusStrings, rxMessage->Payload[0]);
                break;

        case    DEVMGMT_MSG_GET_FW_VERSION_RSP:
                WiMOD_LoRaWAN_DevMgmt_FirmwareVersion_Rsp(rxMessage);
                break;

        default:
                printf("unhandled DeviceMgmt message received - MsgID : 0x%02X\n\r", (UINT8)rxMessage->MsgID);
                break;
    }
}

/**
 * WiMOD_LoRaWAN_DevMgmt_FirmwareVersion_Rsp
 * @brief: show firmware version
 */
static void
WiMOD_LoRaWAN_DevMgmt_FirmwareVersion_Rsp(TWiMOD_HCI_Message*  rxMessage)
{
    char help[80];

    WiMOD_LoRaWAN_ShowResponse("firmware version response", WiMOD_DeviceMgmt_StatusStrings, rxMessage->Payload[0]);

    if (rxMessage->Payload[0] == DEVMGMT_STATUS_OK)
    {
        printf("version: V%d.%d\n\r", (int)rxMessage->Payload[2], (int)rxMessage->Payload[1]);
        printf("build-count: %d\n\r", (int)MAKEWORD(rxMessage->Payload[3], rxMessage->Payload[4]));

        memcpy(help, &rxMessage->Payload[5], 10);
        help[10] = 0;
        printf("build-date: %s\n\r", help);

        // more information attached ?
        if (rxMessage->Length > 15)
        {
            // add string termination
            rxMessage->Payload[rxMessage->Length] = 0;
            printf("firmware-content: %s\n\r", &rxMessage->Payload[15]);
        }
    }
}

/**
 * Process_LoRaWAN_Message
 * @brief: handle received LoRaWAN SAP messages
 */
static void
WiMOD_LoRaWAN_Process_LoRaWAN_Message(TWiMOD_HCI_Message*  rxMessage)
{
    switch(rxMessage->MsgID)
    {
        case    LORAWAN_MSG_JOIN_NETWORK_RSP:
                WiMOD_LoRaWAN_ShowResponse("join network response", WiMOD_LoRaWAN_StatusStrings, rxMessage->Payload[0]);
                break;

        case    LORAWAN_MSG_SEND_UDATA_RSP:
                WiMOD_LoRaWAN_ShowResponse("send U-Data response", WiMOD_LoRaWAN_StatusStrings, rxMessage->Payload[0]);
                break;

        case    LORAWAN_MSG_SEND_CDATA_RSP:
                WiMOD_LoRaWAN_ShowResponse("send C-Data response", WiMOD_LoRaWAN_StatusStrings, rxMessage->Payload[0]);
                break;

        case    LORAWAN_MSG_JOIN_TRANSMIT_IND:
                WiMOD_LoRaWAN_Process_JoinTxIndication(rxMessage);
                break;

        case    LORAWAN_MSG_JOIN_NETWORK_IND:
                WiMOD_LoRaWAN_Process_JoinNetworkIndication(rxMessage);
                break;

        case    LORAWAN_MSG_RECV_UDATA_IND:
                WiMOD_LoRaWAN_Process_U_DataRxIndication(rxMessage);
                break;

        case    LORAWAN_MSG_RECV_CDATA_IND:
                WiMOD_LoRaWAN_Process_C_DataRxIndication(rxMessage);
                break;

        case    LORAWAN_MSG_RECV_NODATA_IND:
                printf("no data received indication\n\r");
                break;

        default:
                printf("unhandled LoRaWAN SAP message received - MsgID : 0x%02X\n\r", (UINT8)rxMessage->MsgID);
                break;
    }
}

/**
 * WiMOD_LoRaWAN_Process_JoinTxIndication
 * @brief: show join transmit indicaton
 */
static void
WiMOD_LoRaWAN_Process_JoinTxIndication(TWiMOD_HCI_Message* rxMessage)
{
    if (rxMessage->Payload[0] == 0)
    {
        printf("join tx event - Status : ok\n\r");
    }
    // channel info attached ?
    else if(rxMessage->Payload[0] == 1)
    {
        printf("join tx event:%d, ChnIdx:%d, DR:%d - Status : ok\n\r", (int)rxMessage->Payload[3], (int)rxMessage->Payload[1], (int)rxMessage->Payload[2]);
    }
    else
    {
        printf("join tx event - Status : error\n\r");
    }
}

/**
 * WiMOD_LoRaWAN_Process_JoinNetworkIndication
 * @brief: show join network indicaton
 */
void
WiMOD_LoRaWAN_Process_JoinNetworkIndication(TWiMOD_HCI_Message* rxMessage)
{
    if (rxMessage->Payload[0] == 0)
    {
        UINT32 address = MAKELONG(MAKEWORD(rxMessage->Payload[1],rxMessage->Payload[2]),
                                  MAKEWORD(rxMessage->Payload[3],rxMessage->Payload[4]));

        printf("join network accept event - DeviceAddress:0x%08X\n\r", address);
    }
    else if (rxMessage->Payload[0] == 1)
    {
        UINT32 address = MAKELONG(MAKEWORD(rxMessage->Payload[1],rxMessage->Payload[2]),
                                  MAKEWORD(rxMessage->Payload[3],rxMessage->Payload[4]));

        printf("join network accept event - DeviceAddress:0x%08X, ChnIdx:%d, DR:%d, RSSI:%d, SNR:%d, RxSlot:%d\n\r", address,
               (int)rxMessage->Payload[5], (int)rxMessage->Payload[6], (int)rxMessage->Payload[7],
               (int)rxMessage->Payload[8], (int)rxMessage->Payload[9]);
    }
    else
    {
        printf("join network timeout event\n\r");
    }
}

/**
 * WiMOD_LoRaWAN_Process_U_DataRxIndication
 * @brief: show received U-Data
 */
void
WiMOD_LoRaWAN_Process_U_DataRxIndication(TWiMOD_HCI_Message* rxMessage)
{
    int payloadSize = rxMessage->Length - 1;

    // rx channel info attached ?
    if (rxMessage->Payload[0] & 0x01)
        payloadSize -= 5;

    if (payloadSize >= 1)
    {
        printf("U-Data rx event: port:0x%02X\n\rapp-payload:", rxMessage->Payload[1]);
        for(int i = 1; i < payloadSize;)
            printf("%02X ", rxMessage->Payload[1+i]);
        printf("\n\r");
    }

    if (rxMessage->Payload[0] & 0x02)
        printf("ack for uplink packet:yes\n\r");
    else
        printf("ack for uplink packet:no\n\r");

    if (rxMessage->Payload[0] & 0x04)
        printf("frame pending:yes\n\r");
    else
        printf("frame pending:no\n\r");

    // rx channel info attached ?
    if (rxMessage->Payload[0] & 0x01)
    {
        UINT8* rxInfo = &rxMessage->Payload[1 + payloadSize];
        printf("ChnIdx:%d, DR:%d, RSSI:%d, SNR:%d, RxSlot:%d\n\r",
              (int)rxInfo[0], (int)rxInfo[1], (int)rxInfo[2],
              (int)rxInfo[3], (int)rxInfo[4]);
    }
}

/**
 * WiMOD_LoRaWAN_Process_C_DataRxIndication
 * @brief: show received C-Data
 */
void
WiMOD_LoRaWAN_Process_C_DataRxIndication(TWiMOD_HCI_Message* rxMessage)
{
    int payloadSize = rxMessage->Length - 1;

    // rx channel info attached ?
    if (rxMessage->Payload[0] & 0x01)
        payloadSize -= 5;

    if (payloadSize >= 1)
    {
        printf("C-Data rx event: port:0x%02X\n\rapp-payload:", rxMessage->Payload[1]);
        for(int i = 1; i < payloadSize;)
            printf("%02X ", rxMessage->Payload[1+i]);
        printf("\n\r");
    }

    if (rxMessage->Payload[0] & 0x02)
        printf("ack for uplink packet:yes\n\r");
    else
        printf("ack for uplink packet:no\n\r");

    if (rxMessage->Payload[0] & 0x04)
        printf("frame pending:yes\n\r");
    else
        printf("frame pending:no\n\r");

    // rx channel info attached ?
    if (rxMessage->Payload[0] & 0x01)
    {
        UINT8* rxInfo = &rxMessage->Payload[1 + payloadSize];
        printf("ChnIdx:%d, DR:%d, RSSI:%d, SNR:%d, RxSlot:%d\n\r",
              (int)rxInfo[0], (int)rxInfo[1], (int)rxInfo[2],
              (int)rxInfo[3], (int)rxInfo[4]);
    }
}

/**
 * WiMOD_LoRaWAN_ShowResponse
 * @brief: show response status as human readable string
 */
static void
WiMOD_LoRaWAN_ShowResponse(const char* string, const TIDString* statusTable, UINT8 statusID)
{
    while(statusTable->String)
    {
        if (statusTable->ID == statusID)
        {
            printf(string);
            printf(" - Status(0x%02X) : ", statusID);
            printf(statusTable->String);
            printf("\n\r");
            return;
        }

        statusTable++;
    }
}
//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
