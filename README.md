# im880b-micro

Multiplatform Implementation of the HCI stack to command the WiMOD iM880B-L LoRaWAN module from IMST. Based on the [HCI LoRaWAN example code from IMST](https://wireless-solutions.de/products/radiomodules/im880b-l.html).

# Compiling

The following table shows the files to be included on your IDE's project, depending on the compilation target (preferently defined in `globaldefs.h`). In general, the whole `*.c` files should be included for compilation unless they ends with a platform indicator like `_win` or `_pic8`. all headers, but every `*.c` you compile will include their needed headers.

| File              | Description                     |  WIN  |  PIC  |
| ---               |     ---                         | :---: | :---: |
|CRC16.c            |CRC16 Creation and Checking      |   X   |   X   |
|hci_stack.c        |Small WiMOD HCI Stack implementation |    |   X  |
|main___.c          |                                 | _win  | _pic8 |
|SerialDevice___.c  |UART Interface                   | _win  | _pic8 |
|SLIP.c             |SLIP Encoding and Decoding Layer |   X   |       |
|WiMOD_HCI_Layer.c  |Only the HCI Layer               |   X   |       |
|WiMOD_LoRaWAN_API.cpp|LoRaWAN API Functions          |   X   |       |
|WMLW_API.c         |LoRaWAN API Functions for MCU    |       |   X   |

## Target Platforms

The source code can be compiled from the following platforms:

### Microchip PIC 8 bit family - Working Fine.

The PIC used was the [PIC18F2550](http://www.microchip.com/PIC18F2550), but the code can be easily ported to another PIC that has at least a UART module. From this reference a EUSART module and interruptions by the Rx module from the UART were required. Includes the basic setup through the whole code (oscillator and EUSART module register values and steps) to use the internal 8 MHz oscillator.

For the working version in this repo, a LED connected to RA0 gets permanently on blink if the device were activated successfully.

The compiler used was [XC8](http://www.microchip.com/mplab/compilers), but you can choose any IDE that supports this ANSI C compliant compiler, like [MPLAB X IDE](http://www.microchip.com/mplab/mplab-x-ide) or [Labcenter's Proteus](https://www.labcenter.com/).

### Windows - Not tested, Migrating.

Compiled on Windows 7 with the mingw compiler (a [GCC](https://gcc.gnu.org/) port for Windows) included within [Code::Blocks](http://www.codeblocks.org). Currently the version on this repo was not tested, but a working version (the compiled one that we are talking about) can be found in [our initial repo](https://github.com/pylatesUD/im880b). Honestly at time it has minimal modifications to the original IMST code.

# Main Functions and Usage

Please refer to the `main___.c` file you use.

## (PIC) microcontrollers, use `hci_stack.h`.

 - `bool InitHCI (WMHCIuserProc HCIRxHandler, HCIMessage_t *RxMessage)`

   This function initializes the serial port. Also indicates to the next functions, the procedure `HCIRxHandler` that will handle a decoded HCI message saved in the external `&RxMessage`.

 - `void IncomingHCIpacker (unsigned char rxByte)`

   Call on every incoming octect from your serial port implementation. It decodes every octect from the SLIP layer using the UART and when detects the end of the message, calls the `HCIRxHandler` function from the user, specified at initialization.

 - `bool SendHCI (HCIMessage_t *TxMessage)`

   Once the user compiled an `HCIMessage_t` struct (only with Destination ID, Message ID and payload with their respective size), the function generates the CRC, codifies to SLIP and send it through the UART.

## In Windows, use `WiMOD_LoRaWAN_API.h`

Currently porting this file, has minimal modifications with respect to the IMST sample code.
