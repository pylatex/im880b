#ifndef SERIAL_DEVICE_H
#define SERIAL_DEVICE_H

#include <stdint.h>

typedef uint8_t             UINT8;
typedef uint32_t            UINT32;

// open serial device
bool
SerialDevice_Open(const char   *comPort,
                  UINT32        baudRate,
                  int           dataBits,
                  UINT8         parity);

// close serial device
bool
SerialDevice_Close();

// send single byte
int
SerialDevice_SendByte(UINT8 txByte);

// send data
int
SerialDevice_SendData(UINT8    *txBuffer,
                      int       txLength);

// read data
int
SerialDevice_ReadData(UINT8    *rxBuffer,
                      int       rxBufferSize);


#endif // SERIAL_DEVICE_H
