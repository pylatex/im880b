#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "spi.h"

#pragma warning disable 520        

inline void spi_close(void)
{
    SSP1CON1bits.SSPEN = 0;
}


//Setup SPI for Master Mode
void spi_master_open(void){
        TRISAbits.TRISA2 = 0;
        TRISCbits.TRISC5 = 0;
        TRISCbits.TRISC1 = 1;
        //setup PPS pins
        SSPDATPPS = 0x11;   //RC1->MSSP:SDI;    
        RA2PPS = 0x10;   //RA2->MSSP:SCK;    
        RC5PPS = 0x12;   //RC5->MSSP:SDO;    
        SSPCLKPPS = 0x02;   //RA2->MSSP:SCK;      
        //setup SPI        
        SSPSTATbits.CKE=true; // Flancos modo 0
        SSPSTATbits.SMP=false; //datos de entrada disponible en el medio del ciclo (SMP=0)
        SSPCON1 = (SSPCON1 & 0xF0) | 0; // Sets clock frequency for SPI in master mode // clock =3 (TMR2/2) =2 (Fosc/64) =1 (Fosc/16)  =0 (Fosc/4)
        SSPCON1bits.CKP=false; //Polaridad modo 0 Polaridad del reloj
        SSPCON1bits.SSPEN=true;   // Enable SPI puerto serie sincrono
        SSP1CON2 = 0x00;
        SSP1ADD  = 0x4f;
        TRISCbits.TRISC2=false;//RC2 output RST
        TRISCbits.TRISC0=false;//RC0 output SS   
}

// Full Duplex SPI Functions
unsigned char spi_exchangeByte(unsigned char b)
{
    SSP1BUF = b;
    //while(!PIR1bits.SSP1IF);
    while(SSP1STATbits.BF == 0);
    PIR1bits.SSP1IF = 0;
    return SSP1BUF;
}

void spi_exchangeBlock(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        *data = spi_exchangeByte(*data );
        data++;
    }
}

// Half Duplex SPI Functions
void spi_writeBlock(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        spi_exchangeByte(*data++);
    }
}
void spi_readBlock(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        *data++ = spi_exchangeByte(0);
    }
}
