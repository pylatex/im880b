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

    typedef enum {
        IDLE,       //Waiting for a $ or ! NMEA beginning
        RECEIVING,  //May be available fields to be read. DnEx can be read.
        COMPLETE    //CRCgiven and CRCok can be read.
    } NMEA_stat_t;  //On any invalid condition, receiver goes to idle

    typedef volatile struct {
        NMEA_stat_t stat;
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
    
    typedef struct {
        int32_t mag;
        uint8_t decimals;
    } NMEAnumber;

    /**
     * Configures the NMEA decoder.
     * @param statusReg     A status object to be read by the user.
     */
    void NMEAinit (NMEAuser_t *statusReg);

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
     * Gives the position of a specified field
     * @param item: the field to be read, starting on 0
     * @return: first byte of the field, null if not a valid position.
     */
    uint8_t *NMEAselect (uint8_t item);

    /**
     * Releases the current message in read, and allows to decode a new message.
     */
    void NMEArelease ();
    
    bool parseCoord2int(NMEAnumber *destination,uint8_t *number,uint8_t *direction);

    void fixDecimals(NMEAnumber *number,uint8_t decimals);

#ifdef	__cplusplus
}
#endif

#endif	/* GNS601_H */
