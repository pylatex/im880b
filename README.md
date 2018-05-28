# iM880B

Multiplatform HCI stack Implementation, to command the IMST's WiMOD iM880B-L LoRaWAN module. Based initialy on the [HCI LoRaWAN example code from IMST](https://wireless-solutions.de/products/radiomodules/im880b-l.html). Here we list the currently supported platforms and IDEs:

| IDE   | Compiler  | Windows | UX  | 8 bit PIC |
| ---   |   ---     |  ---    | --- |   ---     |
| [Code::Blocks](http://www.codeblocks.org) | GCC/G++ Compiler  | X | X |  |
| [Microchip's MPLAB X IDE](http://www.microchip.com/mplab/mplab-x-ide) |  XC8 Compiler |  |  | X |

# Status of Target Platforms

## PC

For both platforms, an example code can be found in `examples/main_pc.cpp`. Every layer of the HCI stack proposed by IMST has their own buffers and source code (`WiMOD_HCI_Layer.c`, `SLIP.c` and `SerialDevice_pc.c`), so every layer can be debugged individually to understand the inner working concepts of the HCI stack.

* Windows: **In Rework**.
* Linux: **Pending Work**

  The whole source code compiles, but the code to access the Serial Port have to be reviewed (due to the fact that the original code was intended to work in Windows).

## MCUs

Due to the fact that MCUs has limited resources, the whole HCI stack is programmed in a single and compact `WMLW_API.c` file.

* Microchip PIC, 8 bit family: **Working**.

  Currently Supported Targets:

  * [PIC18F2550](http://www.microchip.com/PIC18F2550)
  * [PIC16F1769](https://www.microchip.com/PIC16F1769/)

  However, the code can be easily ported to another PIC reference with at least a UART module with interrupts by Rx complete.

  The file `examples/main_pic.c` includes a basic setup (oscillator and EUSART) to use the internal oscillator at 8 MHz (Some tunning on register values have to be done to work with other crystal/oscillator configurations), intending to be a state machine in which a LED connected to RA0 blinks once the OTA authentication was successful and then blinks every time a data is being sent.

# Main Functions and Usage

Depending on the purpose of the final application, if it's intended to send many sensor measures with a [Cayenne](https://mydevices.com/cayenne/docs/lora/#lora-cayenne-low-power-payload)-like protocol, you must `#include` the header library `"pylatex.h"` at top of your source code. But, if you want to directly send messages over LoRaWAN, as this library does, you have to `#include` the header library `"WiMOD_LoRaWAN_API.h"`. For details of inner working, see the related source codes:

| Header | Source |
|   ---  |  ---   |
| pylatex.h | pylatex.c |
| WiMOD_LoRaWAN_API.h  | WMLW_API.c (for MCU) or WiMOD_LoRaWAN_API.c (for PC) |

## `pylatex.h` notable functions:

```C
void initLW (serialTransmitHandler transmitter)
```

Indicates to `pylatex.h` library the external `transmitter` function, that the library must use to send an array over a previously configured 115200, 8N1 UART port, connected to the iM88xx modem.

```C
void pylatexRx (char RxByteUART)
```

Call this on every received byte, `RxByteUART`, from your serial port implementation.

```C
bool AppendMeasure (char type,char *ptrMeasure)
```

For a specific `type` of measure (let's say, CO2 with `PY_CO2` constant), receives a `ptrMeasure` [pointer](https://boredzo.org/pointers/) to an array of the spliced measure, depending on the weight of the measure, and appends it at the end of the message to be sent.

```C
void SendMeasures (pyModeType confirmed)
```
Once appended all measures, `SendMeasures()` sends all measures to the iM88xx to be sent through LoRaWAN. The `confirmed` variable may be `PY_CONFIRMED` for confirmed messages or `PY_UNCONFIRMED` for unconfirmed messages. This function can be called safely, only after a first invocation of `initLW()` and the calling of the `pylatexRx()` on every incoming byte (See the `main_pic8.c` example).

# Pending tasks:

* Integrate PIC targets with [MCC](http://www.microchip.com/mplab/mplab-code-configurator)
* Add channel support (to identify every measure on a message with multiple measures of same type)