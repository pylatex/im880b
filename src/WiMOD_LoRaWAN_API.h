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

#ifdef	__cplusplus
extern "C" {
#endif

    //--------------------------------------------------------------------------
    //  Include Files
    //--------------------------------------------------------------------------

    #include <stdint.h>
    #include <stdbool.h>
    #include "WMLW_API.h"

    //--------------------------------------------------------------------------
    //  General Declarations
    //--------------------------------------------------------------------------

    typedef uint8_t     UINT8;
    typedef uint16_t    UINT16;
    typedef uint32_t    UINT32;

    // helper struct for ID -> string conversion
    typedef struct
    {
        int         ID;
        const char *String;
    }TIDString;

    //--------------------------------------------------------------------------
    //  Function Prototypes
    //--------------------------------------------------------------------------

    /**
     * Init
     * @brief: init complete interface
     */
    bool
    WiMOD_LoRaWAN_Init(const char *comPort);

    /**
     * Ping
     * @brief: ping device
     */
    int
    WiMOD_LoRaWAN_SendPing();

    /**
     * GetFirmwareVersion
     * @brief: get firmware version
     */
    int
    WiMOD_LoRaWAN_GetFirmwareVersion();

    /**
     * JoinNetworkRequest
     * @brief: send join radio message
     */
    int
    WiMOD_LoRaWAN_JoinNetworkRequest();

    /**
     * SendURadioData
     * @brief: send unconfirmed radio message
     */
    int
    WiMOD_LoRaWAN_SendURadioData(UINT8 port, UINT8 *data, UINT8 length);

    /**
     * SendCRadioData
     * @brief: send confirmed radio message
     */
    int
    WiMOD_LoRaWAN_SendCRadioData(UINT8 port, UINT8 *data, UINT8 length);

    /**
     * Process
     * @brief: handle receiver process
     */
    void
    WiMOD_LoRaWAN_Process();

    /**
     * Request the time from RTC on module
     */
    int
    WiMOD_LoRaWAN_GetTime();

    /**
     * Request the status of the device on the LoRaWAN Network
     */
    int
    WiMOD_LoRaWAN_GetNetworkStatus();

#ifdef	__cplusplus
}
#endif

#endif // WIMOD_LORAWAN_API_H

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
