/*
 * Liberia para control de los sensores Telaire T67xx. Codigo basado en los
 * ejemplos disponibles en "i2c.h"
 */

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
#include "iaq.h"
#include "I2Cgeneric.h"

//------------------------------------------------------------------------------
//  Function Implementations
//------------------------------------------------------------------------------

bool iaq_read(IAQ_T *obj)
{
    // this portion will read the byte from the memory location.
    if (I2Cread (IAQ_DEFADDR, obj->raw, 9)) {
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
