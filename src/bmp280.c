#include "bmp280.h"
#include "i2c.h"

#define BMP_ADDR            0x76
#define I2C_MAX_ATTEMPTS    50

static char                 I2Caddr,buff[2];
static I2C_MESSAGE_STATUS   status;
static uint16_t             attempts;

void BMP280init(bool SDOstate) {
    I2Caddr=(char)(BMP_ADDR | (SDOstate?1:0));
}

bool BMP280updateTrim(char *arr) {
    return BMP280read(BMPcalib(0),24,arr);
}

bool BMP280updateValues(char *arr) {
    return BMP280read(BMPpress_msb,6,arr);
}

bool BMP280read(char BMPaddr,char length,char *buffer) {
    if (BMP280write(BMPaddr,0)) {
        // this portion will read the byte from the memory location.
        for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++) {
            // write one byte to EEPROM (2 is the count of bytes to write)
            I2C_MasterRead(buffer, length, I2Caddr, &status);

            // wait for the message to be sent or status has changed.
            while(status == I2C_MESSAGE_PENDING);

            if (status == I2C_MESSAGE_COMPLETE)
                return true; //else may be busy, the reason of this loop.
        }
    }
    return false;
}

bool BMP280write(char BMPaddr,char *value) {
    char sz=0;
    buff[sz++]=BMPaddr;
    if (value)
        switch (BMPaddr) {
            case BMPconfig:
            case BMPctrl_meas:
                buff[sz++]=*value;
            default:
                break;
        }

    for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++)
    {
        // Define the register to be read from sensor
        I2C_MasterWrite(&BMPaddr, sz, I2Caddr, &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C_MESSAGE_PENDING);

        if (status == I2C_MESSAGE_COMPLETE)
            return true;
    }
    return false;
}