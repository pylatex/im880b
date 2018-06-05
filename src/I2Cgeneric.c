/*
 * Implementacion para PIC de la libreria generica para el control de I2C
 */

#include <stdbool.h>
#include "I2Cgeneric.h"
#include "i2c.h"

#define I2C_MAX_ATTEMPTS    30

static bool ready = false;

void checkReady(void);

bool I2Cread (uint16_t addr, uint8_t *buffer, uint8_t size){
    I2C_MESSAGE_STATUS   status;
    unsigned char        attempts;

    checkReady();

    // this portion will read the byte from the memory location.
    for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++) {
        // write one byte to EEPROM (2 is the count of bytes to write)
        I2C_MasterRead(buffer, size, addr, &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C_MESSAGE_PENDING);

        if (status == I2C_MESSAGE_COMPLETE)
            return true;
    }
    return false;
}

bool I2Cwrite (uint16_t addr, uint8_t *buffer, uint8_t size){
    I2C_MESSAGE_STATUS   status;
    unsigned char        attempts;
    
    checkReady();

    for (attempts = 0;(status != I2C_MESSAGE_FAIL) && (attempts<I2C_MAX_ATTEMPTS);attempts++)
    {
        // Define the register to be read from sensor
        I2C_MasterWrite(buffer, size, addr, &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C_MESSAGE_PENDING);

        if (status == I2C_MESSAGE_COMPLETE)
            return true;
    }
    return false;
}

void checkReady(void) {
    if (!ready) {
        I2C_Initialize();
        ready = true;
    }
}