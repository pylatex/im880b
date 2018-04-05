#include "bmp280.h"
#include "i2c.h"

#define BMP_ADDR    0x76

static char I2Caddr;

void BMP280init(bool SDOstate) {
    I2Caddr=(char)(BMP_ADDR | (SDOstate?1:0));
}

bool BMP280updateTemp(unsigned long *value) {
    return false;
}

bool BMP280updateHumidity(unsigned long *value) {
    return false;
}

bool BMP280read(char BMPaddr,char length,char *buffer) {
    return false;
}

bool BMP280write(char BMPaddr,char value) {
    return false;
}