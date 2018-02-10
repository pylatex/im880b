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

    #define IAQ_DEFADDR 0x5A

    // Decomposition of the answer given by the iAQ-CORE sensor
    typedef union {                    // I2C octect position
        struct {
            union {
                struct {
                    unsigned char CO2P_0;   // B1
                    unsigned char CO2P_1;   // B0
                };
                unsigned short pred;    //Prediction Value of CO2 in ppm
            };
            enum {
                OK      = 0x00, //Data valid
                RUNIN   = 0x10, //Module in warm up phase
                BUSY    = 0x01, //re-read multi byte data
                ERROR   = 0x80  //If constant: replace sensor
            } status;                       // B2
            union {
                struct {
                    unsigned char RES_0;    // B6
                    unsigned char RES_1;    // B5
                    unsigned char RES_2;    // B4
                    unsigned char RES_3;    // B3
                };
                unsigned long resistance;
            };
            union {
                struct {
                    unsigned char TVOCe_0;  // B8
                    unsigned char TVOCe_1;  // B7
                };
                unsigned short tvoc;
            };
        };
        unsigned char raw[9];
    } IAQ_T;

    /**
     * Performs the iAQ sensor reading
     * @return a pointer to an object with the measures, on a sucessful reading
     */
    IAQ_T *iaq_read(void);

#ifdef	__cplusplus
}
#endif

#endif	/* IAQ_H */
