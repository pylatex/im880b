/*
 * La logica consiste en dos indices: uno de escritura de los buffer (asociado
 * a NMEAinput() ) y otra para la lectura de cada uno de los campos por parte
 * del usuario (asociada con NMEAselect() )
 */

#include <stdbool.h>
#include "nmea.h"

#define PTR_SZ  20  //Quantity of (pointers to) fields
#define BUFF_SZ 85  //Buffer Size (82 for typical NMEA message)

typedef struct {
    uint8_t     pers[PTR_SZ];   //Indices en que se encuentran ubicadas las comas
    uint8_t     buffer[BUFF_SZ+1];
    NMEAuser_t  intUserStat;
} NMEAbuff_t;

typedef enum {
    WaitingStart, Receiving, WaitingCheckHigh, WaitingCheckLow, BufferFull, ChecksumError
} NMEA_fsm_stat_t;

typedef struct {
    NMEAuser_t   *UserReg;      // Pointer to User's State Indication
    struct {
        unsigned activeRx   :1; // Active Buffer for Reception
        unsigned activeRead :1; // Active Buffer for Reading
    };
    uint8_t CSgiven;    //Parsed CRC
    uint8_t CScalc;     //Calculated CRC
} NMEA_t;

static NMEA_t           NMEA;
static NMEAbuff_t       buff[2];
static NMEA_fsm_stat_t  stat;   //Current Status of the Internal State Machine

static char NibbleVal (char in);
static void updateExternalStatus();

void NMEAinit (NMEAuser_t *externalStatusObject) {
    //Set first buffer as active for both: reception and reading
    NMEA.activeRx=0;
    NMEA.activeRead=0;
    NMEA.UserReg=externalStatusObject; //Saves the reference to status object to use
    //Sets the inicial state
    buff[NMEA.activeRead].intUserStat.stat=IDLE;//Reader: Inactive
    buff[!NMEA.activeRead].intUserStat.stat=IDLE;//Reader: Inactive
    buff[NMEA.activeRead].intUserStat.flags=0;    //Clear all flags
    buff[NMEA.activeRead].intUserStat.completeFields=0; //Clears count of complete fields
    updateExternalStatus();
    buff[0].buffer[BUFF_SZ]=0;
    buff[1].buffer[BUFF_SZ]=0;
}

uint8_t NMEAload (const uint8_t *message){
    while (*message) {
        NMEAinput(*message);
        message++;
    }
    return buff[NMEA.activeRx].intUserStat.completeFields + 1u;
}

//State machine for NMEA String Decoding
void NMEAinput (uint8_t incomingByte) {
    static char count=0;
    if (incomingByte=='$' || incomingByte=='!') {
        //Check if the NMEA message that it's going to begin, can be stored.

        if (buff[NMEA.activeRx].intUserStat.completeFields) {
            //The current reception buffer has at least 1 complete field

            buff[NMEA.activeRx].intUserStat.stat=COMPLETE; //Mark Current Rx as complete
            if (buff[!NMEA.activeRx].intUserStat.stat == IDLE) {
                //Opposite buffer is idle, change to it:
                NMEA.activeRx = !NMEA.activeRx;
            } else {updateExternalStatus();return;}
        }
        //Cleans the (possibly changed) active Rx buffer
        buff[NMEA.activeRx].intUserStat.completeFields=0;
        buff[NMEA.activeRx].intUserStat.DnEx = (unsigned)(incomingByte=='$');
        stat = Receiving;
        count=0;
    } else //A NMEA message may be in receiving course. Proceed based on status
    switch (stat) {

        case Receiving:
        case BufferFull:
            if (incomingByte=='*') {
                stat = WaitingCheckHigh;
                buff[NMEA.activeRx].buffer[count] = 0;
            } else if (stat != BufferFull) {
                NMEA.CScalc ^= incomingByte;
                if (incomingByte==','){
                    buff[NMEA.activeRx].buffer[count] = 0;
                    buff[NMEA.activeRx].pers[buff[NMEA.activeRx].intUserStat.completeFields++] = count+1u;
                } else {
                    buff[NMEA.activeRx].buffer[count] = incomingByte;
                }
                count++;
                if (count==BUFF_SZ) {
                    stat = BufferFull;
                }
            }
            break;

        case WaitingCheckHigh:
            count = NibbleVal(incomingByte);    //Variable recycled
            if (count & 0xF0) {
                //Error
                stat = ChecksumError;
            } else {
                stat = WaitingCheckLow;
                NMEA.CSgiven = (unsigned)((count << 4) | (count >> 4)); //would be optimized to SWAPF
            }
            break;

        case WaitingCheckLow:
            count = NibbleVal(incomingByte);    //Variable recycled
            if (count & 0xF0) {
                //Error
                stat = ChecksumError;
            } else {
                stat = WaitingStart;
                NMEA.CSgiven += count;
                buff[NMEA.activeRx].intUserStat.CRCok = (NMEA.CSgiven == NMEA.CScalc)?1u:0u;
                buff[NMEA.activeRx].intUserStat.stat = COMPLETE;
                count = 0;
            }
            break;

        default:
            break;
    }
    updateExternalStatus();
}

uint8_t *NMEAselect (uint8_t item) {
    //Si el buffer activo esta listo para leer
    if (item <= buff[NMEA.activeRead].intUserStat.completeFields)
        return buff[NMEA.activeRead].buffer + (item?buff[NMEA.activeRead].pers[item-1]:0);
    else
        return 0;
}

/*
 * Funcion para el usuario.
 * Tiene que entenderse como si el usuario dijera "ya no necesito el mensaje que
 * me estas dando, dame el siguiente"
 * Internamente lo que se hace es pasar al siguiente buffer, para la lectura, y
 * de paso actualizar el registro de estado q indico el usuario.
 */
void NMEArelease () {
    buff[NMEA.activeRead].intUserStat.flags=0;    //Clear all flags
    buff[NMEA.activeRead].intUserStat.completeFields=0; //Clears count of complete fields
    buff[NMEA.activeRead].intUserStat.stat = IDLE;
    if (buff[!NMEA.activeRead].intUserStat.stat != IDLE)
        NMEA.activeRead = !NMEA.activeRead; //Choose the opposite buffer
    updateExternalStatus();
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

static void updateExternalStatus() {
    NMEA.UserReg->stat = buff[NMEA.activeRead].intUserStat.stat;
    NMEA.UserReg->flags = buff[NMEA.activeRead].intUserStat.flags;
    NMEA.UserReg->completeFields = buff[NMEA.activeRead].intUserStat.completeFields;
}

bool parseCoord2int(NMEAnumber *destination,uint8_t *number,uint8_t *direction){
    destination->mag = 0;
    destination->decimals = 0;
    uint8_t aux;
    bool punto = false;
    bool menos = false;

    while (*number) {
        if (*number == '.') {
            if (punto) return false;
            punto = true;
        } else if (*number == '-'){
            if (menos) return false;
            menos = true;
        } else if ((aux = *number - '0') < 10) {
            destination->mag *= 10;
            destination->mag += aux;
            if (punto) destination->decimals++;
        } else return false;
    }

    if (menos) destination->mag *= -1;

    switch (*direction) {
        case 'S':
        case 'W':
            destination->mag *= -1;
        case 'N':
        case 'E':
        case 'M':
            return true;

        default:
            return false;
    }
}

void fixDecimals(NMEAnumber *number,uint8_t decimals){
    if (number->decimals < decimals) {
        while (number->decimals < decimals) {
            number->mag *= 10;
            number->decimals++;
        }
    } else if (number->decimals > decimals) {
        while (number->decimals > decimals) {
            number->mag /= 10;
            number->decimals--;
        }
    }
}
