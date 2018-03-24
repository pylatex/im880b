/*
 * Implementacion de la aplicacion propia para LoRaWAN
 * 
 * Author:  Alex F. Bustos P.
 * Created: Feb 7, 2018
 */

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------

#include "WiMOD_LoRaWAN_API.h"
#include "hci_stack.h"
#include "pylatex.h"

//------------------------------------------------------------------------------
//  Definitions, Declarations and Variables
//------------------------------------------------------------------------------

#define LARGO   20
#define PUERTO  5

typedef enum {
    WaitingUART,
    WaitingNetStat,
    WaitingActivation,
    NWKinactive,
    NWKactive,
    NWKjoining
} status_t;

typedef struct {
    char                    carga[LARGO];
    char                    cnt;
    delayHandlerFunction    delfun;
    volatile bool          *tmrrun;
    volatile status_t       status;
} PY_T;

#define initAppPayload() PY.cnt=0

static PY_T PY;
volatile unsigned char timeouts;
volatile static HCIMessage_t RxMessage;
static void ProcesaHCI(); //Procesamiento de HCI entrante

//------------------------------------------------------------------------------
//  Function Implementations
//------------------------------------------------------------------------------

void initLoraApp (void) {
    PY.delfun=0;    //No delay function at startup
    initAppPayload();
    InitHCI(ProcesaHCI,(HCIMessage_t *) &RxMessage);
    PY.status = WaitingUART;    //Initial State
    WiMOD_LoRaWAN_SendPing();
    while (PY.status == WaitingUART);      //Espera hasta que haya respuesta del iM880
    for (;;){
        WiMOD_LoRaWAN_GetNetworkStatus();
        while (PY.status == WaitingNetStat);
        if (PY.status == NWKactive) break;
        if (PY.status == NWKinactive) WiMOD_LoRaWAN_JoinNetworkRequest();
        while (PY.status == NWKinactive || PY.status == NWKjoining);
    }
}

void registerDelayFunction(delayHandlerFunction delfun,volatile bool *flag) {
    PY.delfun=delfun;
    PY.tmrrun=flag;
}

bool AppendMeasure (char variable,char *medida) {
    if (medida) {
        switch (variable) {
            case PY_CO2:
            case PY_TVOC:
            case PY_PM025:
            case PY_PM100:
            case PY_GAS:
                //FORMATO A
                if (PY.cnt < (LARGO - 3)) {
                    PY.carga[PY.cnt++]=variable;   //Identificador
                    PY.carga[PY.cnt++]=medida[0];  //MSB
                    PY.carga[PY.cnt++]=medida[1];  //LSB
                    return true;
                }
                break;
        }
    }
    return false;
}

void SendMeasures (bool confirmed) {
    if (PY.cnt) {
        if (confirmed)
            WiMOD_LoRaWAN_SendCRadioData(5, PY.carga, PY.cnt);
        else
            WiMOD_LoRaWAN_SendURadioData(5, PY.carga, PY.cnt);
        initAppPayload();
    }
}

char *short2charp (unsigned short in) {
    static char val[2];
    val[0]=in>>8    & 0xFF;
    val[1]=in       & 0xFF;
    return val;
}

void pylatexRx (char RxByteUART) {
    IncomingHCIpacker(RxByteUART);
}

/**
 * Handler for (pre)processing of an incoming HCI message. Once the user exits
 * from this handler function, the RxMessage.size variable gets cleared!
 */
static void ProcesaHCI() {
    if (RxMessage.check) {
        switch (PY.status) {

            case WaitingUART:
                //Basta con cualquier HCI entrante.
                PY.status = WaitingNetStat;
                break;

            case WaitingNetStat:
                if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_GET_NWK_STATUS_RSP)) {
                    //The incoming HCI message is a response of NWK STATUS
                    switch (RxMessage.Payload[1]) {
                        case 0: //Inactive
                        case 1: //Active (ABP)
                            PY.status = NWKinactive;
                            break;
                        case 3: //Accediendo (OTAA)
                            PY.status = NWKjoining;
                            break;
                        case 2: //Active (OTAA)
                            PY.status = NWKactive;
                        default:
                            break;
                    }
                }
                break;

            case NWKinactive:
                if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_JOIN_NETWORK_RSP)) {
                    PY.status = NWKjoining;
                }
                break;

            case WaitingActivation:
                if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_JOIN_NETWORK_IND)) {
                    //The incoming HCI message is a join event
                    switch (RxMessage.Payload[0]) {
                        case 0x00:  //device successfully activated
                        case 0x01:  //device successfully activated, Rx Channel Info attached
                            PY.status = NWKactive;
                        default:
                            break;
                    }
                }
                PY.status = WaitingActivation;
                break;

            case NWKactive:
                break;
        }
    }
}
