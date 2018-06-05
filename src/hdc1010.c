/*
 * Implementacion de la libreria para el control del sensor HDC1010
 */

#include "hdc1010.h"
#include "I2Cgeneric.h"

typedef union {
    unsigned short reg;
    struct {
        unsigned        :8;
        unsigned HRES   :2; //8..9
        unsigned TRES   :1; //10
        unsigned BTST   :1; //11
        unsigned MODE   :1; //12
        unsigned HEAT   :1; //13
        unsigned        :1;
        unsigned RST    :1; //14
    };
} HDC_conf_t;

static struct {
    HDC_conf_t      conf;
    unsigned short *temp;   //Variable (address) specified by user, for temperature readings
    unsigned short *hum;    //Variable (address) specified by user, for humidity readings
    DelayFunction   delay;
} HDC;

static void HDCreadConfig (void);
static bool HDCwriteConfig (void);
static void checkMode (const unsigned mode);
static unsigned short ExtractUshort (unsigned char *buff);
static unsigned char *HDC_ReadReg (unsigned char addr,unsigned char len);
static bool HDC_WriteReg (unsigned char addr, unsigned short *val);

#define I2C_MAX_ATTEMPTS    200
#define HDC_ADDR_PINS       0   //External Pin Setup
#define checkSingle()       checkMode(HDC_MODE_SINGLE)
#define checkBoth()         checkMode(HDC_MODE_BOTH)

void HDCinit (unsigned short *tempAdd,unsigned short *humAdd,DelayFunction msDelayHandler) {
    HDC.temp=tempAdd;
    HDC.hum=humAdd;
    HDCreadConfig();
    HDC.delay=msDelayHandler;
}

bool HDCtemp () {
    checkSingle();

    unsigned char *ptr=HDC_ReadReg(HDC_TEMPERATURE,2);
    if (ptr) {
        *HDC.temp = ExtractUshort(ptr);
        return true;
    }

    return false;
}

bool HDChumidity () {
    checkSingle();

    unsigned char *ptr=HDC_ReadReg(HDC_HUMIDITY,2);
    if (ptr) {
        *HDC.hum = ExtractUshort(ptr);
        return true;
    }

    return false;
}

bool HDCboth () {
    checkBoth();

    unsigned char *ptr=HDC_ReadReg(HDC_TEMPERATURE,4);
    if (ptr) {
        *HDC.temp = ExtractUshort(ptr);
        ptr += 2;
        *HDC.hum = ExtractUshort(ptr);
        return true;
    }

    return false;
}

unsigned HDCgetTempResolution (void) {
    return HDC.conf.TRES;
}

void HDCsetTempResolution (unsigned const TempResolution) {
    HDC.conf.TRES=TempResolution & 0x01;
    HDCwriteConfig();
}

unsigned HDCgetHumidityResolution (void) {
    return HDC.conf.HRES;
}

void HDCsetHumidityResolution (unsigned const HumidityResolution) {
    HDC.conf.HRES=HumidityResolution & 0x03;
    HDCwriteConfig();
}

static void HDCreadConfig (void) {
    unsigned char *buff=HDC_ReadReg(HDC_CONFIG,2);
    if (buff)
        HDC.conf.reg=ExtractUshort(buff);
}

static bool HDCwriteConfig (void) {
    HDC.conf.reg &= 0xBFF0;    //Previous cleanup of forbidden bits
    return HDC_WriteReg(HDC_CONFIG,&HDC.conf.reg);
}

// OK
static void checkMode (const unsigned mode) {
    if (mode <= 1 && HDC.conf.MODE != mode) {
        HDC.conf.MODE = mode;
        HDCwriteConfig();
    }
}

// OK
static unsigned short ExtractUshort (unsigned char *buff) {
    return (unsigned short )(buff[0]<<8 | buff[1]);
}

/**
 * Updates the HDC1010 pointer address and (if not null pointer in val) writes a value in it
 * @param addr
 * @param val
 * @return success or not
 */
static bool HDC_WriteReg (unsigned char addr, unsigned short *val)
{
    if (addr >= 0x03 && addr <= 0xFA)
        return false; //Forbidden Addresses

    unsigned char Buffer[3],len=1;

    Buffer[0]=addr;
    if (val) {
        Buffer[1]=(unsigned char)(*val >> 8);
        Buffer[2]=(unsigned char)(*val);
        len = 3;
    }

    return I2Cwrite(HDC_I2C_BASE_ADDR + HDC_ADDR_PINS, Buffer, len);
    /* TODO: Consider this externally:
                if (addr == HDC_TEMPERATURE || addr == HDC_HUMIDITY)
                HDC.delay(7);
    // */
}

static unsigned char *HDC_ReadReg (unsigned char addr,unsigned char len)
{
    static unsigned char    Buffer[4];

    if (len < 4 && HDC_WriteReg(addr,0))    //At first, write (in pointer) the address to be read. If success:
        if (I2Cread(HDC_I2C_BASE_ADDR + HDC_ADDR_PINS, Buffer, len))
            return Buffer;

    return 0;   //null pointer
}
