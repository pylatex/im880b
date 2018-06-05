#include "bmp280.h"
#include "I2Cgeneric.h"

#define BMP_ADDR            0x76

static char                 buff[2];

static struct {
    char            addr;
} BMP;

void BMP280init(bool SDOstate) {
    BMP.addr=(char)(BMP_ADDR | (SDOstate?1:0));
}

bool BMP280read(char BMPaddr,char length,char *buffer) {
    if (BMP280addr(BMPaddr)) {
        return I2Cread (BMP.addr, buffer, length);
    }
    return false;
}

static char *value=0;

bool BMP280addr(char addr) {
    char BMPaddr=addr;
    char sz=0;
    buff[sz++]=BMPaddr;
    if (value) switch (BMPaddr) {
        case BMPconfig:
        case BMPctrl_meas:
            buff[sz++]=*value;
        default:
            value=0;
            break;
    }
    return I2Cwrite (BMP.addr, buff, sz);
}

bool BMP280write(char address,char data) {
    value=&data;
    return BMP280addr(address);
}
