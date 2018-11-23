#include "serialSwitch.h"
#include "SerialDevice.h"

serial_t modoSerial;     //Elemento Serial que se esta controlando

void ppsLock (bool lock){
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = lock?1u:0u; // lock PPS
}

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
            RC6PPS=0;    //RC6 recibe el Latch de su puerto
            break;

        case DEBUG1:
            RC7PPS=0;
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
            RXPPS=0x0D;     //Rx viene de RB5
            RC6PPS=0x16;    //Tx va hacia RC6
            nuevaTasa = B9600;
            break;

        case DEBUG1:
            //Entradas y salidas UART
            RXPPS=0x0F;     //Rx viene de RB7
            RC7PPS=0x16;    //Tx va hacia RC7
            nuevaTasa = B9600;
            break;

        default:
            break;
    }
    ppsLock(true);
    #endif

    SerialDevice_Open("",nuevaTasa,8,0);
}

void compruebaModo (serial_t modo) {
    if (modoSerial != modo)
        cambiaSerial(modo);
}

void enviaIMST(char *arreglo,unsigned char largo) {
    compruebaModo(MODEM_LW);
    SerialDevice_SendData(arreglo,largo);
}

void enviaGPS(char *arreglo,unsigned char largo) {
    compruebaModo(GPS);
    SerialDevice_SendData(arreglo,largo);
}

void enviaDebug(char *arreglo,unsigned char largo) {
    compruebaModo(DEBUG1);
    SerialDevice_SendData(arreglo,largo);
}
