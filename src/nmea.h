/*
 * File:   nmea.h
 * Author: Alex Bustos
 *
 * Created on 20 de enero de 2018, 20:22
 *
 * Rutinas para el manejo de sentencias NMEA
 *
 * La mecanica consiste en que el usuario cree un elemento de tipo NMEAuser_t y
 * lo asocie a la libreria mediante NMEAinit(), para con este elemento verificar
 * el estado del sistema y, de acuerdo al estado, se puede leer los campos
 * detectados con NMEAselect(), inclusive sin haber terminado de llegar el
 * mensaje, permitiendo descartar el mensaje.
 *
 * Una vez terminados de leer los campos necesarios, se puede liberar el mensaje
 * NMEA con NMEArelease().
 */

#ifndef NMEA_H
#define	NMEA_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct {
        int32_t mag;
        uint8_t decimals;
    } NMEAnumber;

    typedef volatile struct {
        union {
            uint8_t flags;
            struct {
            unsigned CRCgiven   :1; //True if checksum given
            unsigned CRCok      :1; //True if given checksum is same as calculated
            unsigned DnEx       :1; //True for Dollar sign, false for Exclamation sign
            };
        };
        uint8_t completeFields; //Complete fields detected (by a ',' or '*')
    } NMEAuser_t;
    
    typedef void (*NMEArxHandler)(void);

    /**
     * Pass the NMEA reception handler.
     * @param hdl: The function who will handle the reception of NMEA strings
     */
    void regNMEAhandler (NMEArxHandler hdl);

    /**
     * Parses a NMEA message. Calls NMEAinput() for every byte on the message.
     * @param phrase A pointer to the message to be read
     * @return The number of fields detected
     */
    uint8_t NMEAload (const uint8_t *message);

    /**
     * Input an octect to the internal NMEA FSM.
     * @param incomingByte
     */
    void NMEAinput (uint8_t incomingByte);

    /**
     * Get the quantity of completed fields (finished by a ',' or by '*')
     * @return quantity of completed fields
     */
    uint8_t NMEAgetCompletedFields(void);

    /**
     * Gives the position of a specified field
     * @param item: the field to be read, starting on 0
     * @return: first byte of the field, null if not a valid position.
     */
    uint8_t *NMEAselect (uint8_t item);


#ifdef	__cplusplus
}
#endif

#endif	/* GNS601_H */
