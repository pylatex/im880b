/*
 * Liberia para control de los sensores Telaire T67xx. Codigo basado en los
 * ejemplos disponibles en "i2c.h"
 */

#define T67XX_I2CMODE
#ifdef T67XX_I2CMODE
#include "i2c.h"
#endif
#include "T67xx.h"

#define T67XX_ADDR          T67XX_DEFADDR
#define I2C_MAX_ATTEMPTS    30

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
#ifdef T67XX_I2CMODE
    I2C_MESSAGE_STATUS status;
    static uint8_t  Buffer[7];
    uint16_t        attempts;

    // build the write buffer first (MODBUS Request w/o CRC)
    // starting address of the EEPROM memory
    Buffer[0] = fc;                    //Function Code
    Buffer[1] = (address >> 8);        //high byte
    Buffer[2] = (uint8_t)(address);    //low byte
    Buffer[3] = 0;     //Quantity of registers
    Buffer[4] = 1;     //to be read

    // Now it is possible that the slave device will be slow.
    // As a work around on these slaves, the application can
    // retry sending the transaction
    for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++)
    {
        // Define the register to be read from sensor
        I2C_MasterWrite(Buffer, 5, T67XX_ADDR, &status);

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

    if (status == I2C_MESSAGE_COMPLETE)
    {

        // this portion will read the byte from the memory location.
        for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++) {
            // write one byte to EEPROM (2 is the count of bytes to write)
            I2C_MasterRead(Buffer, RespLength, T67XX_ADDR, &status);

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
    }
    if (status == I2C_MESSAGE_COMPLETE)
        return Buffer;
    else
        return 0;   //null pointer
#endif
}