/**
 * Algunas funciones que se pueden hacer sobre la API para los modulos
 * WiMOD LoRaWAN de IMST
 */

//------------------------------------------------------------------------------
// INCLUDES AND DEFINITIONS
//------------------------------------------------------------------------------
#include "WiMOD_LoRaWAN_API.h"
#include "hci_stack.h"

//------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FUNCTION IMPLEMENTATIONS
//------------------------------------------------------------------------------

// ping device
int
WiMOD_LoRaWAN_SendPing(){
}

// get firmware Version
int
WiMOD_LoRaWAN_GetFirmwareVersion(){
}

// join network
int
WiMOD_LoRaWAN_JoinNetworkRequest(){
}

// send unconfirmed radio data
int
WiMOD_LoRaWAN_SendURadioData(UINT8 port, UINT8* data, int length){
}

// send confirmed radio data
int
WiMOD_LoRaWAN_SendCRadioData(UINT8 port, UINT8* data, int length){
}

// receiver process
void
WiMOD_LoRaWAN_Process(){
}