/* 
 * File:   pylatex.h
 * Author: Alex Bustos
 *
 * Created on February 7, 2018, 12:28 PM
 */

#ifndef PYLATEX_H
#define	PYLATEX_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdbool.h>
                            //MEDIDA    FORMATO
    #define PY_CO2      1   //CO2       A
    #define PY_TVOC     2   //TVOC      A
    #define PY_PM025    3   //PM 2.5    A
    #define PY_PM100    4   //PM 10     A
    #define PY_RH1      5   //RH        A --> B
    #define PY_TEMP1    6   //TEMP      A --> B
    #define PY_ILUM     7   //LUX       ??
    #define PY_GAS      8   //GAS       A

    void initLoraApp (void);
    bool AppendMeasure (char variable,char *medida);
    void SendMeasures (void);
    char *short2charp (unsigned short in);

#ifdef	__cplusplus
}
#endif

#endif	/* PYLATEX_H */

