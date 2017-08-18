//------------------------------------------------------------------------------
//
//	File:		CRC16.h
//
//	Abstract:	CRC16 calculation
//
//	Version:	0.2
//
//	Date:		18.05.2016
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS"
//              basis without any warranties.
//
//------------------------------------------------------------------------------

#ifndef    __CRC16_H__
#define    __CRC16_H__

//------------------------------------------------------------------------------
//
//  Section Include Files
//
//------------------------------------------------------------------------------

#include <stdint.h>

//------------------------------------------------------------------------------
//
//  Section Defines & Declarations
//
//------------------------------------------------------------------------------

typedef uint8_t     UINT8;
typedef uint16_t    UINT16;

#define CRC16_INIT_VALUE    0xFFFF    // initial value for CRC algorithm
#define CRC16_GOOD_VALUE    0x0F47    // constant compare value for check
#define CRC16_POLYNOM       0x8408    // 16-BIT CRC CCITT POLYNOM

//------------------------------------------------------------------------------
//
//  Function Prototypes
//
//------------------------------------------------------------------------------

// Calc CRC16
UINT16
CRC16_Calc  (UINT8     *data,
             UINT16     length,
             UINT16     initVal);

// Calc & Check CRC16
bool
CRC16_Check (UINT8     *data,
             UINT16     length,
             UINT16     initVal);


#endif // __CRC16_H__
//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
