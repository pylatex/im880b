#include <stdio.h>
#include <string.h>
#include "hpm.h"

#define RXBUFLEN    32  //Tamaño del buffer de recepcion

static hpm_enviaSerie_t     handlerEnvio;
static char                 rxBuffer[RXBUFLEN];   //Buffer de recepcion
static char                 rxIdx;          //Indice del buffer de recepcion
static bool                 autosend;
static uint16_t             pm25;
static uint16_t             pm10;
static comandoEnviar_t ultimoComandoEnviado;

typedef enum{ 
    ReadMeasure,
    StartMeasure, 
    StopMeasure,
    EnableAutosend,
    DisableAutosend
    
}comandoEnviar_t;


static void enviaOrden (char *const orden, char largo);

//si modo es true es auto send
void StartHPM(hpm_enviaSerie_t enviaSerie) {
    handlerEnvio = enviaSerie;
    rxIdx = 0;
    hpmChangeAutosend(false);
}

void hpmChangeAutosend (bool enable) {
    if (enable){
        hpmSendEnableAutoSend();
    }
    else{
        hpmSendDisableAutoSend();
    }
    autosend = enable;
}

void hpmSendReadMeasure(void) {
    ultimoComandoEnviado=ReadMeasure;
    const char orden[] = {0x68,0x01,0x04,0x93};
    enviaOrden(orden,4);
}

void hpmSendStartMeasure(void) {
    ultimoComandoEnviado=StartMeasure;
    const char orden[] = {0x68,0x01,0x01,0x96};
    enviaOrden(orden,4);
    
}

void hpmSendStopMeasure(void) {
    ultimoComandoEnviado=StopMeasure;
    const char orden[] = {0x68,0x01,0x02,0x95};
    enviaOrden(orden,4);
}

void hpmSendEnableAutoSend(void) {
    ultimoComandoEnviado=EnableAutosend;
    const char orden[] = {0x68,0x01,0x40,0x57};
    enviaOrden(orden,4);
}

void hpmSendDisableAutoSend(void) {
    ultimoComandoEnviado=DisableAutosend;
    const char orden[] = {0x68,0x01,0x20,0x77};
    enviaOrden(orden,4);
}

bool SensorAnswer(char *carga) {
    bool medida=true;
    if (carga[0]==0x96 && carga[1] == 0x96) {
        medida=false;
    }
    return medida;
}

uint16_t getLastPM25 (void)
{
    /*
    //Quizas no sea necesario este bloque de codigo, teniendo en cuenta la descripcion
    //de la funcion HPMinput() - TODO: Eliminar este comentario, de ser necesario
    if (autosend == true){
        pm25=(carga[6]<<8)+carga[7];
    }
    else{
        hpmSendReadMeasure();
        pm25=(carga[3]<<8)+carga[4];
    }
    // */
    return pm25;
}

uint16_t getLastPM10(void)
{
    /*
    //Quizas no sea necesario este bloque de codigo, teniendo en cuenta la descripcion
    //de la funcion HPMinput() - TODO: Eliminar este comentario, de ser necesario
    if (autosend == true){
        pm10=(carga[8]<<8)+carga[9];
    }
    else{
        hpmSendReadMeasure();
        pm10=(carga[5]<<8)+carga[6];
    }
    // */
    return pm10;
}
/*
void HPMinput(char octeto) {
    if (rxIdx < RXBUFLEN) {
        rxBuffer[rxIdx++] = octeto;
        /* La gracia del siguiente bloque de codigo es que vaya definiendo el
         * valor de las variables pm25 y pm10 dejadas al comienzo de este archivo.
         * Adicionalmente, hay la posibilidad de cambiar las funciones getLast*
         * para que en vez de devolver la variable directamente (como esta en
         * esta version), devuelvan un booleano, "updated", que se puede declarar
         * al comienzo del codigo. Y es en este bloque de codigo que ese "updated"
         * se ha de actualizar. Idealmente, se habria de poner a true, cuando
         * ya se tengan los octetos suficientes segun el modo y/u orden enviada.
         
        if (autosend) {
            /*TODO: Procesamiento segun Tabla 5 del Datasheet
             
              
        } else {
            /*TODO: Procesamiento segun Tabla 4 del Datasheet
             
            
        }
    }
}
*/

void HPMinput(char octeto) {
    if (rxIdx < RXBUFLEN) {
        rxBuffer[rxIdx++] = octeto;
        if (autosend) {
            if (rxIdx == RXBUFLEN){
                rxBuffer[rxIdx++]=octeto;
                pm10=(rxBuffer[8]<<8)+rxBuffer[9];
                pm25=(rxBuffer[6]<<8)+rxBuffer[7];
                rxIdx=0;
                
            }else{
                switch(ultimoComandoEnviado){
                    case ReadMeasure:
                     if (rxIdx == RXBUFLEN){
                        rxBuffer[rxIdx++]=octeto;
                        pm10=(rxBuffer[5]<<8)+rxBuffer[6];
                        pm25=(rxBuffer[3]<<8)+rxBuffer[4];
                        rxIdx=0;
                        break;
                
            }   
                    
                } 
                
            }
            /*TODO: Procesamiento segun Tabla 5 del Datasheet*/
             
              
        } 
    }
}
         
void enviaOrden (char *const orden, char largo) {
    handlerEnvio(orden,largo);
    rxIdx = 0;
}
