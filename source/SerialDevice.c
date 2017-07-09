//------------------------------------------------------------------------------
//
//	File:		SerialDevice.cpp
//	Abstract:	Serial Device Abstraction
//	Version:	0.1
//	Date:		18.05.2016
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//  Realizadas modificaciones adicionales a este fichero para permitir una
//  implementacion mas independiente de la plataforma.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
#include <stdbool.h>
#include "globaldefs.h"
#include "SerialDevice.h"

#ifdef Q_OS_WIN
#include <windows.h>
#define Baudrate_9600       9600
#define Baudrate_115200     115200
#define DataBits_7          7
#define DataBits_8          8
#define Parity_Even         EVENPARITY
#define Parity_None         NOPARITY
#endif
#ifdef UC_PIC8
#include <xc.h>
#endif

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
 * 
 * Configura e Inicia el modulo/puerto serie.
 */
bool
SerialDevice_Open(UINT8         comNumber,
                  int           dataBits,
                  UINT8         parity)
#ifdef Q_OS_WIN
{
    //POR HACER: el primer argumento ya no es un string, hay que armarlo aca
    //si se requiere.

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
    // error
    return false;
}
#endif
#ifdef UC_PIC8
{
    //UART, Ajustes comunes a Rx y Tx. Inicializado de acuerdo a datasheet 16F2550
    //Se prueba con 8(interno) y 7.3728(externo) MHz
    ////1: (En reset,SPBRG=0). Usar BRG16=1 y BRGH=1. Velocidades despues de PLL (si lo hay)
    SPBRG=16;   //Fosc=8 MHz (ejm,interno)
    //SPBRG=15;   //Fosc=7.3728 MHz (externo)
    SYNC=0; //2. Modo Asincrono
    SPEN=1; //2. Habilita Puerto Serie
    TXEN=1; //6,Tx. Habilita transmisor
    
    return true;
}
#endif

/**
 * Close
 * @brief: close serial device
 */
bool
SerialDevice_Close()
#ifdef Q_OS_WIN
{
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
    return false;
}
#endif
#ifdef UC_PIC8
{
    // TODO : add your own platform specific code here
    return false;
}
#endif

/**
 * SendData
 * @brief: send data
 */
int
SerialDevice_SendData(UINT8* txBuffer, int txLength)
#ifdef Q_OS_WIN
{
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
}
#endif
#ifdef UC_PIC8
{
    // TODO : add your own platform specific code here

    // error
    return -1;
}
#endif

/**
 * SendByte
 * @brief: send single byte
 */
int
SerialDevice_SendByte(UINT8 txByte)
#ifdef Q_OS_WIN
{
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
    // error
    return -1;
}
#endif
#ifdef UC_PIC8
{
    // Todo : add your own platform specific code here
    // error
    return -1;
}
#endif

/**
 * ReadData
 * @brief: read data
 */
int
SerialDevice_ReadData(UINT8* rxBuffer, int rxBufferSize)
#ifdef  Q_OS_WIN
{
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
    // error
    return -1;
}
#endif
#ifdef UC_PIC8
{
    // Todo : add your own platform specific code here
}
#endif
//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
