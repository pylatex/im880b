/* 
 * File:   nucleoPIC.h
 * Author: Alex Francisco Bustos Pinzon - GITUD
 *
 * Created on 30 de junio de 2018, 01:04 PM
 */

#include <xc.h>
#include <stdbool.h>

#ifndef NUCLEOPIC_H
#define	NUCLEOPIC_H

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * Inhabilita/Habilita el cambio de perifericos
     * @param lock: 1 para bloquear los cambios a registros PPS, 0 para desbloquear.
     */
    void ppsLock (bool lock);

#ifdef	__cplusplus
}
#endif

#endif	/* NUCLEOPIC_H */
