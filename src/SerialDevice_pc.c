//------------------------------------------------------------------------------
//
// File:        SerialDevice.cpp
// Abstract:    Serial Device Abstraction
// Version:     0.1
// Date:        18.05.2016
// Disclaimer:  This example code is provided by IMST GmbH on an "AS IS" basis
//              without any warranties.
//
// This file was modified to allow as possible a no platform dependant implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------

#include "SerialDevice.h"

#ifdef  Q_OS_WIN
#include <windows.h>
#endif // Q_OS_WIN
#ifdef Q_OS_UX
//#include <stdio.h>    // Standard input/output definitions
#include <string.h>     // String function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
//#include <errno.h>    // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#endif // Q_OS_UX
//------------------------------------------------------------------------------
//  Section RAM
//------------------------------------------------------------------------------

#ifdef Q_OS_WIN

// File Handle
static HANDLE   ComHandle = INVALID_HANDLE_VALUE;

#endif
#ifdef Q_OS_UX

static int fd = -1; // File descriptor for the port

#endif // Q_OS_UX

//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

/**
 * Open
 * @brief: open serial device
 *
 * Configura e Inicia el modulo/puerto serie.
 */
bool
SerialDevice_Open(const char   *comPort,
                  UINT32        baudRate,
                  int           dataBits,
                  UINT8         parity)
{

#ifdef Q_OS_WIN
    // handle valid ?
    if (ComHandle != INVALID_HANDLE_VALUE)
        SerialDevice_Close();

    char devName[80];

    // windows workaround for COM Ports higher than COM9
    strcpy(devName, "\\\\.\\");
    strcat(devName, comPort);


    ComHandle = CreateFileA(devName,
                            GENERIC_WRITE | GENERIC_READ,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            0, //FILE_FLAG_WRITE_THROUGH, //0
                            NULL);

    // handle valid ?
    if (ComHandle != INVALID_HANDLE_VALUE) {
        DCB dcb;
        if (GetCommState(ComHandle, &dcb)) {
            dcb.DCBlength           = sizeof(DCB);
            dcb.BaudRate            = baudRate;
            dcb.ByteSize            = dataBits;
            dcb.Parity              = parity; //EVENPARITY;// NOPARITY;
            dcb.StopBits            = ONESTOPBIT;
            dcb.fOutxCtsFlow        = FALSE;
            dcb.fOutxDsrFlow        = FALSE;
            dcb.fDtrControl         = DTR_CONTROL_DISABLE;
            dcb.fDsrSensitivity     = FALSE;
            dcb.fTXContinueOnXoff   = FALSE;
            dcb.fOutX               = FALSE; // no XON/XOFF
            dcb.fInX                = FALSE; // no XON/XOFF
            dcb.fErrorChar          = FALSE;
            dcb.fNull               = FALSE;
            dcb.fRtsControl         = RTS_CONTROL_DISABLE;
            dcb.fAbortOnError       = FALSE;

            if (SetCommState(ComHandle, &dcb)) {
                COMMTIMEOUTS commTimeouts;
                commTimeouts.ReadIntervalTimeout 		= MAXDWORD;
                commTimeouts.ReadTotalTimeoutMultiplier = 0;
                commTimeouts.ReadTotalTimeoutConstant 	= 0;

                commTimeouts.WriteTotalTimeoutMultiplier = 10;
                commTimeouts.WriteTotalTimeoutConstant  = 1;

                SetCommTimeouts(ComHandle, &commTimeouts);

                // ok
                return true;
            }
        }
        // close device
        SerialDevice_Close();
    }
#endif // Q_OS_WIN
#ifdef Q_OS_UX
    char devName[80];
    struct termios options;

    strcpy(devName, comPort);

    fd = open(devName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        //Could not open the port.
        return false;
    }
    else {
        fcntl(fd, F_SETFL, 0);

        //Get the current options for the port
        tcgetattr(fd, &options);
        //Set baud rates
        cfsetispeed(&options, baudRate);
        cfsetospeed(&options, baudRate);
        options.c_cflag &= ~CSIZE; /* Mask the character size bits */
        options.c_cflag |= dataBits;    /* Select 8 data bits */
        //Enable the receiver and set local mode
        options.c_cflag |= (CLOCAL | CREAD);
        //Set the new options for the port
        tcsetattr(fd, TCSANOW, &options);
    }

    return (fd);
#endif // Q_OS_UX
    // error
    return false;

}

/**
 * Close
 * @brief: close serial device
 */
bool
SerialDevice_Close()
{
#ifdef Q_OS_WIN
    // handle valid ?
    if (ComHandle != INVALID_HANDLE_VALUE) {
        // cancel last operation
        CancelIo(ComHandle);

        // wait 100us
        Sleep(100);

        // close device
        CloseHandle(ComHandle);

        // invalidate handle
        ComHandle = INVALID_HANDLE_VALUE;

        // ok
        return true;
    }
#else
    // Todo : add your own platform specific code here
#endif
    // error
    return false;
}

/**
 * SendData
 * @brief: send data
 */
int
SerialDevice_SendData(UINT8 *txBuffer, UINT8 txLength)
{
#ifdef Q_OS_WIN
    // handle valid ?
    if (ComHandle == INVALID_HANDLE_VALUE)
        return -1;

    UINT32  numTxBytes;

    // write chunk of data
    if (!WriteFile(ComHandle, txBuffer, txLength, (DWORD*)&numTxBytes, 0))
    {
        // error
        return -1;
    }
    // all bytes written ?
    if (numTxBytes == (UINT32)txLength)
    {
        // ok
        return numTxBytes;
    }
#endif // Q_OS_WIN
#ifdef Q_OS_UX
    unsigned int n = write(fd, txBuffer, txLength);
    if (n < 0)
        return -1;
    else
        return n;

#endif // Q_OS_UX
    // error
    return -1;
}

/**
 * SendByte
 * @brief: send single byte
 */
int
SerialDevice_SendByte(UINT8 txByte)
{
#ifdef Q_OS_WIN
    // handle valid ?
    if (ComHandle == INVALID_HANDLE_VALUE)
        return -1;

    UINT32  numTxBytes;

    // write chunk of data
    if (!WriteFile(ComHandle, &txByte, 1, (DWORD*)&numTxBytes, 0))
    {
        // error
        return -1;
    }
    // all bytes written ?
    if (numTxBytes == 1)
    {
        // ok
        return numTxBytes;
    }
#else
    // Todo : add your own platform specific code here
#endif
    // error
    return -1;
}

/**
 * ReadData
 * @brief: read data
 */
int
SerialDevice_ReadData(UINT8 *rxBuffer, int rxBufferSize)
{
#ifdef  Q_OS_WIN
    // handle ok ?
    if (ComHandle == INVALID_HANDLE_VALUE)
        return -1;

    DWORD numRxBytes = 0;

    // read chunk of data
    if (ReadFile(ComHandle, rxBuffer, rxBufferSize, &numRxBytes, 0))
    {
        // return number of bytes read
        return (int)numRxBytes;
    }
#else
    // Todo : add your own platform specific code here
#endif
    // error
    return -1;
}
//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
