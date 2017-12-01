/*
 * File:    hci_stack.h
 * Author:  Alex F. Bustos
 * 
 * Headers for the implementation of the HCI+CRC16+SLIP+UART functions to be
 * used on WiMOD LoRaWAN EndNodes. Should be homonym of WiMOD_HCI_Layer.h
 * 
 * Here should be the SLIP+CRC
 */

#ifndef LORAWAN_HCI_H
#define	LORAWAN_HCI_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdbool.h>

    //--------------------------------------------------------------------------
    //  Section CONST
    //--------------------------------------------------------------------------

    #define HCI_WKUPCHARS       5
    #define HCI_RX_RESETSTAT    -1
    #define HCI_RX_PENDING      -2
    #define HCI_RX_ESCAPE       -3

    // SLIP Protocol Characters
    #define SLIP_END        0xC0
    #define SLIP_ESC        0xDB
    #define SLIP_ESC_END    0xDC
    #define SLIP_ESC_ESC    0xDD

    //Not so sure about the following defs:
    #define WIMOD_HCI_MSG_HEADER_SIZE       2
    #define WIMOD_HCI_MSG_PAYLOAD_SIZE      200
    #define WIMOD_HCI_MSG_FCS_SIZE          2

    #define LOBYTE(x)                       (x)// & 0x00FF
    #define HIBYTE(x)                       ((UINT16)(x) >> 8)

    //So sure about this typedefs
    typedef struct
    {
        unsigned int        size; // Payload Length, won't be transmitted over UART!!!
        bool                check;     //Checksum verifying bit
        union {
            struct {
            unsigned char   SapID;  // Service Access Point Identifier
            unsigned char   MsgID;  // Message Identifier
            unsigned char   Payload[WIMOD_HCI_MSG_PAYLOAD_SIZE];    // Payload Field
            };
            unsigned char   HCI[2+WIMOD_HCI_MSG_PAYLOAD_SIZE];
        };
        unsigned char       CRC[WIMOD_HCI_MSG_FCS_SIZE];  // Frame Check Sequence Field
    }HCIMessage_t;

    typedef void (*WMHCIuserProc)();
    typedef bool (*WMHCIboolfcchar)(const unsigned char msdelay);   //A bool function receiving a const char
    
    //--------------------------------------------------------------------------
    //  Function Prototypes
    //--------------------------------------------------------------------------

    //Use after reading the Rx buffer
    void ClearRxHCI (void);

    // Las siguientes tres son similares a las que tiene WiMOD_HCI_Layer.h

    //HCI Initialization
    bool InitHCI (
        WMHCIuserProc   HCIRxHandler,   //Handler for processing of Rx HCI messages
        HCIMessage_t   *RxMessage       //HCI message for reception.
    );

    //Envio de un comando HCI
    bool SendHCI (HCIMessage_t *TxMessage);

    //Procesamiento de HCI entrante.
    void IncomingHCIpacker (unsigned char rxByte);

#ifdef	__cplusplus
}
#endif

#endif	/* LORAWAN_HCI_H */