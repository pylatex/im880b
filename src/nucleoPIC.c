#include "nucleoPIC.h"
#include <string.h>
#include <stdio.h>
#include "SerialDevice.h"

void setup (void) {

    //OSCILLATOR
#ifdef _18F2550
    OSCCON=0x73;    //Internal at 8 MHz
    while(!IOFS);   //Waits for stabilization
#endif
#ifdef _16F1769
    OSCCON=0x70;    //Internal at 8 MHz
    while(!HFIOFS); //May be either PLLR or HFIOFS. See OSCSTAT register for specific case
#endif

    //PINS SETUP
#ifdef _16F1769
    
    ANSELA=0;       //All pins as digital
    ANSELB=0;       //All pins as digital
    ANSELC=0;       //All pins as digital
    TRISAbits.TRISA1=false;//LED RA1 output
    ODCONCbits.ODC1 = true;    //Enable Open Drain to drive DVI
    // WDTPS 1:65536; SWDTEN OFF; 
    WDTCON = 0x16;
    
#endif
#ifdef _18F2550
    PORTA=0;
    LATA=0;
    ADCON1=0x0E;
    ADCON2=0x83;
    TRISA=0xFD; //RA1 as output
#endif

    //UART 
#ifdef _16F1769
    RB6PPS=0x12;
    RB4PPS=0x13;
    ppsLock(true);    
#endif

    __delay_ms(200);   //Delay for stabilization of power supply
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
