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

    #include <xc.h>
    #include <stdbool.h>
    #include "nucleoPIC.h"


    #define HC_DELAY_WAITING_TRIGGER   0x10 // 10 us
     
    /**
     * Updates the last distancia result in the pointed variable
     * @return true if success 
     */
    bool HCread (int *buffer);


#ifdef	__cplusplus
}
#endif

#endif	/* HCSR04_H */

