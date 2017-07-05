//------------------------------------------------------------------------------
//
//	File:		SerialDevice.cpp
//	Abstract:	Serial Device Abstraction
//	Version:	0.1
//	Date:		18.05.2016
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------

#include "SerialDevice.h"
//------------------------------------------------------------------------------
//  Section RAM
//------------------------------------------------------------------------------

#ifdef Q_OS_WIN

// File Handle
static HANDLE   ComHandle = INVALID_HANDLE_VALUE;

#else
// Todo : add your own platform specific variables here
#endif

//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

/**
 * Open
 * @brief: open serial device
 */
bool
SerialDevice_Open(const char*   comPort,
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
#else
    // Todo : add your own platform specific code here
#endif
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
SerialDevice_SendData(UINT8* txBuffer, int txLength)
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
#else
    // Todo : add your own platform specific code here
#endif
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
SerialDevice_ReadData(UINT8* rxBuffer, int rxBufferSize)
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
