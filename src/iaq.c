/*
 * Liberia para control de los sensores Telaire T67xx. Codigo basado en los
 * ejemplos disponibles en "i2c.h"
 */

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
#include "iaq.h"
#include "i2c.h"

//------------------------------------------------------------------------------
//  Definitions, Declarations and Variables
//------------------------------------------------------------------------------
#define I2C_MAX_ATTEMPTS    30
#define IAQ_ADDR            IAQ_DEFADDR
//------------------------------------------------------------------------------
//  Function Implementations
//------------------------------------------------------------------------------

bool iaq_read(IAQ_T *obj)
{
    I2C_MESSAGE_STATUS  status;
    unsigned char       attempts;
    
    // this initial value is important
    status = I2C_MESSAGE_PENDING;

    // this portion will read the byte from the memory location.
    for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++) {
        // write one byte to EEPROM (2 is the count of bytes to write)
        I2C_MasterRead((unsigned char *)obj->raw, 9, IAQ_ADDR, &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C_MESSAGE_PENDING);

        if (status == I2C_MESSAGE_COMPLETE)
            break;

        // if status is  I2C_MESSAGE_ADDRESS_NO_ACK,
        //               or I2C_DATA_NO_ACK,
        // The device may be busy and needs more time for the last
        // write so we can retry writing the data, this is why we
        // use a while loop here
    }

    if (status == I2C_MESSAGE_COMPLETE) {
        //CO2 endianness fitting
        obj->pred = (unsigned short)obj->raw[0]<<8 | (unsigned short)obj->raw[1];
        //resistance endianness fitting
        obj->resistance = (unsigned long)obj->raw[4]<<16 | (unsigned long)obj->raw[5]<<8 | (unsigned long)obj->raw[6];
        //TVOC endianness fitting
        obj->tvoc = (unsigned short)obj->raw[7]<<8 | (unsigned short)obj->raw[8];
        return true;
    }
    return false;
}
