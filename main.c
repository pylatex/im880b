/*
 * Archivo de ejemplo, para implementacion en microcontroladores.
 * Se pretende que se requieran interrupciones para el funcionamiento.
 * 
 * La mayoria de archivos vienen del comprimido
 * WiMOD_LoRaWAN_ExampleCode_HCI_C_V0_1.zip
 * y han sido ligeramente modificados para soportar tipos estandar.
 */
#include <xc.h>
#include <stdbool.h>

#include "globaldefs.h"

#if defined Q_OS_WIN
    //Todo el codigo del main del ejemplo de IMST...
    int main(int argc, char *argv[])
#elif defined UC_PIC8
    void main(void) 
#endif
{
#if defined Q_OS_WIN
        ////////////////////////////////////////////////////////////////////////
        //Codigo del main del ejemplo de IMST:
        ////////////////////////////////////////////////////////////////////////
        
        bool run = true;

        char comPort[40];

        // comport parameter attached ?
        if (argc >= 2)
        {
            strncpy(comPort, argv[1], 40);
        }
        else
        {
            strcpy(comPort, defPORT);
            printf("usage: WiMOD_LoRaWAN_HCI_C_ExampleCode COMxy");
        }

        // init interface:
        if(!WiMOD_LoRaWAN_Init(comPort))
        {
            printf("error - couldn't open interface on comport %s\r\n",comPort);
            printf("try: iMOD_LoRaWAN_HCI_C_ExampleCode COMxy to select another comport\n\r");

            return -1;
        }

        // show menu
        ShowMenu(comPort);

        // main loop
        while(run) {
            // handle receiver process
            WiMOD_LoRaWAN_Process();

            // keyboard pressed ?
            if(kbhit())
            {
                // get command
                char cmd = getch();

                printf("%c\n\r",cmd);

                // handle commands
                switch(cmd)
                {
                    case    'e':
                    case    'x':
                            run = false;
                            break;

                    case    'i':
                            // get device info
                            GetDeviceInfo();
                            break;


                    case    'p':
                            // ping device
                            Ping();
                            break;

                    case    'j':
                            // join network
                            Join();
                            break;

                    case    'u':
                            // send u-data
                            SendUData();
                            break;

                    case    'c':
                            // send c-data
                            SendCData();
                            break;

                    case    ' ':
                            ShowMenu(comPort);
                            break;
                }
            }
        }
        //Fin Codigo IMST.
        ////////////////////////////////////////////////////////////////////////
#elif defined UC_PIC8
        ////////////////////////////////////////////////////////////////////////
        //Codigo para los PIC de 8 bits

        //INICIALIZACION
        
        //BUCLE
        while (true) {
            ;
        }
        //Fin Codigo PIC 8 bits
        ////////////////////////////////////////////////////////////////////////
#endif

#if defined Q_OS_WIN
    return 0;
#endif
}

#if defined UC_PIC8
void interrupt ISR (void) {
    
}
#endif