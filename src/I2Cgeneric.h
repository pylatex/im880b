/* 
 * File:   I2Cgeneric.h
 * Author: alex
 *
 * Header for a generic I2C C implementation. May be intended as a wrapper for
 * the platform generated libraries, to allow portability of source code,
 * specially when intended to program libraries.
 *
 * Created on 3 de junio de 2018, 0:10
 */

#ifndef I2CGENERIC_H
#define	I2CGENERIC_H

#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    bool I2Cread (uint16_t addr, uint8_t *buffer, uint8_t size);

    bool I2Cwrite (uint16_t addr, uint8_t *buffer, uint8_t size);

#ifdef	__cplusplus
}
#endif

#endif	/* I2CGENERIC_H */
