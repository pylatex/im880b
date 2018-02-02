#include "gns601.h"
#include "nmea.h"

/**
 * Decodifica la instruccion NMEA recibida por UART
 * @param data: Arreglo o posicion de memoria donde empieza la trama recibida
 * @param size: Longitud total de la trama recibida
 */
void processNMEA (unsigned char *data,unsigned char size) {
    if (size > 6 && data[0] == '$' && data[1] == 'G' && data[2] == 'P') {
        //NMEA GPS Command. Decode:
        if (data[3] == 'G' && data[4] == 'G' && data[5] == 'A') { //GGA
            // $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
        } else if (data[3] == 'G' && data[4] == 'L' && data[5] == 'L') { //GLL
            // $GPGLL,4916.45,N,12311.12,W,225444,A,*1D
        } else if (data[3] == 'G' && data[4] == 'S' && data[5] == 'V') { //GSV
            // $GPGSV,2,1,08,01,40,083,46,02,17,308,41,12,07,344,39,14,22,228,45*75
        } else if (data[3] == 'R' && data[4] == 'M' && data[5] == 'C') { //RMC
            // $GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
        } else if (data[3] == 'V' && data[4] == 'T' && data[5] == 'G') { //VTG
            // $GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48
        }
    }
}
