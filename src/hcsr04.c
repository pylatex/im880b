#include "hcsr04.h"
#include "nucleoPIC.h"
#include "tmr1.h"

#define TRIGGER LATA5 // Pin Trigger HC-SR04
#define ECHO RA4 // Pin Echo HC-SR04
#define HC_DELAY_WAITING_TRIGGER   0x10 // 10 us

void HCSinit(void){
  //  TMR1_Initialize();  // Inicicializa el timer 1 para que cuente de 500ns a 63ms
    TRISAbits.TRISA4=true;//Echo RA4 input
    TRISAbits.TRISA5=false;//Trigger RA5 output
}

bool HCSread (int *buffer) { //*  buffer declara la variable tipo puntero de entero
    int i=0;
    TRIGGER = true;               // Inicia pulso
    __delay_us(HC_DELAY_WAITING_TRIGGER); // Hace el pulso para que el sensor se active
    TRIGGER = false;              // Termina pulso
    while(ECHO == false){
    /*    i=i+1;
        __delay_us(10);
        if(i>100)
        return false;//permite saber si el sensor esta desconectado*/
    }
    //i=0;
    TMR1_StartTimer();
    while(ECHO == true){/*
        i=i+1;
        __delay_us(10);    */
    }
    *buffer=(unsigned int)(5*TMR1_ReadTimer()/584);     //Conversion para medida en centimetro y  *buffer asigna al contenido de la direcciom
    //*buffer=(unsigned int)(5*i/29);     //Conversion para medida en centimetro y  *buffer asigna al contenido de la direcciom
    return true;
}
