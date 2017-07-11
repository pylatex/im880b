//------------------------------------------------------------------------------
// Librerias y Definiciones
//------------------------------------------------------------------------------

#include <stdbool.h>
#include "lorawan_hci.h"
#include "CRC16.h"
#include "SerialDevice.h"   //Acceso al puerto serie / UART

//------------------------------------------------------------------------------
// Codigo Fuente
//------------------------------------------------------------------------------

/**
 * @brief Envia un comando HCI
 * 
 * Toma el comando HCI, calcula su CRC (SLIP) y lo envia.
 */
bool SendHCI (unsigned char destId, unsigned char msgId, unsigned char* payload, unsigned char size)
{

    return true;
}

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
            if (valor==DEVMGMT_SAP_ID) {
                switch (MID) {
                //Todos los que devuelven solamente estado
                case DEVMGMT_MSG_PING_RSP:
                case DEVMGMT_MSG_RESET_RSP:
                case DEVMGMT_MSG_SET_RTC_RSP:
                case DEVMGMT_MSG_SET_RTC_ALARM_RSP:
                case DEVMGMT_MSG_RTC_ALARM_IND:
                case DEVMGMT_MSG_CLEAR_RTC_ALARM_RSP:
                case DEVMGMT_MSG_SET_OPMODE_RSP:
                    estado=3;
                    break;
                }
            }
            else if (valor==LORAWAN_SAP_ID)
            {
                switch (MID) {
                //Todos los que devuelven solamente estado
                case LORAWAN_MSG_ACTIVATE_DEVICE_RSP:
                case LORAWAN_MSG_SET_JOIN_PARAM_RSP:
                case LORAWAN_MSG_JOIN_NETWORK_RSP:
                    estado=3;
                    break;
            }
            }
            
            break;
        case 3: //Carga Util o Fin de Mensaje
            break;
        default:
            break;
    }
    
    return estado;    //Por el momento se informa que se recibio bien.
}