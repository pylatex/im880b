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

//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------

static struct {
    WMHCIuserProc   RxHandler;
    serialTransmitHandler   tx;
} WMHCIsetup;

volatile HCIMessage_t   HCIrxMessage;
//------------------------------------------------------------------------------
// Section Source
//------------------------------------------------------------------------------

//HCI Initialization
void InitHCI (
    WMHCIuserProc           HCI_RxHandler,  //a function that returns a bool, and receives a HCIMessage_t
    serialTransmitHandler   TxFunction      //Handler of function that send messages over UART
)
{
    WMHCIsetup.tx=TxFunction;
    WMHCIsetup.RxHandler=HCI_RxHandler;  //Saves the User Function for Processing of HCI messages
    HCIrxMessage.size=0;
    HCIrxMessage.check=false;
}

//Envio de un comando HCI
bool SendHCI (HCIMessage_t *TxMessage)
{
    char aux=HCI_WKUPCHARS,aux2[2];
    unsigned short crc,size=TxMessage->size;
    size+=2;    //Including the header
    //SERIAL WRAPPING LAYER
    //CRC Generation and a bitwise negation of the result.
    crc= ~(CRC16_Calc(TxMessage->HCI,size,CRC16_INIT_VALUE));
    size+=2;    //Including the final 16 bit CRC

    *aux2=SLIP_END;
    while(aux--)
        WMHCIsetup.tx(aux2,1);

    //UART LAYER + SLIP ENCODING
    WMHCIsetup.tx(aux2,1);
    for (unsigned char i=0;i < size ;i++) {
        if (i < size-2) {
            aux=TxMessage->HCI[i];   //Recycling of unused variable
        } else if (i == size-2) {
            aux=LOBYTE(crc);
        } else {    //i==size-1
            aux=HIBYTE(crc);
        }
        //SLIP coding
        *aux2=SLIP_ESC;
        switch (aux) {
            case SLIP_END:
                aux2[1]=SLIP_ESC_END;
                WMHCIsetup.tx(aux2,2);
                break;
            case SLIP_ESC:
                aux2[1]=SLIP_ESC_ESC;
                WMHCIsetup.tx(aux2,2);
                break;
            default:
                WMHCIsetup.tx(&aux,1);
                break;
        }
    }
    *aux2=SLIP_END;
    return (bool) WMHCIsetup.tx(aux2,1);
}

//Procesamiento de HCI entrante.
void IncomingHCIpacker (unsigned char rxByte)
{
    static bool escape=false;

    if (rxByte==SLIP_END) {
        if (HCIrxMessage.size >=  2+WIMOD_HCI_MSG_FCS_SIZE) {
            HCIrxMessage.check=CRC16_Check(HCIrxMessage.HCI, HCIrxMessage.size, CRC16_INIT_VALUE);
            HCIrxMessage.size -= (2+WIMOD_HCI_MSG_FCS_SIZE);   //Net payload size
            WMHCIsetup.RxHandler();
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
