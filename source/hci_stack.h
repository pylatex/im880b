/*
 * File:    hci_stack.h
 * Author:  Alex F. Bustos
 * 
 * Headers for the implementation of the HCI+CRC16+SLIP+UART functions to be
 * used on WiMOD LoRaWAN EndNodes
 * 
 * Here should be the SLIP+CRC
 */

#ifndef LORAWAN_HCI_H
#define	LORAWAN_HCI_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdbool.h>

    //--------------------------------------------------------------------------
    //  Section CONST
    //--------------------------------------------------------------------------

    #define HCI_WKUPCHARS       5
    #define HCI_RX_RESETSTAT    -1
    #define HCI_RX_PENDING      -2

    // SLIP Protocol Characters
    #define SLIP_END        0xC0
    #define SLIP_ESC        0xDB
    #define SLIP_ESC_END    0xDC
    #define SLIP_ESC_ESC    0xDD
    
    //--------------------------------------------------------------------------
    //  Function Prototypes
    //--------------------------------------------------------------------------

    //TODO: Deberia de existir una funcion de lectura del buffer que permita
    //copiar hacia un arreglo determinado el contenido del mensaje, que retorne
    //el tamaño del mensaje y a su vez "libere" el buffer (habilitando
    //nuevamente la recepcion) para un nuevo mensaje.

    //State of the HCI receiver. -1:idle, -2:receiving, >=0: Size of the payload of a ready HCI message.
    signed char BuffSizeHCI (void);

    //Deprecated: True if the buffer can be readed. Please use a comparison against BuffSizeHCI()
    bool PendingRxHCI (void);

    //Use after reading the Rx buffer
    void ClearRxHCI (void);

    // Las siguientes tres son similares a las que tiene WiMOD_HCI_Layer.h

    //HCI Initialization
    bool InitHCI (void);

    //Envio de un comando HCI
    bool SendHCI (unsigned char *buffer, unsigned int size);

    //Procesamiento de HCI entrante.
    void ProcessHCI (unsigned char *buffer, unsigned int size);

#ifdef	__cplusplus
}
#endif

#endif	/* LORAWAN_HCI_H */