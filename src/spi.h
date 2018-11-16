
#ifndef __SPI_H
#define __SPI_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "nucleoPIC.h"

#define INLINE  inline 
#define RST LATC3 // Pin RST C3
#define SS  LATC2 // Pin RST C2  
/* arbitration interface */

typedef enum { 
    SPI_CUSTOM0
} spi_modes;


INLINE void spi_close(void);


__bit spi_master_open(spi_modes spiUniqueConfiguration);
__bit spi_slave_open(spi_modes spiUniqueConfiguration);
/* SPI native data exchange function */
uint8_t spi_exchangeByte(uint8_t b);
/* SPI Block move functions }(future DMA support will be here) */
void spi_exchangeBlock(void *block, size_t blockSize);
void spi_writeBlock(void *block, size_t blockSize);
void spi_readBlock(void *block, size_t blockSize);

#endif // __SPI_DRIVER_H
