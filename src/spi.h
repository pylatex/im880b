
#ifndef __SPI_H
#define __SPI_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define INLINE  inline 
#define RST LATC2 // Pin RST C2
#define SS LATC0 // Pin RST C0  
/* arbitration interface */
INLINE void spi_close(void);

void spi_master_open(void);
/* SPI native data exchange function */
unsigned char spi_exchangeByte(unsigned char b);
/* SPI Block move functions }(future DMA support will be here) */
void spi_exchangeBlock(void *block, size_t blockSize);
void spi_writeBlock(void *block, size_t blockSize);
void spi_readBlock(void *block, size_t blockSize);

#endif // __SPI_DRIVER_H
