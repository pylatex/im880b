#include "nmea.h"

#define PTR_SZ  20

volatile static char    pers[PTR_SZ];   //Indices en que se encuentran ubicadas las comas

typedef enum {
    WaitingStart,Receiving,WaitingCheckHigh,WaitingCheckLow,WaitingReading,BufferFull,ChecksumError
} NMEA_fsm_stat_t;

typedef volatile struct {
    char           *buffer;
    char            buffsize;
    char            fields;       //Cantidad de comas
    unsigned char   CSgiven;      //Suma de verificacion (parseado de los Ultimos dos octetos)
    unsigned char   CScalc;       //Suma de verificacion (XOR entre $ y *, sin incluirlos)
    NMEA_fsm_stat_t stat;
    NMEAstatus     *UserReg;
} NMEA_t;

static NMEA_t NMEA;

char NibbleVal (char in);

void NMEAinit (char *RxBuffer,char RxBufferSize,NMEAstatus *statusReg) {
    NMEA.buffer=RxBuffer;
    NMEA.buffsize=RxBufferSize;
    NMEA.UserReg=statusReg;
    NMEArelease();
    pers[0]=0;
}

char NMEAload (const char *phrase){
    NMEArelease();
    while (*phrase) {
        NMEAinput(*phrase);
        phrase++;
    }
    return NMEA.fields;
}

void NMEAinput (char incomingByte) {
    static char count=0;
    static enum {none,cash,exclam} type=none;

    switch (NMEA.stat) {

        case WaitingStart:
            if (incomingByte=='$' || incomingByte=='!'){
                if (incomingByte=='$')
                    type=cash;
                else
                    type=exclam;
                NMEA.stat = Receiving;
                count=0;
            }
            break;

        case Receiving:
            if (incomingByte=='*') {
                NMEA.stat = WaitingCheckHigh;
                NMEA.buffer[count] = 0;
            } else if (incomingByte!='$' || incomingByte!='!') {
                NMEA.CScalc ^= incomingByte;
                if (incomingByte==','){
                    NMEA.buffer[count] = 0;
                    pers[NMEA.fields++] = count+1u;
                } else {
                    NMEA.buffer[count] = incomingByte;
                }
                count++;
                if (count==NMEA.buffsize) {
                    NMEA.stat = BufferFull;
                    NMEA.UserReg->full = 1;
                }
            }
            break;

        case BufferFull:
            if (incomingByte=='*') {
                NMEA.stat = WaitingCheckHigh;
            } else {
            }
            break;

        case WaitingCheckHigh:
            count = NibbleVal(incomingByte);    //Variable recycled
            if (count & 0xF0) {
                //Error
                NMEA.stat = ChecksumError;
            } else {
                NMEA.stat = WaitingCheckLow;
                NMEA.CSgiven = (unsigned char)((count << 4) | (count >> 4)); //would be optimized to SWAPF
            }
            break;

        case WaitingCheckLow:
            count = NibbleVal(incomingByte);    //Variable recycled
            if (count & 0xF0) {
                //Error
                NMEA.stat = ChecksumError;
            } else {
                NMEA.stat = WaitingReading;
                NMEA.CSgiven += count;
                NMEA.UserReg->checksumErr= (NMEA.CSgiven == NMEA.CScalc)?0u:1u;
                NMEA.UserReg->isCash=type == cash;
                NMEA.UserReg->isExclam=type == exclam;
                NMEA.UserReg->complete=1;
                type = none;
            }
            break;

        default:
            break;
    }
}

char *NMEAselect (unsigned char item) {
    if (item < NMEA.fields)
        return NMEA.buffer + pers[item];
    else
        return 0;
}

void NMEArelease (void) {
    NMEA.UserReg->reg=0;
    NMEA.stat=WaitingStart;
    NMEA.fields=1;
    NMEA.CScalc=0;
}

char NibbleVal (char in) {
    if (in >= '0' && in <= '9')
        return in-'0';
    else if (in >= 'A' && in <= 'F')
        return in-'A'+10;
    else if (in >= 'a' && in <= 'f')
        return in-'a'+10;
    return 0xFF;
}
