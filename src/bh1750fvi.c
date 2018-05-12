#include "i2c.h"
#include "bh1750fvi.h"

#define I2C_MAX_ATTEMPTS 50

static I2C_MESSAGE_STATUS  status;
static char         attempts,buff[6],addr;

void BHinit(bool ADDRstat) {
    addr = ADDRstat?BH1750_ADDR_H:BH1750_ADDR_L;
}

bool BHwrite (char command) {
    buff[0]=command;
    for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++)
    {
        // Define the register to be read from sensor
        I2C_MasterWrite(buff, 1, addr, &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C_MESSAGE_PENDING);

        if (status == I2C_MESSAGE_COMPLETE)
            return true;
    }
    return false;
}

bool BHread (unsigned short *result) {

    // this portion will read the byte from the memory location.
    for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++) {
        // write one byte to EEPROM (2 is the count of bytes to write)
        I2C_MasterRead(buff, 2, addr, &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C_MESSAGE_PENDING);

        if (status == I2C_MESSAGE_COMPLETE) {
            *result = (unsigned short)buff[0]<<8 | (unsigned short)buff[1];
            return true;
        }
    }
    return false;
}
