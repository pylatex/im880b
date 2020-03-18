#include <xc.h>
#include "ADC.h"

typedef struct {
    unsigned short adcres;
} ADC_D_t;

ADC_D_t ADC_D;


void ADC_Read(void) {
    ADCON0=0b00011111;    //Inicia Medicion en AN7, osea pin RC3
    while(ADCON0bits.GO);      //Se queda esperando hasta que acabe la conversion
    ADC_D.adcres = ADRES;
}

unsigned short ADC_Get_Data(void) {
    ADC_Read();
    return ADC_D.adcres;
}
