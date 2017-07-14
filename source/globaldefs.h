/* 
 * File:   globaldefs.h
 * Author: Alex Bustos
 *
 * Global Header for Source File. Place here your target system definition.
 * Current supported compilation targets:
 *
 * PC type systems:
 *    Q_OS_WIN  Windows, starting from Windows 7 (not verified on prior versions)
 *    UC_PIC8   Microcontroller PIC 8 bit, enhanced. Specifically with XC8 compiler and 18F2550
 *
 * Please only in the whole proyect modify this header (keep else *.h as same),
 * make changes only in *.c source files.
 */

#ifndef GLOBALDEFS_H
#define	GLOBALDEFS_H

// Choose/Add your compilation target, by adding and
// uncommenting the corresponding line here:

//#define Q_OS_WIN    //Windows. Not available yet...
//#define Q_OS_UX     //Unix Based Systems. Not availabe yet...

#define UC_PIC8     //8 bit PIC family.

#define DEFAULT_COMPORT "COM1"

#ifdef	__cplusplus
extern "C" {
#endif

    //Insertar prototipos de funciones en C aqui...
    
#ifdef	__cplusplus
}
#endif

//Actualizar segun el entorno de programacion. Si se creo un entorno nuevo,
//adicionarlo dentro del parentesis.
#if !(defined Q_OS_WIN || defined UC_PIC8)
#error "No se ha definido entorno de compilacion valido"
#endif

#endif	/* GLOBALDEFS_H */