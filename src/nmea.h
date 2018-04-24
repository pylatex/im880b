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

#ifdef	__cplusplus
extern "C" {
#endif

    typedef volatile union {
        char reg;
        struct {
            unsigned    full            :1; //The buffer is full
            unsigned    checksumErr     :1; //Invalid checksum (given or calculated)
            unsigned    complete        :1; //Fully recognized message, ready to be read.
            unsigned    isCash          :1; //NMEA message begun with '$'
            unsigned    isExclam        :1; //NMEA message begun with '!'
        };
    } NMEAstatus;

    /**
     * Configures the NMEA decoder.
     * @param RxBuffer      Reception buffer.
     * @param RxBufferSize  Size of reception buffer.
     * @param statusReg     A status register the user can read.
     */
    void NMEAinit (char *RxBuffer,char RxBufferSize,NMEAstatus *statusReg);

    /**
     * Parses a NMEA message. Calls NMEAload() for every byte on the message.
     * @param phrase A pointer to the message to be read
     * @return The number of fields detected
     */
    char NMEAload (const char *phrase);

    /**
     * Input a char to the internal NMEA FSM.
     * @param incomingByte
     */
    void NMEAinput (char incomingByte);

    /**
     * Gives the position of a specified field
     * @param item: the field to be read, starting on 0
     * @return: first byte of the field, null if not a valid position.
     */
    char *NMEAselect (unsigned char item);

    /**
     * Allows to decode a new message.
     */
    void NMEArelease (void);

#ifdef	__cplusplus
}
#endif

#endif	/* GNS601_H */
