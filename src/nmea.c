#include <stdbool.h>
#include "nmea.h"

typedef enum {
    WaitingStart,Receiving,WaitingCheckHigh,WaitingCheckLow,WaitingReading,BufferFull,ChecksumError
} NMEA_fsm_stat_t;

typedef volatile struct {
    NMEAstatus_t   *UserReg;    //
    union {
        uint8_t;
        struct {
            unsigned b1pending:1;  //
            unsigned b2pending:1;  //
        };
    };
} NMEA_t;

static NMEA_t           NMEA;
static NMEAbuff_t       buff[2];
static uint8_t          active;
static NMEA_fsm_stat_t  stat;

static char NibbleVal (char in);
static void iniciaBuff (NMEAbuff_t *buffer);

void NMEAinit (NMEAstatus_t *statusReg) {
    NMEA.UserReg=statusReg;
    NMEA.b1pending=false;
    NMEA.b2pending=false;
    NMEArelease(active=0);
    iniciaBuff(&buff[0]);
    iniciaBuff(&buff[1]);
}

char NMEAload (const char *phrase){
    NMEArelease(active=0);
    while (*phrase) {
        NMEAinput(*phrase);
        phrase++;
    }
    return buff[active].fields;
}

NMEAbuff_t *pendingNMEAbuffer (void) {
    return &buff[active];
}

//State machine for NMEA String Decoding
void NMEAinput (char incomingByte) {
    static char count=0;
    static enum {none,cash,exclam} type=none;

    switch (stat) {

        case WaitingStart:
            if (incomingByte=='$' || incomingByte=='!'){
                if (incomingByte=='$')
                    type=cash;
                else
                    type=exclam;
                stat = Receiving;
                count=0;
            }
            break;

        case Receiving:
            if (incomingByte=='*') {
                stat = WaitingCheckHigh;
                buff[active].buffer[count] = 0;
            } else if (incomingByte!='$' || incomingByte!='!') {
                buff[active].CScalc ^= incomingByte;
                if (incomingByte==','){
                    buff[active].buffer[count] = 0;
                    buff[active].pers[buff[active].fields++] = count+1u;
                } else {
                    buff[active].buffer[count] = incomingByte;
                }
                count++;
                if (count==BUFF_SZ) {
                    stat = BufferFull;
                    NMEA.UserReg->full = 1;
                }
            }
            break;

        case BufferFull:
            if (incomingByte=='*') {
                stat = WaitingCheckHigh;
            } else {
            }
            break;

        case WaitingCheckHigh:
            count = NibbleVal(incomingByte);    //Variable recycled
            if (count & 0xF0) {
                //Error
                stat = ChecksumError;
            } else {
                stat = WaitingCheckLow;
                buff[active].CSgiven = (unsigned char)((count << 4) | (count >> 4)); //would be optimized to SWAPF
            }
            break;

        case WaitingCheckLow:
            count = NibbleVal(incomingByte);    //Variable recycled
            if (count & 0xF0) {
                //Error
                stat = ChecksumError;
            } else {
                stat = WaitingReading;
                buff[active].CSgiven += count;
                NMEA.UserReg->checksumErr= (buff[active].CSgiven == buff[active].CScalc)?0u:1u;
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
    if (item < buff[active].fields)
        return buff[active].buffer + buff[active].pers[item];
    else
        return 0;
}

void NMEArelease (uint8_t buffNum) {
    NMEA.UserReg->reg=0;
    stat=WaitingStart;
    buff[buffNum].fields=1;
    buff[buffNum].CScalc=0;
}

static char NibbleVal (char in) {
    if (in >= '0' && in <= '9')
        return in-'0';
    else if (in >= 'A' && in <= 'F')
        return in-'A'+10;
    else if (in >= 'a' && in <= 'f')
        return in-'a'+10;
    return 0xFF;
}

static void iniciaBuff (NMEAbuff_t *buffer) {
    buffer->fields=0;
    buffer->CSgiven=0;
    buffer->CScalc=0;
}
