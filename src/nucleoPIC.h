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

    #ifdef _18F2550 //PIC18F2550
    #pragma config PLLDIV = 1, CPUDIV = OSC1_PLL2, USBDIV = 1
    #if _XTAL_FREQ == 8000000
    #pragma config FOSC = INTOSCIO_EC,  FCMEN = ON, IESO = OFF // INTOSC @ 8MHz
    #elif _XTAL_FREQ == 7372800
    #pragma config FOSC = HS,  FCMEN = ON, IESO = OFF // HS @ 7.3728MHz
    #endif
    #pragma config PWRT = ON, BOR = OFF, BORV = 3, VREGEN = OFF
    #pragma config WDT = OFF, WDTPS = 32768
    #pragma config CCP2MX = ON, PBADEN = OFF, LPT1OSC = ON, MCLRE =	ON
    #pragma config STVREN = OFF, LVP = OFF, DEBUG=OFF, XINST = OFF

    #define LED LATA1 //Para las pruebas de parpadeo y ping
    //#define PIN RC0   //Para probar en un loop con LED=PIN
    #define DVI LATA2   //Pin DVI del sensor BH1750FVI
    #endif

    #ifdef _16F1769
    #pragma config FOSC = INTOSC, WDTE = OFF, PWRTE = ON, MCLRE = ON, CP = OFF, BOREN = OFF, CLKOUTEN = OFF, IESO = ON, FCMEN = ON
    #pragma config WRT = OFF, PPS1WAY =	OFF, PLLEN =	OFF, STVREN = ON, BORV = HI, LPBOR =	OFF, LVP = OFF//, ZCD = OFF

    #define LED LATA1   //Para las pruebas de parpadeo y ping
    //#define PIN RB5     //Para probar en un loop con LED=PIN
    #define DVI LATC1   //Pin DVI del sensor BH1750FVI
    #endif

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

