/* 
 * File:   globaldefs.h
 * Author: GITUD
 *
 * Created on 4 de julio de 2017, 07:22 PM
 */

#ifndef GLOBALDEFS_H
#define	GLOBALDEFS_H

//Agregar los entornos de compilacion validos aca:
//#define Q_OS_WIN  //Comentado por logicas razones
#define UC_PIC8 //Familia PIC de 8 bits

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