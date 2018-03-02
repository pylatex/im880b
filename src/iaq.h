/*
 * File:   iaq_core.h
 * Author: alex
 *
 * Created on 8 de febrero de 2018, 20:29
 */

#ifndef IAQ_H
#define	IAQ_H

#ifdef	__cplusplus
extern "C" {
#endif
    #include <stdbool.h>

    #define IAQ_DEFADDR 0x5A    //Default I2C Address

    #define IAQ_OK      0x00    //Data valid
    #define IAQ_RUNIN   0x10    //Module in warm up phase
    #define IAQ_BUSY    0x01    //re-read multi byte data
    #define IAQ_ERROR   0x80    //If constant: replace sensor

    // Decomposition of the answer given by the iAQ-CORE sensor
    typedef volatile union {
        struct {
            unsigned short pred;        //Prediction Value of CO2 in ppm
            unsigned char status;
            unsigned long resistance;   //Internal Resistance in Ohms
            unsigned short tvoc;        //TVOC in ppb
        };
        unsigned char raw[9];
    } IAQ_T;

    /**
     * Performs the iAQ sensor reading
     * @return a pointer to an object with the measures, on a sucessful reading
     */
    bool iaq_read(IAQ_T *obj);

#ifdef	__cplusplus
}
#endif

#endif	/* IAQ_H */
