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
    unsigned int crc;
    
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
    unsigned char idxret=0;
    unsigned int crcheck;
    
    if (valor==SLIP_END) {
        idxret=-1;
        if (idx>=4 && CRC16_Check(HCImsg, idx, CRC16_INIT_VALUE)) {   //Dst+Msg+CRC
            idxret=idx-4;   //Payload Size
        }
        idx=0;
        escape=false;
        return idxret;  //Incomplete HCI-SLIP message
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