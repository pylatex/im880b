//------------------------------------------------------------------------------
// Librerias y Definiciones
//------------------------------------------------------------------------------

#include <stdbool.h>
#include "lorawan_hci.h"
#include "CRC16.h"
#include "globaldefs.h" //Contexto de compilacion

//------------------------------------------------------------------------------
// Codigo Fuente
//------------------------------------------------------------------------------

/**
 * @brief Envia un comando HCI
 * 
 * Toma el comando HCI, calcula su CRC (SLIP) y lo envia.
 */
bool SendHCI (unsigned char destId, unsigned char msgId, unsigned char* payload, unsigned char size)
#ifdef UC_PIC8
{
    return true;
}
#endif

/**
 * @brief Procesa un octeto recibido por el UART
 * 
 * Implementacion de una maquina de estados para el protocolo HCI en recepcion.
 */
unsigned char ProcessHCI (unsigned char valor)
{
    //Variables
    static unsigned char estado = 0;
    static unsigned char EID,MID;
    static unsigned char payloadSize=0;
    
    switch (estado) {
        case 0: //Esperando recepcion
            if (valor==WIMOD_WKUP)
                estado=1;
            break;
        case 1: //EID: Identificador de aplicacion, solo DEVMGMT o LORAWAN
            if ((valor==DEVMGMT_SAP_ID) || (valor==LORAWAN_SAP_ID)) {
                EID=valor;
                estado=2;
            } else {
                estado=0;
            }
            break;
        case 2: //MID: Identificador del Mensaje.
            MID=valor;
            estado=3;
            break;
        case 3: //Carga Util o Fin de Mensaje
            break;
        default:
            break;
    }
    
    return estado;    //Por el momento se informa que se recibio bien.
}