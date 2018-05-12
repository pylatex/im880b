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
void SolicitarMedida() {
    const char orden[] = {0x68,0x01,0x04,0x93};
    hpm.enviar(orden,4);
}

void InciarMedicion() {
    const char orden[] = {0x68,0x01,0x01,0x96};
    hpm.enviar(orden,4);
}

void PararMedicion() {
    const char orden[] = {0x68,0x01,0x02,0x95};
    hpm.enviar(orden,4);
}

/**
 * Procesamiento de mensaje UART entrante
 */
void RespuestaSensor(char *carga,char peso) {
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
        // */
    } else {
        //Caso por defecto, procesamiento no implementado.
        printf("Respuesta (%i):",peso);
        for (unsigned char i=0;i<peso;i++) {
            printf(" %02X",carga[i]);
        }
        printf("\n\r");
    }
}

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