//------------------------------------------------------------------------------
// Inclusiones
//------------------------------------------------------------------------------

#include <stdbool.h>
#include "lorawan_hci.h"
#include "globaldefs.h" //Contexto de compilacion

//------------------------------------------------------------------------------
// Codigo Fuente
//------------------------------------------------------------------------------

/**
 * @brief Envia un comando HCI.
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
    static unsigned char estado = 0;
    return estado;    //Por el momento se informa que se recibio bien.
}