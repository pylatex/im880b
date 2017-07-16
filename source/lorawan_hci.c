/*
 * File:    lorawan_hci.c
 * Author:  Alex F. Bustos
 * 
 * HCI+CRC16+SLIP implementation to be used with IMST Radio Modules.
 */

//------------------------------------------------------------------------------
// Includings
//------------------------------------------------------------------------------

#include "lorawan_hci.h"    //WiMOD LoRaWAN HCI Constants
#include "CRC16.h"          //CRC16 Calculation and Checking functions
#include "SerialDevice.h"   //UART interface

//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------

//8 bit segmented union...
typedef union {
    unsigned        UREG    :8;
    signed          SREG    :8;
    struct  {
        unsigned    NIB0    :4;
        unsigned    NIB1    :4;
    };
    struct {
        unsigned    B0      :1;
        unsigned    B1      :1;
        unsigned    B2      :1;
        unsigned    B3      :1;
        unsigned    B4      :1;
        unsigned    B5      :1;
        unsigned    B6      :1;
        unsigned    B7      :1;
    };
} SEG8;

//volatile SEG8 HCIstat;              //HCI Modules Status
//#define ble HCIstat.B0  //
volatile signed char rxStatus;   //Reflejo del ultimo estado HCI del receptor

//------------------------------------------------------------------------------
// Section Source
//------------------------------------------------------------------------------

bool PendingRxHCI(void) {
    return (rxStatus >= 0);
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
bool InitHCI (void) {
    ClearRxHCI();
    return SerialDevice_Open(0,0,0);   //Enables UART, and from it: RX, TX and interrupts by RX
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
 * @brief Procesa un octeto recibido por el UART
 * State machine implementation to process the bytes of an incoming HCI message
 * 
 * @param buffer: Reception Buffer.
 * @param rxData: Byte received by the UART
 * @return -1 when no message, -2 when receiving and >=0 as the size of a valid HCI.
 */
void ProcessHCI (unsigned char *buffer, unsigned int rxData)
{
    //Variables
    static bool escape = false;
    static unsigned char size = 0;
    
    if (rxData==SLIP_END) {
        if ((size >= 4) && CRC16_Check(buffer, size, CRC16_INIT_VALUE)) {
            rxStatus=size - 4;  //Valid HCI message with (size-4) payload bytes
        } else {
            rxStatus=HCI_RX_RESETSTAT;    // Aborted/non valid HCI message
        }
        size=0;
        escape=false;
    } else {
        if (rxData==SLIP_ESC) {
            escape=true;
        } else {
            if (escape) {
                if (rxData==SLIP_ESC_END) {
                    rxData=SLIP_END;
                } else if (rxData==SLIP_ESC_ESC) {
                    rxData=SLIP_ESC;
                }
                escape=false;
            }
            buffer[size++]=rxData;
        }
        rxStatus=HCI_RX_PENDING;
    }
}