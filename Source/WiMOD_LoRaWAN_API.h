//------------------------------------------------------------------------------
//
//	File:		WiMOD_LoRaWAN_API.h
//	Abstract:	API Layer of LoRaWAN Host Controller Interface
//	Version:	0.1
//	Date:		18.05.2016
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS"
//              basis without any warranties.
//
//------------------------------------------------------------------------------

#ifndef WIMOD_LORAWAN_API_H
#define WIMOD_LORAWAN_API_H

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------

#include <stdint.h>

//------------------------------------------------------------------------------
//  General Declarations
//------------------------------------------------------------------------------

typedef uint8_t     UINT8;
typedef uint16_t    UINT16;
typedef uint32_t    UINT32;


// helper struct for ID -> string conversion
typedef struct
{
    int         ID;
    const char* String;
}TIDString;


//------------------------------------------------------------------------------
//  Function Prototypes
//------------------------------------------------------------------------------

// init
bool
WiMOD_LoRaWAN_Init(const char* comPort);

// ping device
int
WiMOD_LoRaWAN_SendPing();

// get firmware Version
int
WiMOD_LoRaWAN_GetFirmwareVersion();

// join network
int
WiMOD_LoRaWAN_JoinNetworkRequest();

// send unconfirmed radio data
int
WiMOD_LoRaWAN_SendURadioData(UINT8 port, UINT8* data, int length);

// send confirmed radio data
int
WiMOD_LoRaWAN_SendCRadioData(UINT8 port, UINT8* data, int length);

// receiver process
void
WiMOD_LoRaWAN_Process();

#endif // WIMOD_LORAWAN_API_H

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
