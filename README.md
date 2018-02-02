# iM880B

Multiplatform HCI stack Implementation, to command the IMST's WiMOD iM880B-L LoRaWAN module. Based initialy on the [HCI LoRaWAN example code from IMST](https://wireless-solutions.de/products/radiomodules/im880b-l.html). Here we list the currently supported platforms and IDEs:

| IDE   | Compiler  | Windows | UX  | 8 bit PIC |
| ---   |   ---     |  ---    | --- |   ---     |
| [Code::Blocks](http://www.codeblocks.org) | GCC/G++ Compiler  | X | X |  |
| [Microchip's MPLAB X IDE](http://www.microchip.com/mplab/mplab-x-ide) |  XC8 Compiler |  |  | X |

# Status of Target Platforms

## PC

For both platforms, an example code can be found in `main_pc.cpp`. Every layer of the HCI stack proposed by IMST has their own buffers and source code (`WiMOD_HCI_Layer.c`, `SLIP.c` and `SerialDevice_pc.c`), so every layer can be debugged individually to understand the inner working concepts of the HCI stack.

* Windows: **Working**.
* Linux: **Pending Work**

  The whole source code compiles, but the code to access the Serial Port have to be reviewed (due to the fact that the original code was intended to work in Windows).

## MCUs

Due to the fact that MCUs has limited resources, the whole HCI stack is programmed in a single `WMLW_API.c` file.

* Microchip PIC, 8 bit family: **Working**.

  Currently Supported Targets:

  * [PIC18F2550](http://www.microchip.com/PIC18F2550)
  * [PIC16F1769](https://www.microchip.com/PIC16F1769/)

  However, the code can be easily ported to another PIC reference with at least a UART module with Rx interrupts.

  The file `main_pic.c` includes a basic setup (oscillator and EUSART) to use the internal oscillator at 8 MHz (Some tunning on register values have to be done to work with other crystal/oscillator configurations), intending to be a state machine in which a LED connected to RA0 blinks once the OTA authentication was successful and then blinks every time a data is being sent.

# Main Functions and Usage

Here are the main functions. Depending on your compiling environment, please review their specific implementations:

* MCUs: `WMLW_API.[c/h]`
* PC: `WiMOD_LoRaWAN_API.[c/h]`

## `hci_stack.h` notable functions:

```C
bool InitHCI (WMHCIuserProc HCIRxHandler, HCIMessage_t *RxMessage)
```

Initializes the whole HCI stack, saving the `HCIRxHandler` function to be called (handler registration) when a decoded HCI message can  be read in the indicated `&RxMessage` HCI reception buffer.

```C
void IncomingHCIpacker (unsigned char rxByte)
```

Call this for every received octect from your serial port implementation. It decodes every octect from the SLIP layer and when detects the message end, calls the `HCIRxHandler` function indicated at initialization.

```C
bool SendHCI (HCIMessage_t *TxMessage)
```

Once the user compiled an `HCIMessage_t` struct (only with Destination ID, Message ID and payload with their respective size), the function generates the CRC, codifies to SLIP and send it through the UART.

# Pending tasks:

* Rewrite the serial library so it works over an arbitrary serial object with their settings, so if there are more than a single serial port, each one can be used arbitrarily.
* Integrate PIC targets with [MCC](http://www.microchip.com/mplab/mplab-code-configurator)
