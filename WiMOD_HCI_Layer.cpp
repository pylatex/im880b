//------------------------------------------------------------------------------
//
//	File:		WiMOD_HCI_Layer.cpp
//
//	Abstract:	WiMOD HCI Message Layer
//
//	Version:	0.1
//
//	Date:		18.05.2016
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS"
//              basis without any warranties.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// Include Files
//
//------------------------------------------------------------------------------

#include "WiMOD_HCI_Layer.h"
#include "CRC16.h"
#include "SLIP.h"
#include "SerialDevice.h"
#include <string.h>

//------------------------------------------------------------------------------
//
//  Forward Declaration
//
//------------------------------------------------------------------------------

// SLIP Message Receiver Callback
static UINT8*   WiMOD_HCI_ProcessRxMessage(UINT8* rxData, int rxLength);

//------------------------------------------------------------------------------
//
// Declare Layer Instance
//
//------------------------------------------------------------------------------

typedef struct
{
    // CRC Error counter
    UINT32                  CRCErrors;

    // RxMessage
    TWiMOD_HCI_Message*     RxMessage;

    // Receiver callback
    TWiMOD_HCI_CbRxMessage  CbRxMessage;

}TWiMOD_HCI_MsgLayer;

//------------------------------------------------------------------------------
//
//  Section RAM
//
//------------------------------------------------------------------------------

// reserve HCI Instance
static TWiMOD_HCI_MsgLayer  HCI;

// reserve one TxBuffer
static UINT8                TxBuffer[sizeof( TWiMOD_HCI_Message ) * 2 + 2];

//------------------------------------------------------------------------------
//
//  Init
//
//  @brief: Init HCI Message layer
//
//------------------------------------------------------------------------------

bool
WiMOD_HCI_Init(const char*              comPort,        // comPort
               TWiMOD_HCI_CbRxMessage   cbRxMessage,    // HCI msg recv. callback
               TWiMOD_HCI_Message*      rxMessage)      // intial rxMessage
{
    // init error counter
    HCI.CRCErrors   =   0;

    // save receiver callback
    HCI.CbRxMessage =   cbRxMessage;

    // save RxMessage
    HCI.RxMessage   =   rxMessage;

    // init SLIP
    SLIP_Init(WiMOD_HCI_ProcessRxMessage);

    // init first RxBuffer to SAP_ID of HCI message, size without 16-Bit Length Field
    SLIP_SetRxBuffer(&rxMessage->SapID, sizeof(TWiMOD_HCI_Message) - sizeof(UINT16));

    // init serial device
    return SerialDevice_Open(comPort, Baudrate_115200, DataBits_8, Parity_None);
}

//------------------------------------------------------------------------------
//
//  SendMessage
//
//  @brief: Send a HCI message (with or without payload)
//
//------------------------------------------------------------------------------

int
WiMOD_HCI_SendMessage(TWiMOD_HCI_Message* txMessage)
{
    // 1. check parameter
    //
    // 1.1 check ptr
    //
    if (!txMessage)
    {
        // error
        return -1;
    }

    // 2. Calculate CRC16 over header and optional payload
    //
    UINT16 crc16 = CRC16_Calc(&txMessage->SapID,
                              txMessage->Length + WIMOD_HCI_MSG_HEADER_SIZE,
                              CRC16_INIT_VALUE);

    // 2.1 get 1's complement !!!
    //
    crc16 = ~crc16;

    // 2.2 attach CRC16 and correct length, LSB first
    //
    txMessage->Payload[txMessage->Length]     = LOBYTE(crc16);
    txMessage->Payload[txMessage->Length + 1] = HIBYTE(crc16);

    // 3. perform SLIP encoding
    //    - start transmission with SAP ID
    //    - correct length by header size

    int txLength = SLIP_EncodeData(TxBuffer,
                                   sizeof(TxBuffer),
                                   &txMessage->SapID,
                                   txMessage->Length + WIMOD_HCI_MSG_HEADER_SIZE + WIMOD_HCI_MSG_FCS_SIZE);
    // message ok ?
    if (txLength > 0)
    {
        // send wakeup chars
        for(int i= 0; i < 40; i++)
            SerialDevice_SendByte(SLIP_END);

        // 4. send octet sequence over serial device
        if (SerialDevice_SendData(TxBuffer, txLength) > 0)
        {
            // return ok
            return 1;
        }
    }

    // error - SLIP layer couldn't encode message - buffer to small ?
    return -1;
}

//------------------------------------------------------------------------------
//
//  Process
//
//  @brief: read incoming serial data
//
//------------------------------------------------------------------------------

void
WiMOD_HCI_Process()
{
    UINT8   rxBuf[20];

    // read small chunk of data
    int rxLength = SerialDevice_ReadData(rxBuf, sizeof(rxBuf));

    // data available ?
    if (rxLength > 0)
    {
        // yes, forward to SLIP decoder, decoded SLIP message will be passed to
        // function "WiMOD_HCI_ProcessRxMessage"
        SLIP_DecodeData(rxBuf, rxLength);
    }
}

//------------------------------------------------------------------------------
//
//  WiMOD_HCI_ProcessRxMessage
//
//  @brief: process received SLIP message and return new rxBuffer
//
//------------------------------------------------------------------------------

static UINT8*
WiMOD_HCI_ProcessRxMessage(UINT8* rxData, int rxLength)
{
    // check min length
    if (rxLength >= (WIMOD_HCI_MSG_HEADER_SIZE + WIMOD_HCI_MSG_FCS_SIZE))
    {
        if (CRC16_Check(rxData, rxLength, CRC16_INIT_VALUE))
        {
            // receiver registered ?
            if (HCI.CbRxMessage)
            {
                // yes, complete message info
                HCI.RxMessage->Length = rxLength - (WIMOD_HCI_MSG_HEADER_SIZE + WIMOD_HCI_MSG_FCS_SIZE);

                // call upper layer receiver and save new RxMessage
                HCI.RxMessage = (*HCI.CbRxMessage)(HCI.RxMessage);
            }
        }
        else
        {
            HCI.CRCErrors++;
        }
    }

    // free HCI message available ?
    if (HCI.RxMessage)
    {
        // yes, return pointer to first byte
        return &HCI.RxMessage->SapID;
    }

    // error, disable SLIP decoder
    return 0;
}

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------

