#include "bh1750fvi.h"
#include "I2Cgeneric.h"

static char addr,buff[2];

void BHinit(bool ADDRstat) {
    addr = ADDRstat?BH1750_ADDR_H:BH1750_ADDR_L;
}

bool BHwrite (char command) {
    buff[0]=command;
    return I2Cwrite (addr, buff, 1);
}

bool BHread (unsigned short *result) {
    if (I2Cread (addr, buff, 2)) {
        *result = (unsigned short)buff[0]<<8 | (unsigned short)buff[1];
        return true;
    }
    return false;
}
