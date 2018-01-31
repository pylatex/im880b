#include <stdio.h>
#include <string.h>
#include "hpm.h"
#ifdef DEBUG
#include <time.h>
#endif // DEBUG

typedef struct {
    hpm_enviaSerie_t enviar;
} hpm_t;

int flag;
hpm_t hpm;

void InicializacionHPM(hpm_enviaSerie_t enviaSerie) {
    hpm.enviar = enviaSerie;
}

/**
 * Envio de mensaje por UART
 */
void SolicitarMedida() {
    unsigned char orden[] = {0x68,0x01,0x04,0x93};
    hpm.enviar(orden,4);
}

void InciarMedicion() {
    flag=1;
    unsigned char orden[] = {0x68,0x01,0x01,0x96};
    hpm.enviar(orden,4);
}

void PararMedicion() {
    flag=2;
    unsigned char orden[] = {0x68,0x01,0x02,0x95};
    hpm.enviar(orden,4);
}

/**
 * Procesamiento de mensaje UART entrante
 */
void RespuestaSensor(unsigned char *carga,unsigned char peso) {
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
        if(flag == 1)
        {
                printf(" Medicion Iniciada\r\n");
        }
        else{
            printf(" Medicion Detenida\r\n");
        }
    } else {
        //Caso por defecto, procesamiento no implementado.
        printf("Respuesta (%i):",peso);
        for (unsigned char i=0;i<peso;i++) {
            printf(" %02X",carga[i]);
        }
        printf("\n\r");
    }
}
