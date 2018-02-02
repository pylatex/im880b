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

char NMEAload (char *phrase, unsigned char len);
char *NMEAselect (unsigned char item);

#ifdef	__cplusplus
}
#endif

#endif	/* GNS601_H */
