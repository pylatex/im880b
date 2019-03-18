#include <xc.h>
#include <math.h>
#include "ADC.h"
void Config_ADC(void){
    //configuracion de los pines A0,A1,A2,C0 como entradas
    TRISAbits.TRISA0=0;
    TRISAbits.TRISA1=0;
    TRISAbits.TRISA2=0;    
    TRISCbits.TRISC0=0;
    
    //configurando los pines A0,A1,A2,C0 como entradas analogas
    ANSELAbits.ANSA0=1;
    ANSELAbits.ANSA1=1;
    ANSELAbits.ANSA2=1;
    ANSELCbits.ANSC0=1;
    //Desactivar los pull_up 
    WPUAbits.WPUA0=1;
    WPUAbits.WPUA1=1;
    WPUAbits.WPUA2=1;
    WPUCbits.WPUC0=1;
    
    //CONFIGURACION DEL PERIFERICO ADC
    ADCON1bits.ADFM=1;      //justificado a la derecha
    ADCON1bits.ADCS=3;      //FOSC/32 tiempo recomendado de TAD para trabajor a los 8MHz
    ADCON1bits.ADNREF=0;    //VREF- is connected to VSS    
    ADCON1bits.ADPREF=0;    //VREF+ is connected to VDD
    ADCON0bits.ADON=0;      //periferico  apagado, para ahorrar energia
    
    
            
    
    
}
float valorPropano(void) {
    
    //convercion ADC 
    
    ADCON0bits.CHS=0;       //seleccion de canal 0
    ADCON0bits.ADON=1;      //encender el periferico ADC
    __delay_us(10);          //espera requerida por el ADC
    ADCON0bits.GO=1;        //Inicia Medicion en AD0    
    while(ADCON0bits.GO);      //Se queda esperando hasta que acabe la conversion
    float R_adc=ADRES;
    ADCON0bits.ADON=0;      //apagar el periferico ADC para ahorrar energia
    
    //escalamiento de las unidades
    float Voltaje=R_adc*5/1024;
    float Rs=1000*((5-Voltaje)/Voltaje);
//    float ppm=0.4091*pow(Rs/5463,-1.497);
    return Rs;
}

float valorozono(void) {
    
    //convercion ADC 
    
    ADCON0bits.CHS=1;       //seleccion de canal 1
    ADCON0bits.ADON=1;      //encender el periferico ADC
    __delay_us(10);          //espera requerida por el ADC
    ADCON0bits.GO=1;        //Inicia Medicion en AD1  
    while(ADCON0bits.GO);      //Se queda esperando hasta que acabe la conversion
    float R_adc=ADRES;    
    ADCON0bits.ADON=0;      //apagar el periferico ADC para ahorrar energia
    
    //escalamiento de las unidades
    float Voltaje=R_adc*5/1024;
    float Rs=1000*((5-Voltaje)/Voltaje);
//    float ppm=0.4091*pow(Rs/5463,-1.497);
    return Rs;
}

float valorCO(void) {
    
    //convercion ADC 
    
    ADCON0bits.CHS=2;       //seleccion de canal 2
    ADCON0bits.ADON=1;      //encender el periferico ADC
    __delay_us(10);          //espera requerida por el ADC
    ADCON0bits.GO=1;        //Inicia Medicion en AD2  
    while(ADCON0bits.GO);      //Se queda esperando hasta que acabe la conversion
    float R_adc=ADRES;
    ADCON0bits.ADON=0;      //apagar el periferico ADC para ahorrar energia
    
    //escalamiento de las unidades
    float Voltaje=R_adc*5/1024;
    float Rs=1000*((5-Voltaje)/Voltaje);
//    float ppm=0.4091*pow(Rs/5463,-5);
    return Rs;
}


float valor135(void) {
    
    //convercion ADC 
        
    ADCON0bits.CHS=4;       //seleccion de canal 4
    ADCON0bits.ADON=1;      //encender el periferico ADC
    __delay_us(10);          //espera requerida por el ADC
    ADCON0bits.GO=1;        //Inicia Medicion en AD4  
    while(ADCON0bits.GO);      //Se queda esperando hasta que acabe la conversion
    float R_adc=ADRES;
    ADCON0bits.ADON=0;      //apagar el periferico ADC para ahorrar energia
    
    //escalamiento de las unidades
    float Voltaje=R_adc*5/1024;
    float Rs=1000*((5-Voltaje)/Voltaje);
//    float ppm=0.4091*pow(Rs/5463,-5);
    return Rs;
}