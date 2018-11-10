/**
  MSSP Generated Driver API Header File
  @Company
    Microchip Technology Inc.
  @File Name
    mssp.h
  @Summary
    This is the generated header file for the MSSP driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs 
  @Description
    This header file provides APIs for MSSP.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs  - 1.55
        Device            :  PIC12LF1840
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40
*******************************************************************************/

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

#ifndef _SPI_H
#define _SPI_H

/**
  Section: Included Files
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

/**
  Section: Macro Declarations
*/

#define DUMMY_DATA 0x0

#define SCK_TRIS                 TRISBbits.TRISB4
#define SCK_LAT                  LATBbits.LATB4
#define SCK_PORT                 PORTBbits.RB4
#define SCK_WPU                  WPUBbits.WPUB4
#define SCK_OD                   ODCONBbits.ODB4
#define SCK_ANS                  ANSELBbits.ANSB4
#define SCK_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define SCK_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define SCK_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define SCK_GetValue()           PORTBbits.RB4
#define SCK_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define SCK_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define SCK_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define SCK_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define SCK_SetPushPull()        do { ODCONBbits.ODB4 = 0; } while(0)
#define SCK_SetOpenDrain()       do { ODCONBbits.ODB4 = 1; } while(0)
#define SCK_SetAnalogMode()      do { ANSELBbits.ANSB4 = 1; } while(0)
#define SCK_SetDigitalMode()     do { ANSELBbits.ANSB4 = 0; } while(0)

// get/set SDI aliases
#define SDI_TRIS                 TRISBbits.TRISB5
#define SDI_LAT                  LATBbits.LATB5
#define SDI_PORT                 PORTBbits.RB5
#define SDI_WPU                  WPUBbits.WPUB5
#define SDI_OD                   ODCONBbits.ODB5
#define SDI_ANS                  ANSELBbits.ANSB5
#define SDI_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define SDI_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define SDI_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define SDI_GetValue()           PORTBbits.RB5
#define SDI_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define SDI_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define SDI_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define SDI_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define SDI_SetPushPull()        do { ODCONBbits.ODB5 = 0; } while(0)
#define SDI_SetOpenDrain()       do { ODCONBbits.ODB5 = 1; } while(0)
#define SDI_SetAnalogMode()      do { ANSELBbits.ANSB5 = 1; } while(0)
#define SDI_SetDigitalMode()     do { ANSELBbits.ANSB5 = 0; } while(0)

// get/set SDO aliases
#define SDO_TRIS                 TRISBbits.TRISB6
#define SDO_LAT                  LATBbits.LATB6
#define SDO_PORT                 PORTBbits.RB6
#define SDO_WPU                  WPUBbits.WPUB6
#define SDO_OD                   ODCONBbits.ODB6
#define SDO_ANS                  ANSELBbits.ANSB6
#define SDO_SetHigh()            do { LATBbits.LATB6 = 1; } while(0)
#define SDO_SetLow()             do { LATBbits.LATB6 = 0; } while(0)
#define SDO_Toggle()             do { LATBbits.LATB6 = ~LATBbits.LATB6; } while(0)
#define SDO_GetValue()           PORTBbits.RB6
#define SDO_SetDigitalInput()    do { TRISBbits.TRISB6 = 1; } while(0)
#define SDO_SetDigitalOutput()   do { TRISBbits.TRISB6 = 0; } while(0)
#define SDO_SetPullup()          do { WPUBbits.WPUB6 = 1; } while(0)
#define SDO_ResetPullup()        do { WPUBbits.WPUB6 = 0; } while(0)
#define SDO_SetPushPull()        do { ODCONBbits.ODB6 = 0; } while(0)
#define SDO_SetOpenDrain()       do { ODCONBbits.ODB6 = 1; } while(0)
#define SDO_SetAnalogMode()      do { ANSELBbits.ANSB6 = 1; } while(0)
#define SDO_SetDigitalMode()     do { ANSELBbits.ANSB6 = 0; } while(0)

#define RST LATC2 // Pin RST C2
#define SS  LATC1 // Pin RST C1        
        
/**
  Section: SPI Module APIs
*/

/**
  @Summary
    Initializes the SPI
  @Description
    This routine initializes the SPI.
    This routine must be called before any other MSSP routine is called.
    This routine should only be called once during system initialization.
  @Preconditions
    None
  @Param
    None
  @Returns
    None
  @Comment
    
  @Example
    <code>
    uint8_t     myWriteBuffer[MY_BUFFER_SIZE];
    uint8_t     myReadBuffer[MY_BUFFER_SIZE];
    uint8_t     writeData;
    uint8_t     readData;
    uint8_t     total;
    SPI_Initialize();
    total = 0;
    do
    {
        total = SPI_Exchange8bitBuffer(&myWriteBuffer[total], MY_BUFFER_SIZE - total, &myWriteBuffer[total]);
        // Do something else...
    } while(total < MY_BUFFER_SIZE);
    readData = SPI_Exchange8bit(writeData);
    </code>
 */
void SPI_Initialize(void);

/**
  @Summary
    Exchanges a data byte over SPI
  @Description
    This routine exchanges a data byte over SPI bus.
    This is a blocking routine.
  @Preconditions
    The SPI_Initialize() routine should be called
    prior to use this routine.
  @Param
    data - data byte to be transmitted over SPI bus
  @Returns
    The received byte over SPI bus
  @Example
    <code>
    uint8_t     writeData;
    uint8_t     readData;
    uint8_t     readDummy;
    SPI_Initialize();
    // for transmission over SPI bus
    readDummy = SPI_Exchange8bit(writeData);
    // for reception over SPI bus
    readData = SPI_Exchange8bit(DUMMY_DATA);
    </code>
 */
uint8_t SPI_Exchange8bit(uint8_t data);

 /**
  @Summary
    Exchanges buffer of data over SPI
  @Description
    This routine exchanges buffer of data (of size one byte) over SPI bus.
    This is a blocking routine.
  @Preconditions
    The SPI_Initialize() routine should be called
    prior to use this routine.
  @Param
    dataIn  - Buffer of data to be transmitted over SPI.
    bufLen  - Number of bytes to be exchanged.
    dataOut - Buffer of data to be received over SPI.
  @Returns
    Number of bytes exchanged over SPI.
  @Example
    <code>
    uint8_t     myWriteBuffer[MY_BUFFER_SIZE];
    uint8_t     myReadBuffer[MY_BUFFER_SIZE];
    uint8_t     total;
    SPI_Initialize();
    total = 0;
    do
    {
        total = SPI_Exchange8bitBuffer(&myWriteBuffer[total], MY_BUFFER_SIZE - total, &myWriteBuffer[total]);
        // Do something else...
    } while(total < MY_BUFFER_SIZE);
    </code>
 uint8_t SPI_Exchange8bitBuffer(uint8_t *dataIn, uint8_t bufLen, uint8_t *dataOut);
  */


/**
  @Summary
    Gets the SPI buffer full status
  @Description
    This routine gets the SPI buffer full status
  @Preconditions
    The SPI_Initialize() routine should be called
    prior to use this routine.
  @Param
    None
  @Returns
    true  - if the buffer is full
    false - if the buffer is not full.
  @Example
    Refer to SPI_Initialize() for an example
 */
bool SPI_IsBufferFull(void);

/**
  @Summary
    Gets the status of write collision.
  @Description
    This routine gets the status of write collision.
  @Preconditions
    The SPI_Initialize() routine must have been called prior to use this routine.
  @Param
    None
  @Returns
    true  - if the write collision has occurred.
    false - if the write collision has not occurred.
  @Example
    if(SPI_HasWriteCollisionOccured())
    {
        SPI_ClearWriteCollisionStatus();
    }
*/
bool SPI_HasWriteCollisionOccured(void);

/**
  @Summary
    Clears the status of write collision.
  @Description
    This routine clears the status of write collision.
  @Preconditions
    The SPI_Initialize() routine must have been called prior to use this routine.
  @Param
    None
  @Returns
    None
  @Example
    if(SPI_HasWriteCollisionOccured())
    {
        SPI_ClearWriteCollisionStatus();
    }
*/
void SPI_ClearWriteCollisionStatus(void);

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif // _SPI_H
/**
 End of File
*/
