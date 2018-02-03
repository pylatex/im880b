#ifndef CRC16MODBUS_H_INCLUDED
#define CRC16MODBUS_H_INCLUDED

/**
 * Calculates the CRC of a MODBUS message
 * @param puchMsg message to calculate CRC upon
 * @param usDataLen quantity of bytes in message
 * @return the CRC as a unsigned short type
 */
unsigned short CRC16modbus ( unsigned char *puchMsg, unsigned short usDataLen);

#endif // CRC16MODBUS_H_INCLUDED
