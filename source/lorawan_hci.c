//------------------------------------------------------------------------------
// Librerias y Definiciones
//------------------------------------------------------------------------------

#include "lorawan_hci.h"
#include "CRC16.h"
#include "SerialDevice.h"   //Acceso al puerto serie / UART

#define wkupchars   5
//------------------------------------------------------------------------------
// Codigo Fuente
//------------------------------------------------------------------------------

/**
 * @brief: Envia un comando HCI
 * 
 * Verifica el comando HCI, calcula su CRC (SLIP) y lo envia.
 * @param HCImsg: HCI message. In 0 the EID, in 1 the MID.
 * @param size: Size of the payload (starting in 2), NOT the full HCI message
 */
bool SendHCI (unsigned char* HCImsg, unsigned char size)
{   
    unsigned char destId=HCImsg[0];
    unsigned char msgId=HCImsg[1];
    unsigned int crc;
    //HCI LAYER - Validation
    if ((destId!=DEVMGMT_SAP_ID)||(destId!=LORAWAN_SAP_ID))
        return false;
    if (!msgId || msgId>0x38)
        return false;
    //SERIAL WRAPPING LAYER
    //CRC Generation and a bitwise negation of the result.
    crc= ~(CRC16_Calc(HCImsg,size+2,CRC16_INIT_VALUE));
    
    //Attach CRC16 and correct length, LSB first
    HCImsg[size++]=(unsigned char)(crc&0x00FF);
    HCImsg[size++]=(unsigned char)(crc>>8);

    //UART LAYER + SLIP ENCODING
    SerialDevice_SendByte(SLIP_END);
    for (unsigned char i=0;i<size;i++) {
        destId=HCImsg[i];   //Recycling of unused variable
        switch (destId) {
            case SLIP_END:
                SerialDevice_SendByte(SLIP_ESC);
                SerialDevice_SendByte(SLIP_ESC_END);
                break;
            case SLIP_ESC:
                SerialDevice_SendByte(SLIP_ESC);
                SerialDevice_SendByte(SLIP_ESC_ESC);
                break;
            default:
                SerialDevice_SendByte(destId);
                break;
        }
    }
    SerialDevice_SendByte(SLIP_END);
    
    return  SerialDevice_SendData(HCImsg,size+2);
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
            if (valor==SLIP_END)
                estado=1;
            break;
        case 1: //EID: Identificador de aplicacion, solo DEVMGMT o LORAWAN
            if ((valor==DEVMGMT_SAP_ID) || (valor==LORAWAN_SAP_ID)) {
                EID=valor;
                estado=2;
            } else if (valor==SLIP_END) {
                estado=1;
            }else {
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