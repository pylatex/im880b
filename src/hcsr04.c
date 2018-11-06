#include "hcsr04.h"
#include "nucleoPIC.h"
#include "tmr1.h"

#define TRIGGER RC4; // Pin Trigger HC-SR04
#define ECHO LATC5; // Pin Echo HC-SR04
#define HC_DELAY_WAITING_TRIGGER   0x10 // 10 us

void HCSinit(void){
    TRISCbits.TRISC5=true;//Echo RC5 input
    TRISCbits.TRISC4=false;//Trigger RC4 output
}

bool HCSread (int *buffer) { //*  buffer declara la variable tipo puntero de entero
    //int i=0;
    RC4=true;               // Inicia pulso
    __delay_us(HC_DELAY_WAITING_TRIGGER); // Hace el pulso para que el sensor se active
    RC4=false;              // Termina pulso
    while(LATC5 == false){
     /*   i=i+1;
        __delay_us(10);
        if(i>100)
            return false;//permite saber si el sensor esta desconectado*/
    }
    // i=0;
    TMR1_StartTimer();
    while(LATC5 == true){
     /*   i=i+1;
        __delay_us(10);*/        
    }
    *buffer=(unsigned int)(5*TMR1_ReadTimer()/29);     //Conversion para medida en centimetro y  *buffer asigna al contenido de la direcciom
    return true;
}
void StartTimerDelayMs(unsigned char cant)
{
    //CCPR1=cant*250; //250*A = (256-6)*A = (256-4-2)*A
    CCPR1=(unsigned short)((cant<<8)-(cant<<2)-(cant<<1));
    CCP1IF=false;   //Restablecer comparador
    CCP1CON=0x8B;   //Modulo CCP en Comparacion a (representado) cant/ms
    T1CON=0x30;     //Prescale 1:8
    TMR1=0;
    TMR1ON=true;
}