# im880b-micro

Multiplatform Implementation of the HCI stack to command the WiMOD iM880B-L LoRaWAN module from IMST. Based on the [HCI LoRaWAN example code from IMST](https://wireless-solutions.de/products/radiomodules/im880b-l.html).

# Compiling

The following table shows the files to be included on your IDE's proyect, depending on the compilation target (preferently defined in `globaldefs.h`). In general, the whole `*.c` files should be included for compilation but if some of them ends in `_win` or `_pic8` means for a special implementation for the platform, so just only one of them should be compiled. Every `*.c` included will call their needed headers.

| File              | Description                     |  WIN  |  PIC  |
| ---               |     ---                         | :---: | :---: |
| CRC16.c           |CRC16 Creation and Checking      |   X   |   X   |
|hci_stack.c        |Small WiMOD HCI Stack implementation |    |   X  |
|main___.c          |                                 | _win  | _pic8 |
|SerialDevice___.c  |UART Interface                   | _win  | _pic8 |
|SLIP.c             |SLIP Encoding and Decoding Layer |   X   |       |
|WiMOD_HCI_Layer.c  |Only the HCI Layer               |   X   |       |
|WiMOD_LoRaWAN_API.cpp|LoRaWAN API Functions          |   X   |       |

We're working on a Makefile to avoid the most possible this mishaps...

## Target Platforms

The source code can be compiled from the following platforms:

### Microchip PIC 8 bit family - Working Correctly.

The PIC used was the [PIC18F2550](http://www.microchip.com/PIC18F2550), but the code can be easily ported to another PIC that has at least a UART module. From this reference a EUSART module and interruptions by the Rx module from the UART were required. Includes the basic setup through the whole code (oscillator and EUSART module register values and steps) to use the internal 8 MHz oscillator.

For the working version in this repo, a `ping req` are sent out, and a LED conected to RA0 turns ON during half second if there are a successful HCI message as response of that request (specifically, but not limited to, a `ping rsp`).

The compiler used was [XC8](http://www.microchip.com/mplab/compilers), but you can choose any IDE that supports this ANSI C compliant compiler, like [MPLAB X IDE](http://www.microchip.com/mplab/mplab-x-ide) or [Labcenter's Proteus](https://www.labcenter.com/).

### Windows - Not tested, Migrating.

Compiled on Windows 7 with the mingw compiler (a [GCC](https://gcc.gnu.org/) port for Windows) included within [Code::Blocks](http://www.codeblocks.org). Currently the version on this repo was not tested, but a working version (the compiled one that we are talking about) can be found in [our initial repo](https://github.com/pylatesUD/im880b). Honestly at time it has minimal modifications to the original IMST code.

# Main Functions and Usage

Please refer to `main.c` file.

## For microcontrollers, use `hci_stack.h`.

 - `bool SendHCI (unsigned char *buffer, unsigned char size)`

   The function iterates over the `buffer`, that starts with the Destination Endpoint ID (DstID), followed by the Message ID (MsgID) and finally the Payload with their specified `size`.

   Then it calculates the CRC and adds it at the end of the HCI message and finally encodes in SLIP format as it sends the octets through the UART.

 - `signed char ProcessHCI (unsigned char *buffer, unsigned char rxData)`

   This function will use the `buffer` array for an incoming HCI message from the iM880B module, and should be called every time a `rxData` incoming byte are complete and ready to be read from the UART.

   When a successful HCI message (SLIP decoded + valid CRC16) are ready to be read from `buffer`, the function returns the size of the payload (a number >= 0), otherwise returns -1. This returned value can be used in interruption time to write a flag or something that starts the decoding of the HCI received message, out of the interrupt.

[//]: # (TAREA: cambiar la funcion siguiente, por una que devuelva lo que supuestamente devuelve la anterior)

 - `bool PendingRxHCI(void)`

   This function returns `true` when in `buffer` are an HCI message ready to be read (SLIP decoded + valid CRC16).



## In Windows, use `WiMOD_LoRaWAN_API.h`

Currently porting this file, has minimal modifications with respect to the IMST sample code.
