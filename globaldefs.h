/* 
 * File:   globaldefs.h
 * Author: GITUD
 *
 * Created on 4 de julio de 2017, 07:22 PM
 */

#ifndef GLOBALDEFS_H
#define	GLOBALDEFS_H

//#define Q_OS_WIN  //Comentado por logicas razones
#define UC_PIC8 //Familia PIC de 8 bits

#ifdef	__cplusplus
extern "C" {
#endif

    //Ni idea... esto lo coloco mplabx/netbeans

#ifdef	__cplusplus
}
#endif

#if !(defined Q_OS_WIN || defined UC_PIC8)
#error "No se ha definido entorno de compilacion valido"
#endif	/* GLOBALDEFS_H */

