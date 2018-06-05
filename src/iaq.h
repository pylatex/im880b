/*
 * File:   iaq_core.h
 * Author: alex
 *
 * Created on 8 de febrero de 2018, 20:29
 */

#ifndef IAQ_H
#define	IAQ_H

#include <stdbool.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

    #define IAQ_DEFADDR 0x5A    //Default I2C Address

    #define IAQ_OK      0x00    //Data valid
    #define IAQ_RUNIN   0x10    //Module in warm up phase
    #define IAQ_BUSY    0x01    //re-read multi byte data
    #define IAQ_ERROR   0x80    //If constant: replace sensor

    // Decomposition of the answer given by the iAQ-CORE sensor
    typedef union {
        struct {
            uint16_t    pred;       //Prediction Value of CO2 in ppm
            uint8_t     status;
            uint32_t    resistance; //Internal Resistance in Ohms
            uint16_t    tvoc;       //TVOC in ppb
        };
        uint8_t         raw[9];
    } IAQ_T;

    /**
     * Reads the iAQ Core
     * @param obj: A pointer to an IAQ_T object, to write the results
     * @return true if the sensor was read successfully, so the IAQ_T pointed
     * struct has coherent results.
     */
    bool iaq_read(IAQ_T *obj);

#ifdef	__cplusplus
}
#endif

#endif	/* IAQ_H */
