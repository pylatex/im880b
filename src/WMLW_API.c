/**
 * Algunas funciones que se pueden hacer sobre la API para los modulos
 * WiMOD LoRaWAN de IMST
 */

//------------------------------------------------------------------------------
// INCLUDES AND DEFINITIONS
//------------------------------------------------------------------------------
#include <string.h>
#include "WiMOD_LoRaWAN_API.h"

typedef enum {
    WaitingUART,
    WaitingNetStat,
    WaitingActivation,
    NWKinactive,
    NWKactive,
    NWKjoining
} status_t;

static HCIMessage_t TxMessage;
extern volatile HCIMessage_t    HCIrxMessage;
static void ProcesaHCI(); //Procesamiento de HCI entrante
volatile status_t   status;

//------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FUNCTION IMPLEMENTATIONS
//------------------------------------------------------------------------------

bool WiMOD_LoRaWAN_Init (serialTransmitHandler transmitter) {
    InitHCI(ProcesaHCI,transmitter);
    status = WaitingUART;    //Initial State
    WiMOD_LoRaWAN_SendPing();
    while (status == WaitingUART);      //Espera hasta que haya respuesta del iM880
    for (;;){
        WiMOD_LoRaWAN_GetNetworkStatus();
        while (status == WaitingNetStat);
        if (status == NWKactive) return true;
        if (status == NWKinactive) WiMOD_LoRaWAN_JoinNetworkRequest();
        while (status == NWKinactive || status == NWKjoining);
    }
    return false;
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
 * from this handler function, the RxMessage.size variable gets cleared!
 */
static void ProcesaHCI() {
    if (HCIrxMessage.check) {
        switch (status) {

            case WaitingUART:
                //Basta con cualquier HCI entrante.
                status = WaitingNetStat;
                break;

            case WaitingNetStat:
                if ((HCIrxMessage.SapID == LORAWAN_ID) && (HCIrxMessage.MsgID == LORAWAN_MSG_GET_NWK_STATUS_RSP)) {
                    //The incoming HCI message is a response of NWK STATUS
                    switch (HCIrxMessage.Payload[1]) {
                        case 0: //Inactive
                        case 1: //Active (ABP)
                            status = NWKinactive;
                            break;
                        case 3: //Accediendo (OTAA)
                            status = NWKjoining;
                            break;
                        case 2: //Active (OTAA)
                            status = NWKactive;
                        default:
                            break;
                    }
                }
                break;

            case NWKinactive:
                if ((HCIrxMessage.SapID == LORAWAN_ID) && (HCIrxMessage.MsgID == LORAWAN_MSG_JOIN_NETWORK_RSP)) {
                    status = NWKjoining;
                }
                break;

            case WaitingActivation:
                if ((HCIrxMessage.SapID == LORAWAN_ID) && (HCIrxMessage.MsgID == LORAWAN_MSG_JOIN_NETWORK_IND)) {
                    //The incoming HCI message is a join event
                    switch (HCIrxMessage.Payload[0]) {
                        case 0x00:  //device successfully activated
                        case 0x01:  //device successfully activated, Rx Channel Info attached
                            status = NWKactive;
                        default:
                            break;
                    }
                }
                status = WaitingActivation;
                break;

            case NWKactive:
                break;
        }
    }
}

//Receiver Process
void
WiMOD_LoRaWAN_Process() {
}
