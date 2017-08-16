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

volatile signed char rxStatus;   //Reflejo del ultimo estado HCI del receptor

//------------------------------------------------------------------------------
// Section Source
//------------------------------------------------------------------------------

/**
 * State of the HCI receiver or payload size of a complete incoming HCI message.
 * 
 * @return -1 when no message, -2 when receiving and >=0 as the size of a valid HCI.
 */
signed char BuffSizeHCI(void) {
    return rxStatus;
}

/**
 * Indicates if there is an available message to read
 * 
 * @deprecated Please use a comparison of the value returned by Buffsize()
 * @return true or false, depending if there is a valid message available to be read
 */
bool PendingRxHCI(void) {
    return (bool)(rxStatus >= 0);
}

/**
 * Use after reading the buffer.
 */
void ClearRxHCI (void) {
    rxStatus=HCI_RX_RESETSTAT;
}

/**
 * Initializes the serial port / UART
 * @return True if success.
 */
bool InitHCI (const unsigned char* comPort) {
    ClearRxHCI();
    return SerialDevice_Open(comPort,8,0);   //Enables UART, and from it: RX, TX and interrupts by RX
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
    unsigned char aux=HCI_WKUPCHARS;
    unsigned int crc;
    size+=2;
    //SERIAL WRAPPING LAYER
    //CRC Generation and a bitwise negation of the result.
    crc= ~(CRC16_Calc(buffer,size,CRC16_INIT_VALUE));

    //Attach CRC16 and correct length, LSB first
    buffer[size++]=(unsigned char)(crc&0x00FF);
    buffer[size++]=(unsigned char)(crc>>8);

    while(aux--)
        SerialDevice_SendByte(SLIP_END);

    //UART LAYER + SLIP ENCODING
    SerialDevice_SendByte(SLIP_END);
    for (unsigned char i=0;i<size;i++) {
        aux=buffer[i];   //Recycling of unused variable
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
 * Evaluates every incoming octect from UART, in order to create an HCI message.
 * 
 * @param buffer: Buffer for the bytes of the incoming HCI message.
 * @param rxByte: The byte received by the UART
 */
void ProcessHCI (unsigned char *buffer, unsigned int rxByte)
{
    //Variables
    static bool escape = false;
    static unsigned char size = 0;
    
    if (rxByte==SLIP_END) {
        if ((size >= 4) && CRC16_Check(buffer, size, CRC16_INIT_VALUE)) {
            rxStatus=size - 4;  //Valid HCI message with (size-4) payload bytes
        } else {
            rxStatus=HCI_RX_RESETSTAT;    // Aborted/non valid HCI message
        }
        size=0;
        escape=false;
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
            buffer[size++]=rxByte;
        }
        rxStatus=HCI_RX_PENDING;
    }
}