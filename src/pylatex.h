/* 
 * File:   pylatex.h
 * Author: Alex Bustos
 *
 * Created on February 7, 2018, 12:28 PM
 */

#ifndef PYLATEX_H
#define	PYLATEX_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdbool.h>
    #include "WiMOD_LoRaWAN_API.h"
                                            //MEDIDA             FORMATO
    #define PY_CO2              1           //CO2                   A
    #define PY_TVOC             2           //TVOC                  A
    #define PY_PM025            3           //PM 2.5                A
    #define PY_PM100            4           //PM 10                 A
    #define PY_RH1              5           //RH                    A --> B
    #define PY_TEMP1            6           //TEMP                  A --> B
    #define PY_ILUM1            7           //LUX                   A/1.2
    #define PY_GAS              8           //GAS                   A
    #define PY_ILUM2            9           //LUX                   A/2.4
    #define PY_COMP1            10          //Presion + Temp        E
    #define PY_PRESS1           PY_COMP1    //DEPRECADO             E
    #define PY_PRECIPITATION    11          //                      A
    #define PY_WINDSPEED        12          //                      A
    #define PY_GPS              13          //Posicion              F
    #define PY_DISTANCE         14          //Distancia

    typedef void (*delayHandlerFunction)(unsigned char mscant);
    typedef enum {PY_UNCONFIRMED=0,PY_CONFIRMED} pyModeType;

    /**
     * Inicializa la aplicacion LoRaWAN para el envio de datos de sensores.
     * @param transmitter: Funcion que transmite un byte por UART
     */
    void initLW (serialTransmitHandler transmitter, LWstat *LWstatus);

    /**
     * Intenta agregar una medida a la cola de envio.
     * @param variable: Constante que indica el tipo de medida
     * @param medida: Posicion en memoria en que inicia la carga util a enviar
     * @return true o false dependiendo de si hubo o no, espacio para la medida.
     */
    bool AppendMeasure (char variable,char *medida);

    /**
     * Envia los datos de los sensores anexados.
     * @param mode: Modo de envio del mensaje, confirmado o no.
     */
    void SendMeasures (pyModeType mode);

    /**
     * Convierte un short (uint16) a un arreglo de sus octetos.
     * @param in: El entero a ser desglosado
     * @return : Puntero a un arreglo de los octetos, MSB primero.
     */
    char *short2charp (unsigned short in);

    /**
     * Usar despues de la inicializacion para registrar una funcion de demora.
     * @param delfun: Funcion de demora
     * @param flag: Puntero a la bandera a leer para saber que la demora esta en curso.
     */
    void registerDelayFunction(delayHandlerFunction delfun,volatile bool *flag);

    /**
     * Llamar en cada octeto a la entrada del UART, cuando provenga del iM880B
     * @param RxByteUART: El octeto recibido por UART
     */
    void pylatexRx (char RxByteUART);

    /**
     * Vacia las mediciones a enviar por LoRaWAN. Automaticamente llamado por
     * initLoraApp() y SendMeasures().
     */
    void clearAppPayload(void);

#ifdef	__cplusplus
}
#endif

#endif	/* PYLATEX_H */
