//------------------------------------------------------------------------------
//
//  File:		SLIP.cpp
//
//  Abstract:	SLIP Encoder / Decoder
//
//  Version:	0.2
//
//  Date:		18.05.2016
//
//  Disclaimer:	This example code is provided by IMST GmbH on an "AS IS"
//              basis without any warranties.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  Include Files
//
//------------------------------------------------------------------------------

#include "SLIP.h"

//------------------------------------------------------------------------------
//
//  Protocol Definitions
//
//------------------------------------------------------------------------------

// SLIP Receiver/Decoder States
#define SLIPDEC_IDLE_STATE          0
#define	SLIPDEC_START_STATE			1
#define	SLIPDEC_IN_FRAME_STATE		2
#define	SLIPDEC_ESC_STATE			3

//------------------------------------------------------------------------------
//
// Declare SLIP Variables
//
//------------------------------------------------------------------------------

typedef struct
{
    // Decoder
    int                 RxState;
    int                 RxIndex;
    int                 RxBufferSize;
    UINT8*              RxBuffer;
    TSLIP_CbRxMessage   CbRxMessage;

    // Encoder
    int                 TxIndex;
    int                 TxBufferSize;
    UINT8*              TxBuffer;
}TSLIP;

//------------------------------------------------------------------------------
//
// Section RAM
//
//------------------------------------------------------------------------------

// SLIP Instance
static TSLIP   SLIP;

//------------------------------------------------------------------------------
//
// Section Code
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  Init
//
//  @brief: init SLIP decoder
//
//------------------------------------------------------------------------------

void
SLIP_Init(TSLIP_CbRxMessage cbRxMessage)
{
    // init decoder to idle state, no rx-buffer avaliable
    SLIP.RxState         =   SLIPDEC_IDLE_STATE;
    SLIP.RxIndex         =   0;
    SLIP.RxBuffer        =   0;
    SLIP.RxBufferSize    =   0;

    // save message receiver callback
    SLIP.CbRxMessage     =   cbRxMessage;

    // init encoder
    SLIP.TxIndex         =   0;
    SLIP.TxBuffer        =   0;
    SLIP.TxBufferSize    =   0;
}

//------------------------------------------------------------------------------
//
//  SLIP_StoreTxByte
//
//  @brief: store a byte into TxBuffer
//
//------------------------------------------------------------------------------

static void
SLIP_StoreTxByte(UINT8 txByte)
{
  if (SLIP.TxIndex < SLIP.TxBufferSize)
      SLIP.TxBuffer[SLIP.TxIndex++] = txByte;
}

//------------------------------------------------------------------------------
//
//  EncodeData
//
//  @brief: encode a messages into dstBuffer
//
//------------------------------------------------------------------------------

int
SLIP_EncodeData(UINT8* dstBuffer, int dstBufferSize, UINT8* srcData, int srcLength)
{
    // save start pointer
    int txLength = 0;

    // init TxBuffer
    SLIP.TxBuffer = dstBuffer;

    // init TxIndex
    SLIP.TxIndex  = 0;

    // init size
    SLIP.TxBufferSize = dstBufferSize;

    // send start of SLIP message
    SLIP_StoreTxByte(SLIP_END);

    // iterate over all message bytes
    while(srcLength--)
    {
        switch (*srcData)
        {
                case SLIP_END:
                    SLIP_StoreTxByte(SLIP_ESC);
                    SLIP_StoreTxByte(SLIP_ESC_END);
                    break;

                case SLIP_ESC:
                    SLIP_StoreTxByte(SLIP_ESC);
                    SLIP_StoreTxByte(SLIP_ESC_ESC);
                    break;

                default:
                    SLIP_StoreTxByte(*srcData);
                    break;
        }
        // next byte
        srcData++;
    }

    // send end of SLIP message
    SLIP_StoreTxByte(SLIP_END);

    // length ok ?
    if (SLIP.TxIndex <= SLIP.TxBufferSize)
        return SLIP.TxIndex;

    // return tx length error
    return -1;
}

//------------------------------------------------------------------------------
//
//  SetRxBuffer
//
//  @brief: configure a rx-buffer and enable receiver/decoder
//
//------------------------------------------------------------------------------

bool
SLIP_SetRxBuffer(UINT8* rxBuffer, int rxBufferSize)
{
    // receiver in IDLE state and client already registered ?
    if ((SLIP.RxState == SLIPDEC_IDLE_STATE) && SLIP.CbRxMessage)
    {
        // same buffer params
        SLIP.RxBuffer        = rxBuffer;
        SLIP.RxBufferSize    = rxBufferSize;

        // enable decoder
        SLIP.RxState = SLIPDEC_START_STATE;

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
//
//  SLIP_StoreRxByte
//
//  @brief: store SLIP decoded rxByte
//
//------------------------------------------------------------------------------

static void
SLIP_StoreRxByte(UINT8 rxByte)
{
    if (SLIP.RxIndex < SLIP.RxBufferSize)
        SLIP.RxBuffer[SLIP.RxIndex++] = rxByte;
}

//------------------------------------------------------------------------------
//
//  DecodeData
//
//  @brief: process received byte stream
//
//------------------------------------------------------------------------------

void
SLIP_DecodeData(UINT8* srcData, int srcLength)
{
    // init result
    int result = 0;

    // iterate over all received bytes
    while(srcLength--)
    {
        // get rxByte
        UINT8 rxByte = *srcData++;

        // decode according to current state
        switch(SLIP.RxState)
        {
            case    SLIPDEC_START_STATE:
                    // start of SLIP frame ?
                    if(rxByte == SLIP_END)
                    {
                        // init read index
                        SLIP.RxIndex = 0;

                        // next state
                        SLIP.RxState = SLIPDEC_IN_FRAME_STATE;
                    }
                    break;

            case    SLIPDEC_IN_FRAME_STATE:
                    switch(rxByte)
                    {
                        case    SLIP_END:
                                // data received ?
                                if(SLIP.RxIndex > 0)
                                {
                                    // yes, receiver registered ?
                                    if (SLIP.CbRxMessage)
                                    {
                                        // yes, call message receive
                                        SLIP.RxBuffer = (*SLIP.CbRxMessage)(SLIP.RxBuffer, SLIP.RxIndex);

                                        // new buffer available ?
                                        if (!SLIP.RxBuffer)
                                        {
                                            SLIP.RxState = SLIPDEC_IDLE_STATE;
                                        }
                                        else
                                        {
                                            SLIP.RxState = SLIPDEC_START_STATE;
                                        }
                                    }
                                    else
                                    {
                                        // disable decoder, temp. no buffer avaliable
                                        SLIP.RxState = SLIPDEC_IDLE_STATE;
                                    }
                                }
                                // init read index
                                SLIP.RxIndex = 0;
                                break;

                        case  SLIP_ESC:
                                // enter escape sequence state
                                SLIP.RxState = SLIPDEC_ESC_STATE;
                                break;

                        default:
                                // store byte
                                SLIP_StoreRxByte(rxByte);
                                break;
                    }
                    break;

            case    SLIPDEC_ESC_STATE:
                    switch(rxByte)
                    {
                        case    SLIP_ESC_END:
                                SLIP_StoreRxByte(SLIP_END);
                                // quit escape sequence state
                                SLIP.RxState = SLIPDEC_IN_FRAME_STATE;
                                break;

                        case    SLIP_ESC_ESC:
                                SLIP_StoreRxByte(SLIP_ESC);
                                // quit escape sequence state
                                SLIP.RxState = SLIPDEC_IN_FRAME_STATE;
                                break;

                        default:
                                // abort frame receiption
                                SLIP.RxState = SLIPDEC_START_STATE;
                                break;
                    }
                    break;

            default:
                    break;
        }
    }
}

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
