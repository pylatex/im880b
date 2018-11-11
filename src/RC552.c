
#include "spi.h"
#include "nucleoPIC.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "RC522.h"


unsigned char     readData;
unsigned char     readDummy;



bool isCard(void) 
 {
	unsigned char status;
	unsigned char str[MAX_LEN];
     
    
    // Reset baud rates
	writeMFRC522 (TxModeReg, 0x00);
	writeMFRC522(RxModeReg, 0x00);
	// Reset ModWidthReg
	writeMFRC522(ModWidthReg, 0x26);
	
    status = MFRC522Request(PICC_REQIDL, str);	// REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
    
    
    if (status == MI_OK) {
		return true;
	} else { 
		return false; 
	}
 }

bool readCardSerial(void){

	unsigned char status;
	unsigned char str[MAX_LEN];

	// Devuelve el numero de serie de la tarjeta
	status = anticoll(str);
	memcpy(serNum, str, 5); //Destino, Fuente, Tamaño -> Pone el contenido de str en la cadena de caracteres SerNum

	if (status == MI_OK) {
		return true;
	} else {
		return false;
	}

}

void init(void)
{

    SPI_Initialize();
    
    RST = true; //Equivalente a digitalWrite(_NRSTPD,HIGH)
    SS = true;
    
    RST=false;		// Make shure we have a clean LOW state.
    __delay_us(2);				// 8.8.1 Reset timing requirements says about 100ns. Let us be generous: 2?sl
	RST=true;		// Exit power down mode. This triggers a hard reset.
			// Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74?s. Let us be generous: 50ms.
	__delay_ms(50);
        
	reset();
    
    // Reset baud rates
	writeMFRC522(TxModeReg, 0x00);
	writeMFRC522(RxModeReg, 0x00);
	// Reset ModWidthReg
	writeMFRC522(ModWidthReg, 0x26);
	//Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    writeMFRC522(TModeReg, 0x80);		//Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    writeMFRC522(TPrescalerReg, 0xA9);	//TModeReg[3..0] + TPrescalerReg
    writeMFRC522(TReloadRegL, 0xE8);           
    writeMFRC522(TReloadRegH, 0x03);
	writeMFRC522(TxAutoReg, 0x40);		//100%ASK
	writeMFRC522(ModeReg, 0x3D);		// CRC valor inicial de 0x6363
	antennaOn();		//despierta la antena
}

 void reset(void)
{
     
	writeMFRC522(CommandReg, PCD_RESETPHASE);
    
    	// Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74?s. Let us be generous: 50ms.
	char count = 0;
	do {
		// Wait for the PowerDown bit in CommandReg to be cleared (max 3x50ms)
		__delay_ms(50);
	} while ((readMFRC522(CommandReg) & (1 << 4)) && (++count) < 3);
    
    
}


void writeMFRC522( unsigned char addr, unsigned char value )
{       
        //Se pone en 0 para habilitar
        SS = false;
        readData = SPI_Exchange8bit( ( addr << 1 ) & 0x7E );//Equivalente a SPI.transfer((addr<<1)&0x7E);
        readDummy = SPI_Exchange8bit( value );// Equivalente a  SPI.transfer(val);
        SS = true;
        //Se pone en 1 para deshabilitar
}

unsigned char readMFRC522( unsigned char addr )
{
unsigned char value;
        //Activa con c0
        SS = false;
        readData = SPI_Exchange8bit(((addr<<1)&0x7E)|0x80);// Equivalente a SPI.transfer(((addr<<1)&0x7E) | 0x80);
        value = SPI_Exchange8bit(0x00); // Equivalente a val = SPI.transfer(0x00);
        SS = true;
        //Desactiva con C1
return value;
}

  void antennaOn(void)
{
	unsigned char temp;

	temp = readMFRC522(TxControlReg);
	if ((temp & 0x03) != 0x03) {
		writeMFRC522(TxControlReg, temp | 0x03);
	}
}


  void setBitMask(unsigned char reg, unsigned char mask)  
{ 
    unsigned char tmp;
    tmp = readMFRC522(reg);
    writeMFRC522(reg, tmp | mask);  // set bit mask
}

  void clearBitMask(unsigned char reg, unsigned char mask)  
{
    unsigned char tmp;
    tmp = readMFRC522(reg);
    writeMFRC522(reg, tmp & (~mask));  // clear bit mask
} 

  bool calculateCRC(unsigned char * data,		///< In: Pointer to the data to transfer to the FIFO for CRC calculation.
                    unsigned char length,	///< In: The number of bytes to transfer.
					unsigned char *result	///< Out: Pointer to result buffer. Result is written to result[0..1], low byte first.
					 ) {
	writeMFRC522(CommandReg, 0x00);		// Stop any active command.
	writeMFRC522(DivIrqReg, 0x04);				// Clear the CRCIRq interrupt request bit
	writeMFRC522(FIFOLevelReg, 0x80);			// FlushBuffer = 1, FIFO initialization
	writeMFRC522(FIFODataReg, (*data));	// Write data to the FIFO
	writeMFRC522(CommandReg, 0x03);		// Start the calculation
	
	// Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73?s.
	// TODO check/modify for other architectures than Arduino Uno 16bit

	// Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73us.
	unsigned char  n;
    for (uint16_t i = 5000; i > 0; i--) {
		// DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq reserved CRCIRq reserved reserved
		n = readMFRC522(DivIrqReg);
		if (n & 0x04) {									// CRCIRq bit set - calculation done
			writeMFRC522(CommandReg, 0x00);	// Stop calculating CRC for new content in the FIFO.
			// Transfer the result from the registers to the result buffer
			result[0] = readMFRC522(CRCResultRegL);
			result[1] = readMFRC522(CRCResultRegH);
			return MI_OK;
		}
	}
	// 89ms passed and nothing happend. Communication with the MFRC522 might be down.
	return MI_ERR;
  
  }

unsigned char MFRC522ToCard(unsigned char command, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned char *backLen){
    unsigned char irqEn;
    unsigned char waitIRq;
	unsigned char lastBits;
    unsigned char n;
    uint16_t i;
    char validBits=7;
    char txLastBits = validBits ? validBits : 0;
	char bitFraming = (0x0 << 4) + lastBits;
    
    switch (command)
    {
        case PCD_AUTHENT:		// Tarjetas de certificaciÃ?Â³n cerca
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE:	//La transmisiÃ?Â³n de datos FIFO
		{
			irqEn = 0x77;
			waitIRq = 0x30;//Esta bien
			break;
		}
		default:
			break;
    } 
    
    writeMFRC522(CommandReg, 0x0);			// Stop any active command.
	writeMFRC522(0x04<<1, 0x7F);					// Clear all seven interrupt request bits
	writeMFRC522(FIFOLevelReg, 0x80);				// FlushBuffer = 1, FIFO initialization
	writeMFRC522(FIFODataReg, sendData);	// Write sendData to the FIFO
	writeMFRC522(BitFramingReg, bitFraming);		// Bit adjustments
	writeMFRC522(CommandReg, command);				// Execute the command
	if (command == PCD_TRANSCEIVE) {
		setBitMask(BitFramingReg, 0x80);	// StartSend=1, transmission of data starts
	}
	for (i = 2000; i > 0; i--) {
        n = readMFRC522(0x04 << 1);	// ComIrqReg[7..0] bits are: Set1 TxIRq RxIRq IdleIRq HiAlertIRq LoAlertIRq ErrIRq TimerIRq
		if (n & waitIRq) {					// One of the interrupts that signal success has been set.
			break;
		}
		if (n & 0x01) {						// Timer interrupt - nothing received in 25ms
			return MI_ERR;
		}
	}
	// 35.7ms and nothing happend. Communication with the MFRC522 might be down.
	if (i == 0) {
		return MI_ERR;
	}
	// Stop now if any errors except collisions were detected.
	char errorRegValue = readMFRC522(ErrorReg); // ErrorReg[7..0] bits are: WrErr TempErr reserved BufferOvfl CollErr CRCErr ParityErr ProtocolErr
	if (errorRegValue & 0x13) {	 // BufferOvfl ParityErr ProtocolErr
		return MI_ERR;
	}
	char _validBits = 0;
	// If the caller wants data back, get it from the MFRC522.
	if (backData && backLen) {
		 n = readMFRC522(FIFOLevelReg);	// Number of bytes in the FIFO
		if (n > *backLen) {
			return MI_ERR;
		}
		*backLen = n;											// Number of bytes returned
		_validBits = readMFRC522(ControlReg) & 0x07;		// RxLastBits[2:0] indicates the number of valid bits in the last received byte. If this value is 000b, the whole byte is valid.
		if (validBits) {
			validBits = _validBits;
		}
	}// Tell about collisions
	if (errorRegValue & 0x08) {		// CollErr
		return MI_ERR;  
	}
	// Perform CRC_A validation if requested.
	if (backData && backLen && 1) {
		// In this case a MIFARE Classic NAK is not OK.
		if (*backLen == 1 && _validBits == 4) {
			return MI_ERR;
		}
		// We need at least the CRC_A value and all 8 bits of the last byte must be received.
		if (*backLen < 2 || _validBits != 0) {
			return MI_ERR;
		}
		// Verify CRC_A - do our own calculation and store the control in controlBuffer.
		char controlBuffer[2];
		if (calculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]) != MI_OK) {
			return MI_ERR;
		}
		if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1])) {
			return MI_ERR;
		}
	}
	return MI_OK;
}
unsigned char  MFRC522Request(unsigned char reqMode, unsigned char *TagType)
{
	unsigned char status;  
	uint16_t backBits;			//   RecibiÃ?Â³ bits de datos
    
    clearBitMask(CollReg, 0x80);		// ValuesAfterColl=1 => Bits received after collision are cleared.
	
	TagType[0] = reqMode;
	status = MFRC522ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != MI_OK) || (backBits != 0x10))
	{    
		status = MI_ERR;
	}

	return status;
}


unsigned char anticoll(unsigned char *serNum)
{
    unsigned char status;
    unsigned char i;
	unsigned char serNumCheck=0;
    uint16_t unLen;


    //ClearBitMask(Status2Reg, 0x08);		//TempSensclear
    //ClearBitMask(CollReg,0x80);			//ValuesAfterColl
	writeMFRC522(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]

    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
	{
		//?????? Compruebe el nÃ?Âºmero de serie de la tarjeta
		for (i=0; i<4; i++)
		{   
		 	serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i])
		{   
			status = MI_ERR;    
		}
    }

    //SetBitMask(CollReg, 0x80);		//ValuesAfterColl=1

    return status;
}

unsigned char auth(unsigned char authMode, unsigned char BlockAddr, unsigned char *Sectorkey, unsigned char *serNum)
{
    unsigned char status;
    uint16_t recvBits;
    unsigned char i;
	unsigned char buff[12]; 

	//????+???+????+???? Verifique la direcciÃ?Â³n de comandos de bloques del sector + + contraseÃ?Â±a + nÃ?Âºmero de la tarjeta de serie
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {    
		buff[i+2] = *(Sectorkey+i);   
	}
    for (i=0; i<4; i++)
    {    
		buff[i+8] = *(serNum+i);   
	}
    status = MFRC522ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MI_OK) || (!(readMFRC522(Status2Reg) & 0x08)))
    {   
		status = MI_ERR;   
	}

    return status;
}

  void halt()
{
    unsigned char status;
    uint16_t unLen;
    unsigned char buff[4];

    buff[0] = PICC_HALT;
    buff[1] = 0;
    calculateCRC(buff, 2, &buff[2]);

    clearBitMask(Status2Reg, 0x08); // turn off encryption

    status = MFRC522ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}