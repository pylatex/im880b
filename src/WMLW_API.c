/**
 * Algunas funciones que se pueden hacer sobre la API para los modulos
 * WiMOD LoRaWAN de IMST
 */

//------------------------------------------------------------------------------
// INCLUDES AND DEFINITIONS
//------------------------------------------------------------------------------
#include <string.h>
#include "WiMOD_LoRaWAN_API.h"
#include "hci_stack.h"

static HCIMessage_t TxMessage;

//------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FUNCTION IMPLEMENTATIONS
//------------------------------------------------------------------------------

/**
 * Ping
 * @brief: ping device
 */
int
WiMOD_LoRaWAN_SendPing(){
    //Message Setup
    TxMessage.SapID = DEVMGMT_ID;
    TxMessage.MsgID = DEVMGMT_MSG_PING_REQ;
    TxMessage.size = 0;
    //Send Message
    return SendHCI(&TxMessage);
}

/**
 * Request the status of the device on the LoRaWAN Network
 */
int
WiMOD_LoRaWAN_GetNetworkStatus() {
    //Message Setup
    TxMessage.SapID = LORAWAN_ID;
    TxMessage.MsgID = LORAWAN_MSG_GET_NWK_STATUS_REQ;
    TxMessage.size = 0;
    //Send Message
    return SendHCI(&TxMessage);
}

/**
 * GetFirmwareVersion
 * @brief: get firmware version
 */
int
WiMOD_LoRaWAN_GetFirmwareVersion(){
}

/**
 * JoinNetworkRequest
 * @brief: send join radio message
 */
int
WiMOD_LoRaWAN_JoinNetworkRequest(){
    //Message Setup
    TxMessage.SapID = LORAWAN_ID;
    TxMessage.MsgID = LORAWAN_MSG_JOIN_NETWORK_REQ;
    TxMessage.size = 0;
    //Send Message
    return SendHCI(&TxMessage);
}

/**
 * SendURadioData
 * @brief: send unconfirmed radio message
 */
int
WiMOD_LoRaWAN_SendURadioData(UINT8 port, UINT8 *data, UINT8 length){
    //Message Setup
    TxMessage.SapID=LORAWAN_ID;
    TxMessage.MsgID=LORAWAN_MSG_SEND_UDATA_REQ;
    TxMessage.Payload[0]=port; //Puerto LoRaWAN
    TxMessage.size = (unsigned)(length+1);
    memcpy(&TxMessage.Payload[1], data, length);
    //Send Message
    return SendHCI(&TxMessage);
}

/**
 * SendCRadioData
 * @brief: send confirmed radio message
 */
int
WiMOD_LoRaWAN_SendCRadioData(UINT8 port, UINT8* data, UINT8 length){
    //Message Setup
    TxMessage.SapID=LORAWAN_ID;
    TxMessage.MsgID=LORAWAN_MSG_SEND_CDATA_REQ;
    TxMessage.Payload[0]=port; //Puerto LoRaWAN
    TxMessage.size = (unsigned)(length+1);
    memcpy(&TxMessage.Payload[1], data, length);
    //Send Message
    return SendHCI(&TxMessage);
}

/**
 * Process
 * @brief: handle receiver process
 */
void
WiMOD_LoRaWAN_Process(){
}