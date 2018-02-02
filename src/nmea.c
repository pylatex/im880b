#include "nmea.h"

#define PTR_SZ  20

typedef struct {
    char         *pers[PTR_SZ]; //Indices en que se encuentran ubicadas las comas
    unsigned char fields;      //Cantidad de comas
    unsigned int  CSgiven;      //Suma de verificacion (parseado de los Ultimos dos octetos)
    unsigned char CScalc;       //Suma de verificacion (XOR entre $ y *)
} NMEA_t;

static NMEA_t NMEA;
unsigned char NibbleVal (unsigned char in);

char NMEAload (char *phrase, unsigned char len) {
    unsigned char aux=0;
    unsigned int sum=0;

    //Detect and skip the beginning of NMEA content
    while (len) {
        if (*phrase=='$' || *phrase=='!') {
            if (!aux) aux=1;
        } else {
            if (aux) break;
        }

        phrase++;
        len--;
    }
    NMEA.CScalc=0;
    NMEA.fields=0;
    while (len && NMEA.fields<PTR_SZ) {
        if (aux==2) {
            //End of NMEA sentence
            NMEA.CSgiven=(NibbleVal(*phrase) << 4) + NibbleVal(*(phrase+1));
            break;
        } else {
            NMEA.CScalc ^= *phrase;
            if (aux)
                NMEA.pers[NMEA.fields++]=phrase;
            switch (*phrase) {
                case ',':
                    aux=1;
                    break;
                case '*':
                    aux=2;
                    NMEA.CScalc ^= '*'; //Restaurar el checksum del asterisco.
                    break;
                default:
                    aux=0;
                    break;
            }
            if (aux)
                *phrase=0;
        }
        phrase++;
        len--;
    }

    return NMEA.fields;
}

char *NMEAselect (unsigned char item) {
    if (item <= NMEA.fields)
        return NMEA.pers[item];
    else
        return 0;
}

unsigned char NibbleVal (unsigned char in) {
    if (in >= '0' && in <= '9')
        return in-'0';
    else if (in >= 'A' && in <= 'F')
        return in-'A'+10;
    else if (in >= 'a' && in <= 'f')
        return in-'a'+10;
    return 0xFF;
}