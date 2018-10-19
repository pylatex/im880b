/*
 * La logica consiste en dos indices: uno de escritura de los buffer (asociado
 * a NMEAinput() ) y otra para la lectura de cada uno de los campos por parte
 * del usuario (asociada con NMEAselect() )
 */

#include <stdbool.h>
#include <string.h>
#include "nmea.h"

#define PTR_SZ  20  //Quantity of (pointers to) fields
#define BUFF_SZ 85  //Buffer Size (82 for typical NMEA message)

static enum {
    WaitingStart, Receiving, WaitingCheckHigh, WaitingCheckLow, BufferFull, ChecksumError
} stat = WaitingStart;  //FSM Status

static uint8_t      pers[PTR_SZ];   //Indices en que se encuentran ubicadas las comas
static uint8_t      buffer[BUFF_SZ+1];
static NMEAuser_t   intUserStat;
static uint8_t      CSgiven;    //Parsed CRC
static uint8_t      CScalc;     //Calculated CRC

static char NibbleVal (char in);
static void fallbackHandler();
static NMEArxHandler rxHandler = fallbackHandler;

//------------------------------------------------------------------------------
// CORE FUNCTIONS
//------------------------------------------------------------------------------

void regNMEAhandler (NMEArxHandler hdl) {
    //Sets the inicial state
    rxHandler = hdl;
}

uint8_t NMEAload (const uint8_t *message){
    while (*message) {
        NMEAinput(*message);
        message++;
    }
    return intUserStat.completeFields + 1u;
}

//State machine for NMEA String Decoding
void NMEAinput (uint8_t incomingByte) {
    static char count=0;

    if (incomingByte=='$' || incomingByte=='!') {
        //Cleans the buffer
        intUserStat.flags=0;    //Clear all flags
        intUserStat.completeFields=0; //Clears count of complete fields
        buffer[BUFF_SZ]=0;
        stat=WaitingStart;//Reader: Inactive
        intUserStat.DnEx = (unsigned)(incomingByte=='$');
        stat = Receiving;
        CScalc = 0;
        count=0;
    } else //A NMEA message may be in receiving course. Proceed based on status
    switch (stat) {

        case Receiving:
        case BufferFull:
            if (incomingByte=='*') {
                stat = WaitingCheckHigh;
                buffer[count] = 0;
            } else if (stat != BufferFull) {
                CScalc ^= incomingByte;
                if (incomingByte==','){
                    buffer[count] = 0;
                    pers[intUserStat.completeFields++] = count+1u;
                } else {
                    buffer[count] = incomingByte;
                }
                count++;
                if (count==BUFF_SZ) {
                    stat = BufferFull;
                }
            } else
                stat = WaitingStart;
            break;

        case WaitingCheckHigh:
            count = NibbleVal(incomingByte);    //Variable recycled
            if (count & 0xF0) {
                //Error
                stat = WaitingStart;
            } else {
                stat = WaitingCheckLow;
                CSgiven = (unsigned)((count << 4) | (count >> 4)); //would be optimized to SWAPF
            }
            break;

        case WaitingCheckLow:
            count = NibbleVal(incomingByte);    //Variable recycled
            if ( !(count & 0xF0) ) {
                CSgiven += count;
                if (CSgiven == CScalc)
                    rxHandler();
            }
            stat = WaitingStart;
            break;

        default:
            break;
    }
}

uint8_t NMEAgetCompletedFields(void) {
    return intUserStat.completeFields;
}

uint8_t *NMEAselect (uint8_t item) {
    //Si el buffer activo esta listo para leer
    if (item <= intUserStat.completeFields)
        return &buffer[item?pers[item - 1u]:0u];
    else
        return 0;
}

//------------------------------------------------------------------------------
// AUXILIAR FUNCTIONS
//------------------------------------------------------------------------------

static char NibbleVal (char in) {
    if (in >= '0' && in <= '9')
        return in-'0';
    else if (in >= 'A' && in <= 'F')
        return in-'A'+10;
    else if (in >= 'a' && in <= 'f')
        return in-'a'+10;
    return 0xFF;
}

static void fallbackHandler(void) {;}
