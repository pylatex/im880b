/*
 * File:   hcsr04.h
 * Author: Nicolas Pastran at gmail dot com
 *
 * Created on 1 Nov de 2018, 15:49
 */

#ifndef HCSR04_H
#define	HCSR04_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdbool.h>

    /**
     * Initializes the pin setup
     */
    void HCSinit(void);

    /**
     * Updates the last distance result in the pointed variable
     * @return true on success
     */
    bool HCSread (int *buffer);

#ifdef	__cplusplus
}
#endif

#endif	/* HCSR04_H */

