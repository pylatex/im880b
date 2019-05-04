/*

 */

//#define SOFTWARE_REDIRECTION    //GPS redirection to debug port by software

#include "nucleoPIC.h"
#ifndef SOFTWARE_REDIRECTION
#include <stdio.h>
#include "hpm.h"
#include "SerialDevice.h"
#include "pylatex.h"
extern serial_t modoSerial;     //Elemento Serial que se esta controlando
extern void cambiaSerial (serial_t serial);    //Para cambiar el elemento a controlar
extern void enviaDebug(char *arreglo,unsigned char largo);
extern void enviaHPM(char *arreglo,unsigned char largo);
#endif



void main (void) {
    setup();

    #ifdef SOFTWARE_REDIRECTION
    /*
    LATC = 0xC0;
    TRISC = 0x7F;   //Salida solo C7
    while(true){
        RC7=RB5;
    }
    // */
    //*
    TRISC = 0xF7;   //Salida solo en C3
    TRISA = 0xEF;   //salida solo en A4
    while(true){
        RA4=RB7;
        RC3=RA5;
    }
    #else
    cambiaSerial(DEBUG1);
    enableInterrupts();

    HPMdata_t HPM;
    InicializacionHPM(enviaHPM);
    //retardo();

    //TODO: Desactivar autosend
    //En la inicializaciòn del hpm esta la desactivacion del autosend
    
    
    while(1) {
        //TODO: Solicitar medicion
        
        hpmSendStartMeasure (); //Inicia la medicion
        __delay_ms(25);
        hpmSendReadMeasure(); //Lee la medicion
        __delay_ms(25);
        hpmSendStopMeasure(); //Deja de medir
        __delay_ms(25);
        //HPMinput(enviaHPM);//Obtiene el valor dependinedo del modo de uso para PM10 y PM25
        //TODO: Esperar respuesta
        //TODO: Procesar respuesta
        
        //TODO: Imprimir valor en el debug usando enviaDebug()
        if (HPMupdated()) {
            HPM.pm10=getLastPM10();
            HPM.pm25=getLastPM25();
            uint8_t buff[50];//verificar el tamaño
            sprintf(buff, "PM10: %i, PM25: %i \n",
                    (unsigned int)HPM.pm10, (unsigned int)HPM.pm25 );
            enviaDebug(buff,0);
            cambiaSerial(DEBUG1);
        }
        else{ 
          uint8_t buff[50];   
        sprintf(buff," fallo ");
            enviaDebug(buff,0);
            cambiaSerial(DEBUG1);
        }
        
    }
    #endif

}

#ifndef SOFTWARE_REDIRECTION
void __interrupt ISR (void) {
    uint8_t rx_err;  //Error de recepcion en UART
    if (RCIE && RCIF) {
        //Error reading
        rx_err=RCSTA;
        //As RCREG is argument, their reading implies the RCIF erasing
        HPMinput(RCREG);
    }
    else {
        //Unhandled Interrupt
    }
}
#endif
