#include <stdbool.h>
#include "nmea.h"

typedef enum {
    WaitingStart,Receiving,WaitingCheckHigh,WaitingCheckLow,WaitingReading,BufferFull,ChecksumError
} NMEA_fsm_stat_t;

typedef volatile struct {
    NMEAstatus_t   *UserReg;        // Pointer to User's State Indication
    union {
        uint8_t;
        struct {
            unsigned b1pending:1;   // Buffer 1 is ready to be read
            unsigned b2pending:1;   // Buffer 2 is ready to be read
            unsigned activeRx:1;    // Active Buffer for Reception
            unsigned activeRead:1;  // Active Buffer for Reading
        };
    };
} NMEA_t;

static NMEA_t           NMEA;
static NMEAbuff_t       buff[2];
static NMEA_fsm_stat_t  stat;   //Current Status of the Internal State Machine

static char NibbleVal (char in);
static void iniciaBuff (NMEAbuff_t *buffer);
static void iniciaStat (NMEAbuff_t *buffer);

void NMEAinit (NMEAstatus_t *statusReg) {
    NMEA.UserReg=statusReg;
    NMEA.b1pending=false;
    NMEA.b2pending=false;
    NMEA.activeRx=0;
    NMEA.activeRead=0;
    NMEArelease(0);
    iniciaBuff(&buff[0]);
    iniciaBuff(&buff[1]);
}

uint8_t NMEAload (const uint8_t *phrase){
    NMEArelease(NMEA.activeRx=0);
    while (*phrase) {
        NMEAinput(*phrase);
        phrase++;
    }
    return buff[NMEA.activeRx].fields;
}

NMEAbuff_t *pendingNMEAbuffer (void) {
    return &buff[NMEA.activeRx];
}

//State machine for NMEA String Decoding
void NMEAinput (uint8_t incomingByte) {
    static char count=0;
    static enum {NONE,DOLLAR,EXCLAM} type=NONE;

    if ((NMEA.activeRx == 0 && !NMEA.b1pending) || (NMEA.activeRx == 1 && !NMEA.b2pending))
    switch (stat) {

        case WaitingStart:
            if (incomingByte=='$' || incomingByte=='!'){
                type = (incomingByte=='$') ? DOLLAR : EXCLAM;
                stat = Receiving;
                count=0;
            }
            break;

        case Receiving:
            if (incomingByte=='*') {
                stat = WaitingCheckHigh;
                buff[NMEA.activeRx].buffer[count] = 0;
            } else if (incomingByte!='$' || incomingByte!='!') {
                buff[NMEA.activeRx].CScalc ^= incomingByte;
                if (incomingByte==','){
                    buff[NMEA.activeRx].buffer[count] = 0;
                    buff[NMEA.activeRx].pers[buff[NMEA.activeRx].fields++] = count+1u;
                } else {
                    buff[NMEA.activeRx].buffer[count] = incomingByte;
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
                buff[NMEA.activeRx].CSgiven = (unsigned char)((count << 4) | (count >> 4)); //would be optimized to SWAPF
            }
            break;

        case WaitingCheckLow:
            count = NibbleVal(incomingByte);    //Variable recycled
            if (count & 0xF0) {
                //Error
                stat = ChecksumError;
            } else {
                stat = WaitingReading;
                buff[NMEA.activeRx].CSgiven += count;
                NMEA.UserReg->checksumErr= (buff[NMEA.activeRx].CSgiven == buff[NMEA.activeRx].CScalc)?0u:1u;
                NMEA.UserReg->isDollar=type == DOLLAR;
                NMEA.UserReg->isExclam=type == EXCLAM;
                NMEA.UserReg->complete=1;
                type = NONE;
            }
            break;

        default:
            break;
    }
}

uint8_t *NMEAselect (uint8_t item) {
    //Si el buffer activo esta listo para leer
    if (item < buff[NMEA.activeRx].fields)
        return buff[NMEA.activeRx].buffer + buff[NMEA.activeRx].pers[item];
    else
        return 0;
}

//Borrar la bandera de que el buffer activo esta listo para leer
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
