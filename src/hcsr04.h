/* 
 * File:   hcsr04.h
 * Author: Nicolas Pastran at gmail dot com
 *
 * Created on 1 Noviembre de 2018, 15:49
 */

#ifndef HCSR04_H
#define	HCSR04_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdbool.h>


    #define HC_DELAY_WAITING_TRIGGER   0x10 // 10 us
    #define HDC_DELAY_TIMEOUT   false

        /**
     * Initializes the library working mode
     * @param tempAdd   Address in which will be saved the temperature readings
     * @param humAdd    Address in which will be saved the humidity readings
     */
    void HCinit (unsigned short *distanceAdd);
     
    /**
     * Updates the last distancia result in the pointed variable
     * @return true if success 
     */
    bool HCread ();


#ifdef	__cplusplus
}
#endif

#endif	/* HCSR04_H */

