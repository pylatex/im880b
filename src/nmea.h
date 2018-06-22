/*
 * File:   nmea.h
 * Author: Alex Bustos
 *
 * Created on 20 de enero de 2018, 20:22
 *
 * Rutinas para el manejo de sentencias NMEA
 *
 */

#ifndef NMEA_H
#define	NMEA_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

    #define PTR_SZ  20  //Quantity of (pointers to) fields
    #define BUFF_SZ 85  //Buffer Size (82 for typical NMEA message)

    typedef struct {
        uint8_t         pers[PTR_SZ];   //Indices en que se encuentran ubicadas las comas
        uint8_t         buffer[BUFF_SZ];
        uint8_t         fields;       //Cantidad de comas
        uint8_t         CSgiven;      //Suma de verificacion (parseado de los Ultimos dos octetos)
        uint8_t         CScalc;       //Suma de verificacion (XOR entre $ y *, sin incluirlos)
    } NMEAbuff_t;

    typedef volatile union {
        char reg;
        struct {
            unsigned    full        :1; //Buffer(s) full
            unsigned    checksumErr :1; //Invalid checksum (given or calculated)
            unsigned    complete    :1; //Fully recognized message, ready to be read.
            unsigned    isDollar    :1; //NMEA message begun with '$'
            unsigned    isExclam    :1; //NMEA message begun with '!'
        };
    } NMEAstatus_t;
    
    /**
     * Configures the NMEA decoder.
     * @param statusReg     A status register the user can read.
     */
    void NMEAinit (NMEAstatus_t *statusReg);

    /**
     * Parses a NMEA message. Calls NMEAinput() for every byte on the message.
     * @param phrase A pointer to the message to be read
     * @return The number of fields detected 
     */
    uint8_t NMEAload (const uint8_t *phrase);

    /**
     * Input a char to the internal NMEA FSM.
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
     * Allows to decode a new message.
     */
    void NMEArelease (uint8_t buffNum);

#ifdef	__cplusplus
}
#endif

#endif	/* GNS601_H */
