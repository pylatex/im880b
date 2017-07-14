//------------------------------------------------------------------------------
// Includings
//------------------------------------------------------------------------------

#include "lorawan_hci.h"
#include "CRC16.h"
#include "SerialDevice.h"   //Acceso al puerto serie / UART

//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------
extern volatile unsigned char HCIstat;      //HCI Modules Status
extern volatile unsigned char HCIbuff[HCIBUFFSIZE];    //Tx/Rx Buffer

//------------------------------------------------------------------------------
// Section Source
//------------------------------------------------------------------------------

/**
 * @brief: Envia un comando HCI
 * 
 * Verifica el comando HCI, calcula su CRC (SLIP) y lo envia.
 * @param HCImsg: HCI message. In 0 the EID, in 1 the MID.
 * @param size: Size of the payload (starting in 2), NOT the full HCI message
 */
bool SendHCI (unsigned char *buffer, unsigned int size)
{   
    unsigned char aux=HCIWKUPCHARS;
    unsigned int crc;
    size+=2;
    //SERIAL WRAPPING LAYER
    //CRC Generation and a bitwise negation of the result.
    crc= ~(CRC16_Calc(buffer,size,CRC16_INIT_VALUE));
    
    //Attach CRC16 and correct length, LSB first
    buffer[size++]=(unsigned char)(crc&0x00FF);
    buffer[size++]=(unsigned char)(crc>>8);

    while(aux--)
        SerialDevice_SendByte(SLIP_END);
    //UART LAYER + SLIP ENCODING
    SerialDevice_SendByte(SLIP_END);
    for (unsigned char i=0;i<size;i++) {
        aux=buffer[i];   //Recycling of unused variable
        switch (aux) {
            case SLIP_END:
                SerialDevice_SendByte(SLIP_ESC);
                SerialDevice_SendByte(SLIP_ESC_END);
                break;
            case SLIP_ESC:
                SerialDevice_SendByte(SLIP_ESC);
                SerialDevice_SendByte(SLIP_ESC_ESC);
                break;
            default:
                SerialDevice_SendByte(aux);
                break;
        }
    }
    return (bool) SerialDevice_SendByte(SLIP_END);
}

/**
 * @brief Procesa un octeto recibido por el UART
 * 
 * Implementacion de una maquina de estados para el protocolo HCI en recepcion.
 */
signed char ProcessHCI (unsigned char *buffer, unsigned int rxData)
{
    //Variables
    static bool escape = false;
    static unsigned char size = 0;
    unsigned char idxret;
    
    if (rxData==SLIP_END) {
        idxret=((size >= 4) && CRC16_Check(buffer, size, CRC16_INIT_VALUE)) ? size - 4 : -1 ;
        size=0;
        escape=false;
        return idxret;  //Incomplete HCI-SLIP message
    } else {
        if (escape) {
            if (rxData==SLIP_ESC_END) {
                rxData=SLIP_END;
            } else if (rxData==SLIP_ESC_ESC) {
                rxData=SLIP_ESC;
            }
            buffer[size++]=rxData;
            escape=false;
        } else if (rxData==SLIP_ESC) {
            escape=true;
        } else {
            buffer[size++]=rxData;
            escape=false;
        }
        return -1;
    }
}