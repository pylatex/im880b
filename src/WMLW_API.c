/**
 * Algunas funciones que se pueden hacer sobre la API para los modulos
 * WiMOD LoRaWAN de IMST
 */

//------------------------------------------------------------------------------
// INCLUDES AND DEFINITIONS
//------------------------------------------------------------------------------
#include <string.h>
#include "WiMOD_LoRaWAN_API.h"

static HCIMessage_t TxMessage;
static void ProcesaHCI(HCIMessage_t *receivedHCI); //Procesamiento de HCI entrante
static LWstat *userStat;
static flag_t *catched;

//------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FUNCTION IMPLEMENTATIONS
//------------------------------------------------------------------------------

bool WiMOD_LoRaWAN_Init (serialTransmitHandler transmitter, LWstat *LWstatus) {
    InitHCI(ProcesaHCI,transmitter);
    userStat = LWstatus;
    *userStat = CONNECTING;
    return true;
}

void
WiMOD_LoRaWAN_nextRequest(flag_t *responseCatched) {
    catched = responseCatched;
    *catched = false;
    switch (*userStat) {

        case CONNECTING:
            WiMOD_LoRaWAN_GetNetworkStatus();
            break;

        case NET_IDLE:
            WiMOD_LoRaWAN_JoinNetworkRequest();
            break;

        default:
            break;
    }
}

// ping device
int
WiMOD_LoRaWAN_SendPing(){
    //Message Setup
    TxMessage.SapID = DEVMGMT_ID;
    TxMessage.MsgID = DEVMGMT_MSG_PING_REQ;
    TxMessage.size = 0;
    //Send Message
    return SendHCI(&TxMessage);
}

// get the LoRaWAN Network status of the device
int
WiMOD_LoRaWAN_GetNetworkStatus() {
    //Message Setup
    TxMessage.SapID = LORAWAN_ID;
    TxMessage.MsgID = LORAWAN_MSG_GET_NWK_STATUS_REQ;
    TxMessage.size = 0;
    //Send Message
    return SendHCI(&TxMessage);
}

// request to join network
int
WiMOD_LoRaWAN_JoinNetworkRequest(){
    //Message Setup
    TxMessage.SapID = LORAWAN_ID;
    TxMessage.MsgID = LORAWAN_MSG_JOIN_NETWORK_REQ;
    TxMessage.size = 0;
    //Send Message
    return SendHCI(&TxMessage);
}

// send unconfirmed radio data
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

// send confirmed radio data
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
 * Handler for (pre)processing of an incoming HCI message. Once the user exits
 * from this handler function, the *receivedHCI->size variable gets cleared!
 */
static void ProcesaHCI(HCIMessage_t *receivedHCI) {
    if (receivedHCI->check) {
        *catched = true;
        switch (*userStat) {

            case CONNECTING:
                if (receivedHCI->SapID == LORAWAN_ID) {
                    switch (receivedHCI->MsgID) {

                        case LORAWAN_MSG_GET_NWK_STATUS_RSP:
                            switch (receivedHCI->Payload[1]) {
                                //case 1: //Active (ABP)
                                case 2: //Active (OTAA)
                                    *userStat = ACTIVE;
                                default:
                                    break;
                            }
                            break;

                        case LORAWAN_MSG_JOIN_NETWORK_IND:
                            //The incoming HCI message is a join event
                            switch (receivedHCI->Payload[0]) {
                                case 0x00:  //device successfully activated
                                case 0x01:  //device successfully activated, Rx Channel Info attached
                                    *userStat = ACTIVE;
                                default:
                                    break;
                            }
                            break;

                        case LORAWAN_MSG_JOIN_NETWORK_RSP:
                            *userStat = CONNECTING;
                            break;

                        default:
                            break;
                    }
                }
                break;

            default:
                break;
        }
    }
}

//Receiver Process
void
WiMOD_LoRaWAN_Process() {
}
