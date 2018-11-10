/**
  MSSP Generated Driver File
  @Company
    Microchip Technology Inc.
  @File Name
    spi.c
  @Summary
    This is the generated driver implementation file for the MSSP driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs 
  @Description
    This source file provides APIs for MSSP.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs  - 1.55
        Device            :  PIC12LF1840
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "spi.h"

/**
  Section: Macro Declarations
*/

#define SPI_RX_IN_PROGRESS 0x0

typedef struct { uint8_t con1; uint8_t stat; uint8_t add; } spi_configuration_t;
static const spi_configuration_t spi_configuration[] = {   
    {0x0, 0x80, 0x4f }
};

/**
  Section: Module APIs
*/

void SPI_Initialize(void)
{   
    // R_nW write_noTX; P stopbit_notdetected; S startbit_notdetected; BF RCinprocess_TXcomplete; SMP Middle; UA dontupdate; CKE Idle to Active; D_nA lastbyte_address; 
    SSP1STAT = 0x00;
    // SSPEN enabled; WCOL no_collision; CKP Idle:Low, Active:High; SSPM FOSC/16; SSPOV no_overflow; 
    SSP1CON1 = 0x21;
    SSPCON1 = (SSPCON1 & 0xF0) | 1;
    SSPCON1bits.SSPEN=1;   // Enable SPI port
    SSP1CON2 = 0x00;
    
    SSPCON1bits.CKP=0; //Polaridad modo 0
    SSPSTATbits.CKE=1;  // Flancos modo 0
    SSPSTATbits.SMP=0;//medio ciclo
    
    // SSPADD 0; 
    SSP1ADD = 0x00;  
    //setup PPS pins
    SSPCLKPPS = 12;
    SSPDATPPS = 13;
    TRISBbits.TRISB6=0; TRISBbits.TRISB5=1; // SDO, out, SDI in
    
    TRISBbits.TRISB4 = 0; // RB4 Salida SCK out
    
    //TRISCbits.TRISC0=true;// RC0 input IRQ
    TRISCbits.TRISC2=false;//RC2 output RST
    TRISCbits.TRISC1=false;//RC1 output SS    
    SSPDATPPS = 0x0D;   //RB5->MSSP:SDI;    
		
    RB6PPS = 0x14;   //RB6->MSSP:SDO;    
		
    RB4PPS = 0x12;   //RB4->MSSP:SCK;    
		
    SSPCLKPPS = 0x0C;   //RB4->MSSP:SCK;
    
        
}

uint8_t SPI_Exchange8bit(uint8_t data)
{
    // Clear the Write Collision flag, to allow writing
    SSP1CON1bits.WCOL = 0;

    SSPBUF = data;

    while(SSP1STATbits.BF == SPI_RX_IN_PROGRESS)
    {
    }

    return (SSPBUF);
}
/*
uint8_t SPI_Exchange8bitBuffer(uint8_t *dataIn, uint8_t bufLen, uint8_t *dataOut)
{
    uint8_t bytesWritten = 0;

    if(bufLen != 0)
    {
        if(dataIn != NULL)
        {
            while(bytesWritten < bufLen)
            {
                if(dataOut == NULL)
                {
                    SPI_Exchange8bit(dataIn[bytesWritten]);
                }
                else
                {
                    dataOut[bytesWritten] = SPI_Exchange8bit(dataIn[bytesWritten]);
                }

                bytesWritten++;
            }
        }
        else
        {
            if(dataOut != NULL)
            {
                while(bytesWritten < bufLen )
                {
                    dataOut[bytesWritten] = SPI_Exchange8bit(DUMMY_DATA);

                    bytesWritten++;
                }
            }
        }
    }

    return bytesWritten;
}
*/

bool SPI_IsBufferFull(void)
{
    return (SSP1STATbits.BF);
}

bool SPI_HasWriteCollisionOccured(void)
{
    return (SSP1CON1bits.WCOL);
}

void SPI_ClearWriteCollisionStatus(void)
{
    SSP1CON1bits.WCOL = 0;
}
/**
 End of File
*/