//------------------------------------------------------------------------------
//
// File:        CRC16.h
// Abstract:    CRC16 calculation
// Version:     0.2
// Date:        18.05.2016
// Disclaimer:  This example code is provided by IMST GmbH on an "AS IS"
//              basis without any warranties.
//
//------------------------------------------------------------------------------

#ifndef    __CRC16_H__
#define    __CRC16_H__

#ifdef	__cplusplus
extern "C" {
#endif

    //--------------------------------------------------------------------------
    //  Section Include Files
    //--------------------------------------------------------------------------

    #include <stdint.h>
    #include <stdbool.h>

    //--------------------------------------------------------------------------
    //  Section Defines & Declarations
    //--------------------------------------------------------------------------

    typedef uint8_t     UINT8;
    typedef uint16_t    UINT16;

    #define CRC16_INIT_VALUE    0xFFFF    // initial value for CRC algorithm
    #define CRC16_GOOD_VALUE    0x0F47    // constant compare value for check
    #define CRC16_POLYNOM       0x8408    // 16-BIT CRC CCITT POLYNOM

    //--------------------------------------------------------------------------
    //  Function Prototypes
    //--------------------------------------------------------------------------

    /**
     * CRC16_Calc
     * @brief:  calculate CRC16
     * This function calculates the one's complement of the standard
     * 16-BIT CRC CCITT polynomial G(x) = 1 + x^5 + x^12 + x^16
     */
    UINT16
    CRC16_Calc  (UINT8     *data,
                 UINT16     length,
                 UINT16     initVal);

    /**
     * CRC16_Check
     * @brief   calculate & test CRC16
     * This function checks a data block with attached CRC16
     */
    bool
    CRC16_Check (UINT8     *data,
                 UINT16     length,
                 UINT16     initVal);

#ifdef	__cplusplus
}
#endif

#endif // __CRC16_H__
//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
