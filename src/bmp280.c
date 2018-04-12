#include "bmp280.h"
#include "i2c.h"

#define BMP_ADDR            0x76
#define I2C_MAX_ATTEMPTS    50

static char                 I2Caddr,buff[2];
static I2C_MESSAGE_STATUS   status;
static uint16_t             attempts;

static struct {
    char            addr;
} BMP;

void BMP280init(bool SDOstate) {
    BMP.addr=(char)(BMP_ADDR | (SDOstate?1:0));
}

bool BMP280read(char BMPaddr,char length,char *buffer) {
    if (BMP280addr(BMPaddr)) {
        // this portion will read the byte from the memory location.
        for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++) {
            // write one byte to EEPROM (2 is the count of bytes to write)
            I2C_MasterRead(buffer, length, BMP.addr, &status);

            // wait for the message to be sent or status has changed.
            while(status == I2C_MESSAGE_PENDING);

            if (status == I2C_MESSAGE_COMPLETE)
                return true; //else may be busy, the reason of this loop.
        }
    }
    return false;
}

static char BMPaddr;
static char *value=0;

bool BMP280addr(char addr) {
    BMPaddr=addr;
    char sz=0;
    buff[sz++]=BMPaddr;
    if (value)
        switch (BMPaddr) {
            case BMPconfig:
            case BMPctrl_meas:
                buff[sz++]=*value;
            default:
                value=0;
                break;
        }

    for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++)
    {
        // Define the register to be read from sensor
        I2C_MasterWrite(buff, sz, BMP.addr, &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C_MESSAGE_PENDING);

        if (status == I2C_MESSAGE_COMPLETE)
            return true;
    }
    return false;
}

bool BMP280write(char address,char data) {
    value=&data;
    return BMP280addr(address);
}
