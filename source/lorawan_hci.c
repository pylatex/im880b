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
bool SendHCI (unsigned char *HCImsg, unsigned char size)
{   
    unsigned char destId;
    //unsigned char msgId;
    unsigned int crc;
    /*
    destId=HCImsg[0];
    //HCI LAYER - Validation
    if ((destId!=DEVMGMT_SAP_ID)||(destId!=LORAWAN_SAP_ID))
        return false;
    msgId=HCImsg[1];
    if ((!msgId) || (msgId>0x38))
        return false;
     */
    //SERIAL WRAPPING LAYER
    //CRC Generation and a bitwise negation of the result.
    crc= ~(CRC16_Calc(HCImsg,size+2,CRC16_INIT_VALUE));
    
    //Attach CRC16 and correct length, LSB first
    HCImsg[2 + size++]=(unsigned char)(crc&0x00FF);
    HCImsg[2 + size++]=(unsigned char)(crc>>8);

    //UART LAYER + SLIP ENCODING
    SerialDevice_SendByte(SLIP_END);
    for (unsigned char i=0;i<size+2;i++) {
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
    return SerialDevice_SendByte(SLIP_END);
}

/**
 * @brief Procesa un octeto recibido por el UART
 * 
 * Implementacion de una maquina de estados para el protocolo HCI en recepcion.
 */
signed char ProcessHCI (unsigned char *HCImsg, unsigned char valor)
{
    //Variables
    static bool escape = false;
    static unsigned char idx= 0;
    
    if (valor==SLIP_END) {
        if (idx>=4) {   //Dst+Msg+CRC
            //Verificacion del CRC
            
            return idx-4;   //Payload Size
        } else {
            idx=0;
            escape=false;
            return -1;  //Incomplete HCI-SLIP message
        }
    } else {
        if (escape) {
            if (valor==SLIP_ESC_END) {
                valor=SLIP_END;
            } else if (valor==SLIP_ESC_ESC) {
                valor=SLIP_ESC;
            }
            HCImsg[idx++]=valor;
            escape=false;
        } else if (valor==SLIP_ESC) {
            escape=true;
        } else {
            HCImsg[idx++]=valor;
            escape=false;
        }
        return -1;
    }
}