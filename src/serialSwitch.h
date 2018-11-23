/* 
 * File:   serialSwitch.h
 * Author: Alex F. Bustos P. - GITUD Member
 *
 * Created on November 22, 2018, 5:53 PM
 */

#ifndef SERIALSWITCH_H
#define	SERIALSWITCH_H

#include <xc.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum {MODEM_LW,GPS,HPM,DEBUG1,DEBUG2} serial_t;

    /**
     * Inhabilita/Habilita el cambio de perifericos
     * @param lock: 1 para bloquear los cambios a registros PPS, 0 para desbloquear.
     */
    void ppsLock (bool lock);

    extern void cambiaSerial (serial_t serial);
    extern void enviaIMST(char *arreglo,unsigned char largo);
    extern void enviaGPS(char *arreglo,unsigned char largo);
    extern void enviaDebug(char *arreglo,unsigned char largo);
    
    extern serial_t modoSerial;     //Elemento Serial que se esta controlando

#ifdef	__cplusplus
}
#endif

#endif	/* SERIALSWITCH_H */

