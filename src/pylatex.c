/*
 * Implementacion de la aplicacion propia para LoRaWAN
 * 
 * Author:  Alex F. Bustos P.
 * Created: Feb 7, 2018
 */

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------

#include "WiMOD_LoRaWAN_API.h"
#include "pylatex.h"

//------------------------------------------------------------------------------
//  Definitions, Declarations and Variables
//------------------------------------------------------------------------------

#define LARGO   20
#define PUERTO  5

typedef struct {
    char carga[LARGO];
    char cnt;
} PY_T;

#define initPayload() PY.cnt=0

static PY_T PY;

//------------------------------------------------------------------------------
//  Function Implementations
//------------------------------------------------------------------------------

void initLoraApp (void) {
    initPayload();
}

bool AppendMeasure (char variable,char *medida) {
    if (medida) {
        switch (variable) {
            case PY_CO2:
            case PY_TVOC:
            case PY_PM025:
            case PY_PM100:
            case PY_GAS:
                //FORMATO A
                if (PY.cnt < (LARGO - 3)) {
                    PY.carga[PY.cnt++]=variable;   //Identificador
                    PY.carga[PY.cnt++]=medida[0];  //MSB
                    PY.carga[PY.cnt++]=medida[1];  //LSB
                    return true;
                }
                break;
        }
    }
    return false;
}

void SendMeasures (void) {
    WiMOD_LoRaWAN_SendURadioData(5, PY.carga, PY.cnt);
    initPayload();
}

char *short2charp (unsigned short in) {
    static char val[2];
    val[0]=in>>8    & 0xFF;
    val[1]=in       & 0xFF;
    return val;
}