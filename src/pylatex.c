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
#include <string.h>

//------------------------------------------------------------------------------
//  Definitions, Declarations and Variables
//------------------------------------------------------------------------------

#define LARGO   50
#define PUERTO  5

static struct {
    char                    carga[LARGO];
    char                    cnt;
    delayHandlerFunction    delfun;
    volatile bool          *tmrrun;
} PY;

volatile unsigned char timeouts;

//------------------------------------------------------------------------------
//  Function Implementations
//------------------------------------------------------------------------------

void initLW (serialTransmitHandler txfun, LWstat *LWstatus) {
    WiMOD_LoRaWAN_Init(txfun,LWstatus);
}

void registerDelayFunction(delayHandlerFunction delfun,volatile bool *flag) {
    PY.delfun=delfun;
    PY.tmrrun=flag;
}

bool AppendMeasure (char variable,char *medida) {
    if (medida) {
        switch (variable) {
            case PY_CO2:
            case PY_TVOC:
            case PY_PM025:
            case PY_PM100:
            case PY_GAS:
            case PY_ILUM1:
            case PY_ILUM2:
            case PY_PRECIPITATION:
            case PY_WINDSPEED:
            case PY_DISTANCE:
                //FORMATO A
                if (PY.cnt < (LARGO - 3)) {
                    PY.carga[PY.cnt++]=variable;   //Identificador
                    PY.carga[PY.cnt++]=medida[0];  //MSB
                    PY.carga[PY.cnt++]=medida[1];  //LSB
                    return true;
                }
                break;

            case PY_PRESS1:
                //FORMATO E
                if (PY.cnt < (LARGO - 31)) {
                    PY.carga[PY.cnt++]=variable;   //Identificador
                    memcpy(&PY.carga[PY.cnt], medida, 30 );
                    PY.cnt+=30;
                }
                break;

            case PY_GPS:
                //FORMATO F
                if (PY.cnt < (LARGO - 10)) {
                    PY.carga[PY.cnt++]=variable;   //Identificador
                    memcpy(&PY.carga[PY.cnt], medida, 9 );
                    PY.cnt+=9;
                }
                break;

            default:
                break;
        }
    }
    return false;
}


void SendMeasures (pyModeType confirmed) {
    if (PY.cnt) {
        if (confirmed)
            WiMOD_LoRaWAN_SendCRadioData(PUERTO, PY.carga, PY.cnt);
        else
            WiMOD_LoRaWAN_SendURadioData(PUERTO, PY.carga, PY.cnt);
        clearAppPayload();
    }
}

char *short2charp (unsigned short in) {
    static char val[2];
    val[0]=in>>8    & 0xFF;
    val[1]=in       & 0xFF;
    return val;
}

void pylatexRx (char RxByteUART) {
    IncomingHCIpacker(RxByteUART);
}

void clearAppPayload(void){
    PY.cnt=0;
}
