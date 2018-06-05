/*
 * Liberia para control de los sensores Telaire T67xx. Codigo basado en los
 * ejemplos disponibles en "i2c.h"
 */

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------

#include "T67xx.h"

#define T67XX_I2C_MODE
//#define T67XX_UART_MODE

#ifdef T67XX_I2C_MODE
#include "I2Cgeneric.h"
#define I2C_MAX_ATTEMPTS    30
#endif

#ifdef T67XX_UART_MODE
#include "SerialDevice.h"
#endif

//------------------------------------------------------------------------------
//  Definitions, Declarations and Variables
//------------------------------------------------------------------------------

#define T67XX_ADDR          T67XX_DEFADDR

unsigned char *T67XX_Read(unsigned char fc,unsigned short address,unsigned char RespLength);

//------------------------------------------------------------------------------
//  Function Implementations
//------------------------------------------------------------------------------

/**
 * Reads a register on the T67xx sensor.
 * from T67xx sensor.
 * @param fc:           Function Code
 * @param address:      Address of the register
 * @param RespLength:   Expected length of the response (in bytes)
 * @return A pointer to the array result of reading the sensor register specified
 */
unsigned char *T67XX_Read(unsigned char fc,unsigned short address,unsigned char RespLength)
{   
    static uint8_t  Buffer[7];

    // build the write buffer first (MODBUS Request w/o CRC)
    // starting address of the EEPROM memory
    Buffer[0] = fc;                    //Function Code
    Buffer[1] = (address >> 8);        //high byte
    Buffer[2] = (uint8_t)(address);    //low byte
    Buffer[3] = 0;     //Quantity of registers
    Buffer[4] = 1;     //to be read

#ifdef T67XX_I2C_MODE
    // Now it is possible that the slave device will be slow.
    // As a work around on these slaves, the application can
    // retry sending the transaction
    if (I2Cwrite (T67XX_ADDR, Buffer, 5))
        if (I2Cread (T67XX_ADDR, Buffer, RespLength))
            return Buffer;

    return 0;   //null pointer
#endif  //T67XX_I2C_MODE

#ifdef T67XX_UART_MODE
    
#endif
}

bool T67xx_CO2 (unsigned short *reg)
{
    unsigned char *arrp;

    if (arrp = T67XX_Read(T67XX_GASPPM_FC,T67XX_GASPPM,4))
    {
        *reg = (unsigned short)(arrp[2]<<8) + (unsigned short)(arrp[3]);
        return true;
    }
    return false;
}
