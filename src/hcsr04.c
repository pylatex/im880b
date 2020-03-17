#include "hcsr04.h"
#include "nucleoPIC.h"
#include "tmr1.h"

#define TRIGGER LATA5 // Pin Trigger HC-SR04
#define ECHO RA4 // Pin Echo HC-SR04
#define HC_DELAY_WAITING_TRIGGER   0x10 // 10 us

void HCSinit(void){
    TRISAbits.TRISA4=true;//Echo RA4 input
    TRISAbits.TRISA5=false;//Trigger RA5 output
}

bool HCSread (int *buffer) { //*  buffer declara la variable tipo puntero de entero
    TRIGGER = true;               // Inicia pulso
    __delay_us(HC_DELAY_WAITING_TRIGGER); // Hace el pulso para que el sensor se active
    TRIGGER = false;              // Termina pulso
    TMR1_Initialize();  // Inicicializa el timer 1 para que cuente de 500ns a 63ms
    TMR1_StartTimer();
    while(ECHO == false && TMR1_ReadTimer()<2000);
    TMR1_StopTimer();
    if(ECHO==false)
        return false;
    TMR1_Initialize();  // Inicicializa el timer 1 para que cuente de 500ns a 63ms
    TMR1_StartTimer(); //
    while(ECHO == true );
    TMR1_StopTimer();
    *buffer=(unsigned int)(5*TMR1_ReadTimer()/584);     //Conversion para medida en centimetro y  *buffer asigna al contenido de la direccion
    return true;
}
