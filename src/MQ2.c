#include <xc.h>
#include "MQ2.h"

typedef struct {
    unsigned short adcres;
} MQ2_t;

MQ2_t MQ2;


void ADCpropano(void) {
    ADCON0=0x03;    //Inicia Medicion en AD0
    while(ADCON0bits.GO);      //Se queda esperando hasta que acabe la conversion
    MQ2.adcres = ADRES;
}

unsigned short valorPropano(void) {
    ADCpropano();
    return MQ2.adcres;
}
