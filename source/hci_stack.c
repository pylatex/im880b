/*
 * File:    hci_stack.c
 * Author:  Alex F. Bustos
 * 
 * Compact implementation of the HCI+CRC16+SLIP layers, to be used with IMST
 * WiMOD. If used, ONLY compile this hci_stack.c source (Exclude HCI and SLIP)
 */

//------------------------------------------------------------------------------
// Includings
//------------------------------------------------------------------------------

#include "hci_stack.h"      //WiMOD LoRaWAN HCI Constants
#include "CRC16.h"          //CRC16 Calculation and Checking functions
#include "SerialDevice.h"   //UART interface

//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------

typedef struct {
    WMHCIuserProc   RxHandler;
    HCIMessage_t   *rxmsg;
} HCIsetup_t;

volatile static HCIsetup_t              WMHCIsetup;
volatile static HCIMessage_t            HCIrxMessage;
//------------------------------------------------------------------------------
// Section Source
//------------------------------------------------------------------------------

/**
 * Initializes the serial port / UART
 * @param UserHandlerRx Call to user function that process a formed HCI message
 * @return true on success
 */
bool InitHCI (
    WMHCIuserProc   HCI_RxHandler,    //a function that returns a bool, and receives a HCIMessage_t
    HCIMessage_t   *RxMessage       //HCI message for reception.
)
{
    WMHCIsetup.RxHandler=HCI_RxHandler;  //Saves the User Function for Processing of HCI messages
    WMHCIsetup.rxmsg=RxMessage;
    WMHCIsetup.rxmsg->size=0;
    WMHCIsetup.rxmsg->check=false;
    return SerialDevice_Open("",8,0);   //Enables UART, and from it: RX, TX and interrupts by RX
}

/**
 * @brief: Sends an HCI message
 * Calculates the CRC to the HCI message to be sent, stored in buffer, then
 * applies the SLIP wrapper to the result and sends it through the UART.
 * 
 * @param buffer: HCI message. In 0 the DstID, in 1 the MsgID. Else the payload
 * @param size: Size of the payload of the HCI message
 */
bool SendHCI (HCIMessage_t *TxMessage)
{
    unsigned short aux=HCI_WKUPCHARS;
    unsigned short crc,size=TxMessage->size;
    size+=2;    //Including the header
    //SERIAL WRAPPING LAYER
    //CRC Generation and a bitwise negation of the result.
    crc= ~(CRC16_Calc(TxMessage->HCI,size,CRC16_INIT_VALUE));
    size+=2;    //Including the final 16 bit CRC

    while(aux--)
        SerialDevice_SendByte(SLIP_END);

    //UART LAYER + SLIP ENCODING
    SerialDevice_SendByte(SLIP_END);
    for (unsigned char i=0;i < size ;i++) {
        if (i < size-2) {
            aux=TxMessage->HCI[i];   //Recycling of unused variable
        } else if (i == size-2) {
            aux=LOBYTE(crc);
        } else {    //i==size-1
            aux=HIBYTE(crc);
        }
        //SLIP coding
        switch (aux) {
            case SLIP_END:
                SerialDevice_SendByte(SLIP_ESC);
                SerialDevice_SendByte(SLIP_ESC_END);
                break;
            case SLIP_ESC:
                SerialDevice_SendByte(SLIP_ESC);
                SerialDevice_SendByte(SLIP_ESC_ESC);
                break;
            default:
                SerialDevice_SendByte(aux);
                break;
        }
    }
    return (bool) SerialDevice_SendByte(SLIP_END);
}

/**
 * Groups every incoming UART octect in an HCI message. At the end calls the
 * function given by user at initialization, to process the formed HCI message.
 * 
 * @param rxByte: The byte received by the UART
 */
void IncomingHCIpacker (unsigned char rxByte)
{
    static bool escape=false;

    if (rxByte==SLIP_END) {
        if (WMHCIsetup.rxmsg->size >=  2+WIMOD_HCI_MSG_FCS_SIZE) {
            WMHCIsetup.rxmsg->check=CRC16_Check(WMHCIsetup.rxmsg->HCI, WMHCIsetup.rxmsg->size, CRC16_INIT_VALUE);
            WMHCIsetup.rxmsg->size -= (2+WIMOD_HCI_MSG_FCS_SIZE);   //Net payload size
            WMHCIsetup.RxHandler();
        }
        escape=false;
        WMHCIsetup.rxmsg->size=0;
        WMHCIsetup.rxmsg->check=false;
    } else {
        if (rxByte==SLIP_ESC) {
            escape=true;
        } else {
            if (escape) {
                if (rxByte==SLIP_ESC_END) {
                    rxByte=SLIP_END;
                } else if (rxByte==SLIP_ESC_ESC) {
                    rxByte=SLIP_ESC;
                }
                escape=false;
            }
            WMHCIsetup.rxmsg->HCI[WMHCIsetup.rxmsg->size++]=rxByte;
        }
    }
}
