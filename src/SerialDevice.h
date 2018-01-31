#ifndef SERIAL_DEVICE_H
#define SERIAL_DEVICE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef uint8_t             UINT8;
    typedef uint32_t            UINT32;

    // open serial device
    bool
    SerialDevice_Open(const char   *comPort,
                      UINT32        baudRate,   //115200, siempre.
                      int           dataBits,   //8, siempre.
                      UINT8         parity);    //Ninguna, siempre.

    // close serial device
    bool
    SerialDevice_Close();

    // send single byte
    int
    SerialDevice_SendByte(UINT8 txByte);

    // send data
    int
    SerialDevice_SendData(UINT8    *txBuffer,
                          UINT8     txLength);

    // read data
    int
    SerialDevice_ReadData(UINT8    *rxBuffer,
                          int       rxBufferSize);

#ifdef	__cplusplus
}
#endif

#endif // SERIAL_DEVICE_H
