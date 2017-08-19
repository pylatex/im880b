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
} HCIparams_t;

HCIparams_t             WMHCIsetup;
HCIMessage_t            HCIrxMessage;
//------------------------------------------------------------------------------
// Section Source
//------------------------------------------------------------------------------

/**
 * Initializes the serial port / UART
 * @param UserHandlerRx Call to user function that process a formed HCI message
 * @return true on success
 */
bool InitHCI (
    WMHCIuserProc UserHandlerRx    //a function that returns a bool, and receives a HCIMessage_t
)
{
    WMHCIsetup.RxHandler=UserHandlerRx;  //Saves the User Function for Processing of HCI messages
    HCIrxMessage.size=0;
    HCIrxMessage.check=false;
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
bool SendHCI (unsigned char *buffer, unsigned int size)
{
    unsigned short aux=HCI_WKUPCHARS;
    unsigned int crc;
    size+=2;
    //SERIAL WRAPPING LAYER
    //CRC Generation and a bitwise negation of the result.
    crc= ~(CRC16_Calc(buffer,size,CRC16_INIT_VALUE));
    size+=2;
    //Attach CRC16 and correct length, LSB first
    //buffer[size++]=(unsigned char)(crc&0x00FF);
    //buffer[size++]=(unsigned char)(crc>>8);

    while(aux--)
        SerialDevice_SendByte(SLIP_END);

    //UART LAYER + SLIP ENCODING
    SerialDevice_SendByte(SLIP_END);
    for (unsigned char i=0;i<size;i++) {
        if (i<size-2) {
            aux=buffer[i];   //Recycling of unused variable
        } else if (i==size-2) {
            //aux=(unsigned char)(crc&0x00FF);
            aux=LOBYTE(crc);
        } else {    //i==size-1
            //aux=(unsigned char)(crc>>8);
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
        if (HCIrxMessage.size >=  2+WIMOD_HCI_MSG_FCS_SIZE) {
            HCIrxMessage.check=CRC16_Check(HCIrxMessage.HCI, HCIrxMessage.size, CRC16_INIT_VALUE);
            HCIrxMessage.size -= (2+WIMOD_HCI_MSG_FCS_SIZE);   //Net payload size
            WMHCIsetup.RxHandler(&HCIrxMessage);
        }
        escape=false;
        HCIrxMessage.size=0;
        HCIrxMessage.check=false;
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
            HCIrxMessage.HCI[HCIrxMessage.size++]=rxByte;
        }
    }
}
