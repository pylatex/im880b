#include <stdio.h>
#include <string.h>
#include "hpm.h"
#ifdef DEBUG
#include <time.h>
#endif // DEBUG

typedef struct {
    hpm_enviaSerie_t    enviar;
    char                idx,buffer[8];
    hpm_timStarter      timer;
    bool                bandera;
} hpm_t;

static hpm_t hpm;

#define iniciatim()   if (hpm.timer) hpm.timer(5,&hpm.bandera);


void InicializacionHPM(hpm_enviaSerie_t enviaSerie) {
    hpm.enviar = enviaSerie;
    hpm.idx = 0;
    hpm.timer = 0;
}

void HPMregistraTimer(hpm_timStarter timerFunction) {
    hpm.timer = timerFunction;
}

/**
 * Envio de mensaje por UART
 */
void ReadMeasure() {
    const char orden[] = {0x68,0x01,0x04,0x93};
    hpm.enviar(orden,4);
}

void StartMeasure() {
    const char orden[] = {0x68,0x01,0x01,0x96};
    hpm.enviar(orden,4);
}

void StopMeasure() {
    const char orden[] = {0x68,0x01,0x02,0x95};
    hpm.enviar(orden,4);
}
void StartAutoSend() {
    const char orden[] = {0x68,0x01,0x40,0x57};
    hpm.enviar(orden,4);
}
void StopAutoSend() {
    const char orden[] = {0x68,0x01,0x20,0x77};
    hpm.enviar(orden,4);
}
bool SensorAnswer(char *carga) {
    bool medida=true;
    if (carga[0]==0x96 && carga[1] == 0x96) {
        medida=false;
   }
    return medida;
}
void StartHPM(bool modo)//si modo es true es auto send
{
    StartMeasure();
    if (bool=true){
        StartAutoSend();
    }
    else{
        StopAutoSend();
    }
}
char ReadPM25 (char *carga, bool modo)//si modo es true es auto send
{
    char pm25;
    if (modo=true){
        pm25=(carga[6]<<8)+carga[7];
    }
    else{
        ReadMeasure();
        pm25=(carga[3]<<8)+carga[4];
    }
    return pm25;
}
char ReadPM10(char *carga, bool modo)//si modo es true es auto send
{
    char pm10;
    if (modo=true){
        pm10=(carga[8]<<8)+carga[9];
    }
    else{
        ReadMeasure();
        pm10=(carga[5]<<8)+carga[6];
    }
    return pm10;
}

/**
 * Procesamiento de mensaje UART entrante
 */
/*void RespuestaSensor(char *carga,char peso) {
    if (carga[0]==0x96 && carga[1] == 0x96) {
        //Instruccion no aceptada por el sensor
        printf("Algo Malio Sal. Reintente\r\n");
        return;
    }
    else if ((carga[0]==0x40)&&(carga[1]==0x05)&&(carga[2]==0x04))
    {
        //Mostrar mediciones
        printf(" PM 2.5: %d, PM 10: %d\r\n",((carga[3]<<8)+carga[4]),((carga[5]<<8)+carga[6]));
    }
    else if ((carga[0]==0xA5)&&(carga[1]==0xA5))
    {
        printf(" OK\r\n");
        /*
        if(flag == 1)
        {
                printf(" Medicion Iniciada\r\n");
        }
        else{
            printf(" Medicion Detenida\r\n");
        }
        // 
    } else {
        //Caso por defecto, procesamiento no implementado.
        printf("Respuesta (%i):",peso);
        for (unsigned char i=0;i<peso;i++) {
            printf(" %02X",carga[i]);
        }
        printf("\n\r");
    }
}*/

void smHPM (char val) {
    if (hpm.bandera) {
        //Esta corriendo, quiere decir que aun no se ha cortado la recepcion
        iniciatim();
        hpm.buffer[hpm.idx++] = val;
    } else {
        //Desborde, por tanto: fin de mensaje
        
    }
}

void enviaOrden (char *const orden) {
    hpm.enviar(orden,4);
    iniciatim();
}