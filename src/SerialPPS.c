#include "SerialPPS.h"

/**
 * Cambia la asignacion interna del EUSART
 * @param serial: El elemento con que se desea conectar (vease serial_t arriba)
 */
void cambiaSerial (serial_t serial){
    baudrate_t nuevaTasa;
    SerialDevice_Close();

    #ifdef _16F1769
    ppsLock(false);
    //Desligar las salidas del serial
    switch (modoSerial) {
        case MODEM_LW:
            RC4PPS=0;    //RC4 recibe el Latch de su puerto
            break;

        case GPS:
            //Entradas y salidas UART
            RB5PPS=0;    //RB5 recibe el Latch de su puerto
            break;

        default:
            break;
    }
    //Ligar las nuevas salidas y entradas al modulo UART
    switch (modoSerial=serial) {
        case MODEM_LW:
            //Entradas y salidas UART
            RXPPS=0x15;     //Rx viene de RC5
            RC4PPS=0x16;    //Tx va hacia RC4
            nuevaTasa = B115200;
            break;

        case GPS:
            //Entradas y salidas UART
            RXPPS=0x16;     //Rx viene de RC6
            RB5PPS=0x16;    //Tx va hacia RB5
            nuevaTasa = B9600;
            break;

        default:
            break;
    }
    ppsLock(true);
    #endif

    SerialDevice_Open("",nuevaTasa,8,0);
}

void enviaIMST(char *arreglo,unsigned char largo) {
    if (modoSerial != MODEM_LW)
        cambiaSerial(MODEM_LW);
    SerialDevice_SendData(arreglo,largo);
}

void enviaGPS(char *arreglo,unsigned char largo) {
    if (modoSerial != GPS)
        cambiaSerial(GPS);
    SerialDevice_SendData(arreglo,largo);
}