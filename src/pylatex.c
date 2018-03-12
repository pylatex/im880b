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
    NWKjoining,
    RESET,
    TestUART,
    GetNwkStatus,
    NWKaccept,
    SENSprocess
} status_t;

static volatile status_t status = WaitingUART; //Initial State for the machine of Main.

typedef struct {
    char                    carga[LARGO];
    char                    cnt;
    delayHandlerFunction    delfun;
    volatile bool          *tmrrun;
} PY_T;

#define initAppPayload() PY.cnt=0

static PY_T PY;
volatile unsigned bool pendingmsg;
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
    WiMOD_LoRaWAN_SendPing();
    while (status == WaitingUART);      //Espera hasta que haya respuesta del iM880
    WiMOD_LoRaWAN_GetNetworkStatus();
    while (status == WaitingNetStat);   //Espera a obtener el estado de la red
    if (status == NWKinactive) {
        WiMOD_LoRaWAN_JoinNetworkRequest();
        while (status == NWKinactive);
    }
    while (status == WaitingActivation);
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

void maquina () {
    switch (status) {
        case RESET:
            pendingmsg = false;
            timeouts = 0;
            if (InitHCI(ProcesaHCI,(HCIMessage_t *) &RxMessage))
                status = TestUART; //Full Duplex UART and Rx interruptions enabled
            break;
        case TestUART:
            WiMOD_LoRaWAN_SendPing();
            //Wait for TimeOut or HCI message and identify the situation:
            if (PY.delfun) PY.delfun(20);
            while (*PY.tmrrun && !pendingmsg); //Escapes at timeout or HCI received.
            if (pendingmsg) {
                timeouts=0;
                if ((RxMessage.SapID == DEVMGMT_ID) && (RxMessage.MsgID == DEVMGMT_MSG_PING_RSP)) {
                    //The incoming HCI message is a response of PING
                    status = GetNwkStatus;
                    pendingmsg = false;
                }
            }
            break;
        case GetNwkStatus:
            WiMOD_LoRaWAN_GetNetworkStatus();
            //Wait for TimeOut or HCI message and identify the situation:
            if (PY.delfun) PY.delfun(20);
            while (*PY.tmrrun && !pendingmsg); //Escapes at timeout or HCI received.
            if (pendingmsg) {
                timeouts=0;
                if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_GET_NWK_STATUS_RSP)) {
                    pendingmsg = false;
                    //The incoming HCI message is a response of NWK STATUS
                    switch (RxMessage.Payload[1]) {
                        case 0: //Inactive
                        case 1: //Active (ABP)
                            status = NWKinactive;
                            break;
                        case 2: //Active (OTAA)
                            status = NWKactive;
                            break;
                        case 3: //Accediendo (OTAA)
                            status = NWKjoining;
                            break;
                        default:
                            break;
                    }
                }
            }
            break;
        case NWKinactive:
            WiMOD_LoRaWAN_JoinNetworkRequest();
            //Wait for TimeOut or HCI message and identify the situation:
            if (PY.delfun) PY.delfun(20);
            while (*PY.tmrrun && !pendingmsg); //Escapes at timeout or HCI received.
            if (pendingmsg) {
                timeouts=0;
                if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_JOIN_NETWORK_RSP)) {
                    //The incoming HCI message is a response of NWK STATUS
                    if (RxMessage.Payload[0] == LORAWAN_STATUS_OK) {
                        pendingmsg = false;
                        status = NWKjoining;
                    }
                }
            }
            break;
        case NWKjoining:
            //Wait for TimeOut or HCI message and identify the situation:
            if (PY.delfun) PY.delfun(20);
            while (*PY.tmrrun && !pendingmsg); //Escapes at timeout or HCI received.
            if (pendingmsg) {
                timeouts=0;
                if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_JOIN_NETWORK_IND)) {
                    //The incoming HCI message is a join event
                    switch (RxMessage.Payload[0]) {
                        case 0x00:  //device successfully activated
                        case 0x01:  //device successfully activated, Rx Channel Info attached
                            status = NWKaccept;
                        default:
                            break;
                    }
                }
            }
            break;
        case NWKaccept:
            //Wait for TimeOut or HCI message and identify the situation:
            if (PY.delfun) PY.delfun(20);
            while (*PY.tmrrun && !pendingmsg); //Escapes at timeout or HCI received.
            if (pendingmsg) {
                timeouts=0;
                if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_RECV_NO_DATA_IND)) {
                    //The incoming HCI message is an indication of reception with no data
                    if (RxMessage.Payload[0] == LORAWAN_STATUS_OK) {
                        pendingmsg = false;
                        status = NWKactive;
                    }
                }
            }
            break;
        case NWKactive:
            //Create more states to attend other HCI messages or modify this state
            //ms100(49);//Espera ~5 segundos
            status = SENSprocess;
            break;
        case SENSprocess:
            /*
            LED=true;
            //Starts an I2C reading and decides upon the response.
            respuesta=T67xx_C02();
            if (respuesta)
                AppendMeasure(PY_CO2,respuesta);
            AppendMeasure(PY_GAS,short2charp(valorPropano()));
            SendMeasures();
            ms100(1);   //Completa los 5 segundos...
            LED=false;
            */
            status = NWKactive;
            break;
        default:
            break;
    }
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
        switch (status) {

            case WaitingUART:
                //Basta con cualquier HCI entrante.
                status = WaitingNetStat;
                break;

            case WaitingNetStat:
                if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_GET_NWK_STATUS_RSP)) {
                    //The incoming HCI message is a response of NWK STATUS
                    switch (RxMessage.Payload[1]) {
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
                if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_JOIN_NETWORK_RSP)) {
                    status = WaitingActivation;
                }
                break;

            case WaitingActivation:
                if ((RxMessage.SapID == LORAWAN_ID) && (RxMessage.MsgID == LORAWAN_MSG_JOIN_NETWORK_IND)) {
                    //The incoming HCI message is a join event
                    switch (RxMessage.Payload[0]) {
                        case 0x00:  //device successfully activated
                        case 0x01:  //device successfully activated, Rx Channel Info attached
                            status = NWKaccept;
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