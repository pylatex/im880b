#ifndef SERIAL_DEVICE_H
#define SERIAL_DEVICE_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdint.h>
    #include <stdbool.h>

    typedef enum {
        B0,B9600,B19200,B115200
    } baudrate_t;

    typedef enum {MODEM_LW,GPS,HPM,DEBUG1,DEBUG2} serial_t;

    /**
     * Initializes the library to work with specified serial port
     * @param comPort
     * @param baudRate
     * @param dataBits
     * @param parity
     * @return True on success
     */
    bool
    SerialDevice_Open(const char   *comPort,
                      baudrate_t    baudRate,   //115200, siempre.
                      int           dataBits,   //8, siempre.
                      uint8_t         parity);    //Ninguna, siempre.

    /**
     * Close
     *
     * @Summary
     * close serial device
     *
     * @return
     * True on sucess
     */
    bool
    SerialDevice_Close();

    /**
     * SendByte
     * @brief send single byte
     * @param txByte
     * @return Bytes sent or -1 if communication error
     */
    int
    SerialDevice_SendByte(uint8_t txByte);

    /**
     * SendData
     * @brief: send data
     */
    int
    SerialDevice_SendData(uint8_t    *txBuffer,
                          uint8_t     txLength);

    /**
     * ReadData
     * @brief: read data
     */
    int
    SerialDevice_ReadData(uint8_t    *rxBuffer,
                          int       rxBufferSize);

#ifdef	__cplusplus
}
#endif

#endif // SERIAL_DEVICE_H
