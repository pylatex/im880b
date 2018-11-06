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

    #define _XTAL_FREQ 8000000  //May be either Internal RC or external oscillator.

    #pragma config FOSC = INTOSC, WDTE = OFF, PWRTE = ON, MCLRE = ON, CP = OFF, BOREN = OFF, CLKOUTEN = OFF, IESO = ON, FCMEN = ON
    #pragma config WRT = OFF, PPS1WAY =	OFF, ZCD = OFF, PLLEN =	OFF, STVREN = ON, BORV = HI, LPBOR =	OFF, LVP = OFF

    #define LED LATC0   //Para las pruebas de parpadeo y ping
    //#define PIN RB5     //Para probar en un loop con LED=PIN
    #define DVI LATC1   //Pin DVI del sensor BH1750FVI
    #define I2CSENS LATC7 //Sensores I2C

    #define I2CPersOn()     I2CSENS = 1
    #define I2CPersOff()    I2CSENS = 0

    /**
     * Inhabilita/Habilita el cambio de perifericos
     * @param lock: 1 para bloquear los cambios a registros PPS, 0 para desbloquear.
     */
    void ppsLock (bool lock);

    /**
     * Inicializacion de la mayoria de perifericos y otras
     * opciones del microcontrolador
     */
    void setup (void);

    /**
     * Habilita las interrupciones
     */
    void enableInterrupts (void);

#ifdef	__cplusplus
}
#endif

#endif	/* NUCLEOPIC_H */

