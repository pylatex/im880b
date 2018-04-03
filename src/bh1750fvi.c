#include "i2c.h"
#include "bh1750fvi.h"
#include <xc.h>

static struct {
    char    addr;
} conf;

void BHinit(bool ADDRpinstatus) {
    conf.addr = ADDRpinstatus ? BH1750_ADDR_H : BH1750_ADDR_L ;
}

bool BHread (unsigned short *result) {
}