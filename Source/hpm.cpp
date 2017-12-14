#include <stdio.h>
#include <string.h>
#include "SerialDevice.h"
#include "hpm.h"
#ifdef DEBUG
#include <time.h>
#endif // DEBUG

int flag;

/**
 * Envio de mensaje por UART
 */
void SolicitarMedida() {
    unsigned char orden[] = {0x68,0x01,0x04,0x93};
    SerialDevice_SendData(orden,4);
}

void InciarMedicion() {
    flag=1;
    unsigned char orden[] = {0x68,0x01,0x01,0x96};
    SerialDevice_SendData(orden,4);
}

void PararMedicion() {
    flag=2;
    unsigned char orden[] = {0x68,0x01,0x02,0x95};
    SerialDevice_SendData(orden,4);
}

#define MAXIMO_SENSOR 32
/**
 * Procesamiento de mensaje UART entrante
 */
void RespuestaSensor() {
    unsigned char   rxBuf[MAXIMO_SENSOR];

    // read small chunk of data
    int rxLength = SerialDevice_ReadData(rxBuf, MAXIMO_SENSOR);
    if (rxLength) { // data available ?
        if (rxBuf[0]==0x96 && rxBuf[1] == 0x96) {
            //Instruccion no aceptada por el sensor
            printf("Algo Malio Sal. Reintente\r\n");
            return;
        }
        else if ((rxBuf[0]==0x40)&&(rxBuf[1]==0x05)&&(rxBuf[2]==0x04))
        {
            //Mostrar mediciones
            printf(" PM 2.5: %d, PM 10: %d\r\n",((rxBuf[3]<<8)+rxBuf[4]),((rxBuf[5]<<8)+rxBuf[6]));
        }
        else if ((rxBuf[0]==0xA5)&&(rxBuf[1]==0xA5))
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
            printf("Respuesta (%i):",rxLength);
            for (unsigned char i=0;i<rxLength;i++) {
                printf(" %02X",rxBuf[i]);
            }
            printf("\n\r");
        }
        #ifdef DEBUG
        printf("%d\n\r",(int)clock());
        #endif // DEBUG
    }
}
