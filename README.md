# im880b-micro

Microcontroller Implementation of the HCI LoRaWAN stack to put in run the iM880B module from IMST. Partially based from the [HCI LoRaWAN example code from IMST](https://wireless-solutions.de/products/radiomodules/im880b-l.html).


### Status:

For the working version in this repo (for PIC Enhanced Midrange), a `ping req` are sent out, and a LED conected to RA0 turns ON during half second if there are a successful HCI message as response of that request (specifically, but not limited to, a `ping rsp`)

### Requirements:

The following table shows the files to be included on your IDE's proyect, depending on the compilation target (preferently defined in `globaldefs.h`).

| File | Q_OS_WIN | UC_PIC8 |
| ---- |  :---:   |  :---:  |
| ***Headers:***    |   |   |
|CRC16.h            | x | x |
|**globaldefs.h**   | x | x |
|lorawan_hci.h      |   | x |
|SerialDevice.h     | x | x |
|SLIP.h             | x |   |
|WiMOD_HCI_Layer.h  | x |   |
|***Source Files:***|   |   |
|CRC16.c            | x | x |
|lorawan_hci.c      |   | x |
|**main.c**         | x | x |
|SerialDevice.c     | x | x |
|SLIP.c             | x |   |
|WiMOD_HCI_Layer.c  | x |   |

#####  Windows (Q_OS_WIN)

Surely not working from this repo at this time, but a working version for Windows can be found also in [our initial repo](https://github.com/pylatesUD/im880b). The goal are to merge both repos in just one, possibly this.

The IDE used was [Code::Blocks](http://www.codeblocks.org), with mingw included, a Windows port that includes `g++` from [GCC](https://gcc.gnu.org/).

##### Enhanced 8 bit PIC family (UC_PIC8)

The PIC used was the [PIC18F2550](http://www.microchip.com/PIC18F2550). From this reference a EUSART module and interruptions by the Rx module was required. Includes the basic setup through the whole code (oscillator and EUSART module register values and steps) to use the internal 8 MHz oscillator.

The compiler used was [XC8](http://www.microchip.com/mplab/compilers), but you can choose any IDE that supports this ANSI C compliant compiler, like [MPLAB X](http://www.microchip.com/mplab/mplab-x-ide) or [Proteus](https://www.labcenter.com/)

### Main Functions and Usage

Please refer to `main.c` file.
 - `bool SendHCI (unsigned char *HCImsg, unsigned char size)`
   *HCImsg* is an array that starts with the Destination Endpoint ID (DstID), then the Message ID and finally the Payload.
   *size* are the quantity of octets in the payload.
   Internally the function calculates the CRC, adds it at the end of the HCI message and then encodes in SLIP format and send through the UART.
 - `signed char ProcessHCI (unsigned char *HCImsg, unsigned char valor)`
   This function will use the `HCImsg` as the reception buffer for an incoming HCI message from the iM880B module, and should be called every time a `valor` byte arrives to the Rx port of the UART. If there is successful HCI message (SLIP decoded + valid CRC16) ready to be readed from `HCImsg`, returns the size of the payload (a number >= 0), else returns -1. This returned value can be used in interruption time to write a flag or something that starts the decoding of the HCI received message, out of the interrupt.
