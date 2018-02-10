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

IAQ_T *iaq_read(void)
{
    static IAQ_T        IAQ;
    I2C_MESSAGE_STATUS  status;
    unsigned char       attempts;
    
    // this initial value is important
    status = I2C_MESSAGE_PENDING;

    // this portion will read the byte from the memory location.
    for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++) {
        // write one byte to EEPROM (2 is the count of bytes to write)
        I2C_MasterRead(IAQ.raw, 9, IAQ_ADDR, &status);

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
        unsigned char aux;

        //CO2 endianness fitting
        aux=IAQ.CO2P_0;
        IAQ.CO2P_1=IAQ.CO2P_0;
        IAQ.CO2P_0=aux;
        //resistance endianness fitting
        aux=IAQ.RES_0;
        IAQ.RES_0=IAQ.RES_3;
        IAQ.RES_3=aux;
        aux=IAQ.RES_1;
        IAQ.RES_1=IAQ.RES_2;
        IAQ.RES_2=aux;
        //TVOC endianness fitting
        aux=IAQ.TVOCe_0;
        IAQ.TVOCe_0=IAQ.TVOCe_1;
        IAQ.TVOCe_1=aux;

        return &IAQ;
    } else return 0;   //null pointer
}
