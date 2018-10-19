/* 
 * File:   nmeaCayenne.h
 * Author: alex
 *
 * Created on 8 de octubre de 2018, 9:59
 */

#ifndef NMEACAYENNE_H
#define	NMEACAYENNE_H

#include <stdbool.h>
#include <stdint.h>

#ifndef int24_t
#define int24_t int32_t
#endif

#ifdef	__cplusplus
extern "C" {
#endif
    
    typedef struct {
        int24_t latitude;
        int24_t longitude;
        int24_t height;
    } NMEAdata_t;

    /**
     * Set up the NMEA to Cayenne converter
     * @param NCobjPtr: An object for the received NMEA data.
     */
    void initNC(NMEAdata_t *NCobjPtr);

    /**
     * You can use this function to avoid unnecesary readings of your NCobjPtr
     * @return true if a new GPS update have been received.
     */
    bool NCupdated(void);

    /**
     * Must be called to process every char in the Rx pin
     * @param rxByte
     */
    void NCinputSerial(char rxByte);

#ifdef	__cplusplus
}
#endif

#endif	/* NMEACAYENNE_H */

