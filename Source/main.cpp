/*------------------------------------------------------------------------------
  File:		main.cpp
  Abstract:	main module
	Version:	0.1
	Date:		18.05.2016
  Disclaimer:	This example code is provided by IMST GmbH on an "AS IS"
              basis without any warranties.

------------------------------------------------------------------------------*/
#define DEBUG
//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "target.h"   //Open and modify to change the target system (WIN/UX)
#include "SerialDevice.h"
#ifdef DEBUG
#include <time.h>
#endif // DEBUG
//------------------------------------------------------------------------------
//  Declarations and Definitions
//------------------------------------------------------------------------------

// forward declarations
static void     ShowMenu(const char*);
int flag;

#define DEF_PORT "COM5"  //Default port, in case of empty input

//------------------------------------------------------------------------------
//  Section Code
//------------------------------------------------------------------------------

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
    UINT8   rxBuf[MAXIMO_SENSOR];

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

/**
 * Main
 */
int main(int argc, char *argv[])
{
    bool run = true;

    char comPort[40];

    // comport parameter attached ?
    if (argc >= 2)
    {
        strncpy(comPort, argv[1], 40);
    }
    else
    {
        strcpy(comPort, DEF_PORT);
        printf("usage: WiMOD_LoRaWAN_HCI_C_ExampleCode COMxy");
    }

    // init interface:
    if(!SerialDevice_Open(comPort,9600,8,0))
    {
        printf("error - couldn't open interface on comport %s\r\n",comPort);
        printf("try: WiMOD_LoRaWAN_HCI_C_ExampleCode COMxy to select another comport\n\r");

        return -1;
    }

    // show menu
    ShowMenu(comPort);

    // main loop
    while(run) {
        // handle receiver process
        RespuestaSensor();

        // keyboard pressed ?
        if(kbhit())
        {
            // get command
            char cmd = getch();

            printf("%c\n\r",cmd);
            #ifdef DEBUG
            printf("%d\n\r",(int)clock());
            #endif // DEBUG
            // handle commands
            switch(cmd)
            {
                case 'q':
                    run = false;
                    break;

                case 't':
                    // Obtener medida
                    SolicitarMedida();
                    break;

                case 'i':
                    // inciar medicion
                    InciarMedicion();
                    break;

                case 's':
                    // parar medicion
                    PararMedicion();
                    break;

                case ' ':
                    ShowMenu(comPort);
                    break;

                #ifdef DEBUG
                case 'f':
                    printf("CLOCKS_PER_SEC=%d\n\r",(int)CLOCKS_PER_SEC);
                #endif // DEBUG
            }
        }
    }
    return 0;
}

/**
 * ShowMenu
 * @brief: show main menu
 */
void ShowMenu(const char* comPort) {
    printf("\n\r");
    printf("------------------------------\n\r");
    printf("Using comport: %s\r\n", comPort);
    printf("------------------------------\n\r");
    printf("[SPACE]\t: show this menu\n\r");
    printf("[i]\t: Start measure from module\n\r");
    printf("[s]\t: End measure from module\n\r");
    printf("[t]\t: get measure from module\n\r");
    printf("[q]\t: exit program\n\r");
    #ifdef DEBUG
    printf("[f]\t: DEBUG - Get CLOCKS_PER_SEC value\n\r");
    #endif // DEBUG
}
//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
