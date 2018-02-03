//------------------------------------------------------------------------------
//
//	File:		SLIP.h
//	Abstract:	SLIP Encode / Decoder
//	Version:	0.2
//	Date:		18.05.2016
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS"
//              basis without any warranties.
//
//------------------------------------------------------------------------------

#ifndef SLIP_H
#define SLIP_H

#ifdef	__cplusplus
extern "C" {
#endif
    //--------------------------------------------------------------------------
    // Include Files
    //--------------------------------------------------------------------------

    #include <stdint.h>
    #include <stdbool.h>

    //------------------------------------------------------------------------------
    // General Definitions
    //------------------------------------------------------------------------------

    // SLIP Protocol Characters
    #define SLIP_END        0xC0
    #define SLIP_ESC        0xDB
    #define SLIP_ESC_END    0xDC
    #define SLIP_ESC_ESC    0xDD

    typedef uint8_t     UINT8;

    //------------------------------------------------------------------------------
    //  Function Prototypes
    //------------------------------------------------------------------------------

    // SLIP message receiver callback
    typedef UINT8  *(*TSLIP_CbRxMessage)(UINT8 *message, int length);

    /**
     * Init
     * @brief: init SLIP decoder
     */
    void
    SLIP_Init(TSLIP_CbRxMessage cbRxMessage);

    /**
     * SetRxBuffer
     * @brief: configure a rx-buffer and enable receiver/decoder
     */
    bool
    SLIP_SetRxBuffer(UINT8 *rxBuffer, int rxBufferSize);

    /**
     * EncodeData
     * @brief: encode a messages into dstBuffer
     */
    int
    SLIP_EncodeData(UINT8 *dstBuffer, int txBufferSize, UINT8 *srcData,int srcLength);

    /**
     * DecodeData
     * @brief: process received byte stream
     */
    void
    SLIP_DecodeData(UINT8 *srcData, int srcLength);

#ifdef	__cplusplus
}
#endif

#endif // SLIP_H

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
