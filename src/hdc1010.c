/*
 * Implementacion de la libreria para el control del sensor HDC1010
 */

#include "hdc1010.h"
#include "i2c.h"

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

static I2C_MESSAGE_STATUS   status;
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

    unsigned char       attempts,Buffer[3],len;

    Buffer[0]=addr;
    len=1;
    if (val) {
        Buffer[1]=(unsigned char)(*val >> 8);
        Buffer[2]=(unsigned char)(*val);
        len = 3;
    }

    // Now it is possible that the slave device will be slow.
    // As a work around on these slaves, the application can
    // retry sending the transaction
    for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++)
    {
        // Define the register to be read from sensor
        I2C_MasterWrite(Buffer, len, HDC_I2C_BASE_ADDR + HDC_ADDR_PINS, &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C_MESSAGE_PENDING);

        if (status == I2C_MESSAGE_COMPLETE)
        {
            if (addr == HDC_TEMPERATURE || addr == HDC_HUMIDITY)
                HDC.delay(7);
            return true;
        }
        // if status is  I2C_MESSAGE_ADDRESS_NO_ACK,
        //               or I2C_DATA_NO_ACK,
        // The device may be busy and needs more time for the last
        // write so we can retry writing the data, this is why we
        // use a while loop here
    }
    return false;
}

static unsigned char *HDC_ReadReg (unsigned char addr,unsigned char len)
{
    static unsigned char    Buffer[4];
    unsigned char           attempts;

    if (len < 4 && HDC_WriteReg(addr,0))    //At first, write (in pointer) the address to be read. If success:
    {

        // this portion will read the byte(s) from the memory location.
        for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++) {
            // write one byte to EEPROM (2 is the count of bytes to write)
            I2C_MasterRead(Buffer, len, HDC_I2C_BASE_ADDR + HDC_ADDR_PINS, &status);

            // wait for the message to be sent or status has changed.
            while(status == I2C_MESSAGE_PENDING);

            if (status == I2C_MESSAGE_COMPLETE) {
                return Buffer;
            }

            // if status is  I2C_MESSAGE_ADDRESS_NO_ACK,
            //               or I2C_DATA_NO_ACK,
            // The device may be busy and needs more time for the last
            // write so we can retry writing the data, this is why we
            // use a while loop here

        }
    }

    return 0;   //null pointer
}
