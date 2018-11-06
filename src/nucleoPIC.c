#include "nucleoPIC.h"
#include <string.h>
#include <stdio.h>
#include "SerialDevice.h"

void setup (void) {

    //OSCILLATOR
    OSCCON=0x70;    //Internal at 8 MHz
    while(!HFIOFS); //May be either PLLR or HFIOFS. See OSCSTAT register for specific case

    //PINS SETUP
    //PORTC=0;
    //LATC=0;
    ANSELA=0;       //All pins as digital
    ANSELB=0;       //All pins as digital
    ANSELC=0;       //All pins as digital
    ODCONCbits.ODC1 = true;    //Enable Open Drain to drive DVI
    TRISC=0x7C; //RC0/1/7 as outputs

    OPTION_REGbits.nWPUEN=0;
    WPUBbits.WPUB4=1;
    WPUBbits.WPUB6=1;

    //UART & I2C

    //Entradas y salidas I2C se dejan en los pines por defecto: SCK:RB6, SDA:RB4
    //(unicos totalmente compatibles con I2C/SMBus, segun seccion 12.3 del datasheet)
    RB6PPS=0x12;
    RB4PPS=0x13;
    SSPDATPPS = 0x0C;   //RB4->MSSP:SDA;
    SSPCLKPPS = 0x0E;   //RB6->MSSP:SCL;

    ppsLock(true);

    __delay_ms(200);   //Delay for stabilization of power supply

    #ifdef _I2C_H
    I2C_Initialize();
    #endif
}

#ifdef PPSLOCK
void ppsLock (bool lock){
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = lock?1u:0u; // lock PPS
}
#endif

void enableInterrupts (void) {
    CCP1IE = true; //Comparison, for timeouts.
    PEIE = true; //Peripheral Interrupts Enable
    GIE = true; //Global Interrupts Enable
}
