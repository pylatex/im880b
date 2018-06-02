/*
 * File:    hci_stack.h
 * Author:  Alex F. Bustos
 *
 * Headers for the implementation of the HCI+CRC16+SLIP+UART functions to be
 * used on WiMOD LoRaWAN EndNodes. Should be homonym of WiMOD_HCI_Layer.h
 *
 * Here should be the SLIP+CRC
 */

#ifndef MINIFIED_HCI_H
#define MINIFIED_HCI_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdbool.h>

    //--------------------------------------------------------------------------
    //  Section CONST
    //--------------------------------------------------------------------------

    #define HCI_WKUPCHARS       5

    // SLIP Protocol Characters
    #define SLIP_END        0xC0
    #define SLIP_ESC        0xDB
    #define SLIP_ESC_END    0xDC
    #define SLIP_ESC_ESC    0xDD

    //Not so sure about the following defs:
    #define WIMOD_HCI_MSG_PAYLOAD_SIZE      200
    #define WIMOD_HCI_MSG_FCS_SIZE          2

    #define LOBYTE(x)                       (x)// & 0x00FF
    #define HIBYTE(x)                       ((UINT16)(x) >> 8)

    //So sure about this typedefs
    /*
    typedef union {
        struct {
        unsigned char   SapID;      // Service Access Point Identifier
        unsigned char   MsgID;      // Message Identifier
        unsigned char   Payload[];  // Payload Field
        };
        unsigned char   HCI[];      // The whole HCI message, without CRC field
    } HCImsg_t;

    typedef struct {
        unsigned int    size;   // Payload Length, won't be transmitted over UART!!!
        bool            check;  // Checksum verifying bit
        unsigned char   CRC[WIMOD_HCI_MSG_FCS_SIZE];    // Frame Check Sequence Field
        HCImsg_t       *HCImsg;
    } HCIMessage_t2;
    // */

    typedef struct
    {
        unsigned char       size; // Payload Length, won't be transmitted over UART!!!
        bool                check;     //Checksum verifying bit
        union {
            struct {
            unsigned char   SapID;  // Service Access Point Identifier
            unsigned char   MsgID;  // Message Identifier
            unsigned char   Payload[WIMOD_HCI_MSG_PAYLOAD_SIZE-2];    // Payload Field
            };
            unsigned char   HCI[WIMOD_HCI_MSG_PAYLOAD_SIZE];
        };
        unsigned char       CRC[WIMOD_HCI_MSG_FCS_SIZE];  // Frame Check Sequence Field
    } HCIMessage_t;

    typedef void (*WMHCIuserProc)(HCIMessage_t *receivedHCI);
    typedef bool (*serialTransmitHandler)(char *buffer,char length);

    //--------------------------------------------------------------------------
    //  Function Prototypes
    //--------------------------------------------------------------------------

    // The following three functions are similar to the ones in WiMOD_HCI_Layer.h

    /**
     * Initializes the serial port / UART
     * @param UserHandlerRx Call to user function that process a formed HCI message
     * @return true on success
     */
    void InitHCI (
        WMHCIuserProc           HCIRxHandler,   //Handler for processing of Rx HCI messages
        serialTransmitHandler   TxFunction      //Handler of function that send messages over UART
    );

    /**
     * @brief: Sends an HCI message
     * Calculates the CRC to the HCI message to be sent, stored in buffer, then
     * applies the SLIP wrapper to the result and sends it through the UART.
     *
     * @param buffer: HCI message. In 0 the DstID, in 1 the MsgID. Else the payload
     * @param size: Size of the payload of the HCI message
     */
    bool SendHCI (HCIMessage_t *TxMessage);

    /**
     * Groups every incoming UART octect in an HCI message. Additionally, when
     * the HCI message is complete, it calls the (handler) function given by
     * user at initialization, to process the received HCI message.
     *
     * @param rxByte: The byte received by the UART
     */
    void IncomingHCIpacker (unsigned char rxByte);

#ifdef	__cplusplus
}
#endif

#endif	/* LORAWAN_HCI_H */
