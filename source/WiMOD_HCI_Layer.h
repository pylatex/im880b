//------------------------------------------------------------------------------
//
//	File:		WiMOD_HCI_Layer.h
//	Abstract:	WiMOD HCI Message Layer
//	Version:	0.1
//	Date:		18.05.2016
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS"
//              basis without any warranties.
//
//------------------------------------------------------------------------------

#ifndef WIMOD_HCI_LAYER_H
#define WIMOD_HCI_LAYER_H

#ifdef	__cplusplus
extern "C" {
#endif
    //------------------------------------------------------------------------------
    //  Include Files
    //------------------------------------------------------------------------------

    #include <stdint.h>
    #include <stdbool.h>

    //------------------------------------------------------------------------------
    //  General Declarations & Definitions
    //------------------------------------------------------------------------------

    typedef unsigned char                   UINT8;
    typedef uint16_t                        UINT16;

    #define WIMOD_HCI_MSG_HEADER_SIZE       2
    #define WIMOD_HCI_MSG_PAYLOAD_SIZE      300
    #define WIMOD_HCI_MSG_FCS_SIZE          2

    #define LOBYTE(x)                       (x)
    #define HIBYTE(x)                       ((UINT16)(x) >> 8)

    #define WMLWbaudrate    115200

    //HCI Message Structure (internal software usage)
    typedef struct
    {
        // Payload Length Information,
        // this field not transmitted over UART interface !!!
        UINT16  Length;

        // Service Access Point Identifier
        UINT8   SapID;

        // Message Identifier
        UINT8   MsgID;

        // Payload Field
        UINT8   Payload[WIMOD_HCI_MSG_PAYLOAD_SIZE];

        // Frame Check Sequence Field
        UINT8   CRC16[WIMOD_HCI_MSG_FCS_SIZE];

    }TWiMOD_HCI_Message;

    //------------------------------------------------------------------------------
    //  Function Prototypes
    //------------------------------------------------------------------------------

    //Message receiver callback
    typedef TWiMOD_HCI_Message *(*TWiMOD_HCI_CbRxMessage)(TWiMOD_HCI_Message *rxMessage);

    //Init HCI Layer
    bool
    WiMOD_HCI_Init(const unsigned char     *comPort,
                   TWiMOD_HCI_CbRxMessage   cbRxMessage,
                   TWiMOD_HCI_Message      *rxMessage);

    //Send HCI Message
    int
    WiMOD_HCI_SendMessage(TWiMOD_HCI_Message *txMessage);

    //Receiver Process
    void
    WiMOD_HCI_Process();

#ifdef	__cplusplus
}
#endif

#endif // WIMOD_HCI_LAYER_H

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
