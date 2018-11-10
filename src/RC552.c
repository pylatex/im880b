
#include "spi.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "RC522.h"


unsigned char     readData;
unsigned char     readDummy;



bool isCard(void) 
 {
	unsigned char status;
	unsigned char str[MAX_LEN];

	status = MFRC522Request(PICC_REQIDL, str);	
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
    
	reset();
	//Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    writeMFRC522(TModeReg, 0x8D);		//Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    writeMFRC522(TPrescalerReg, 0x3E);	//TModeReg[3..0] + TPrescalerReg
    writeMFRC522(TReloadRegL, 30);           
    writeMFRC522(TReloadRegH, 0);
	writeMFRC522(TxAutoReg, 0x40);		//100%ASK
	writeMFRC522(ModeReg, 0x3D);		// CRC valor inicial de 0x6363

	//ClearBitMask(Status2Reg, 0x08);	//MFCrypto1On=0
	//writeMFRC522(RxSelReg, 0x86);		//RxWait = RxSelReg[5..0]
	//writeMFRC522(RFCfgReg, 0x7F);   	//RxGain = 48dB
	antennaOn();		//Abre  la antena
}

 void reset(void)
{
	writeMFRC522(CommandReg, PCD_RESETPHASE);
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
	if (!(temp & 0x03))
	{
		setBitMask(TxControlReg, 0x03);
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

  void calculateCRC( unsigned char *pIndata, unsigned char len, unsigned char *pOutData)
{
    unsigned char i, n;

    clearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
    setBitMask(FIFOLevelReg, 0x80);			//Claro puntero FIFO
    //Write_MFRC522(CommandReg, PCD_IDLE);

	//Escribir datos en el FIFO	
    for (i=0; i<len; i++)
    {   
		writeMFRC522(FIFODataReg, *(pIndata+i));   
	}
    writeMFRC522(CommandReg, PCD_CALCCRC);

	// Esperar a la finalizaciÃ?Â³n de cÃ?Â¡lculo del CRC
    i = 0xFF;
    do 
    {
        n = readMFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	//Lea el cÃ?Â¡lculo de CRC
    pOutData[0] = readMFRC522(CRCResultRegL);
    pOutData[1] = readMFRC522(CRCResultRegM);
}


unsigned char MFRC522ToCard(unsigned char command, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, uint16_t *backLen)
{
    unsigned char status = MI_ERR;
    unsigned char irqEn = 0x00;
    unsigned char waitIRq = 0x00;
	unsigned char lastBits;
    unsigned char n;
    uint16_t i;

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
			waitIRq = 0x30;
			break;
		}
		default:
			break;
    }

    writeMFRC522(CommIEnReg, irqEn|0x80);	//De solicitud de interrupcion
    clearBitMask(CommIrqReg, 0x80);			// Borrar todos los bits 
    setBitMask(FIFOLevelReg, 0x80);			//FlushBuffer=1, FIFO de inicializaciÃ?Â³n

	writeMFRC522(CommandReg, PCD_IDLE);	//NO action;Y cancelar el comando

	//Escribir datos en el FIFO
    for (i=0; i<sendLen; i++)
    {   
		writeMFRC522(FIFODataReg, sendData[i]);    
	}

	//???? ejecutar el comando
	writeMFRC522(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {    
		setBitMask(BitFramingReg, 0x80);		//StartSend=1,transmission of data starts  
	}   

	// A la espera de recibir datos para completar
	i = 2000;	//i????????,??M1???????25ms	??? i De acuerdo con el ajuste de frecuencia de reloj, el tiempo mÃ?Â¡ximo de espera operaciÃ?Â³n M1 25ms tarjeta??
    do 
    {
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = readMFRC522(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    clearBitMask(BitFramingReg, 0x80);			//StartSend=0

    if (i != 0)
    {    
        if(!(readMFRC522(ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   
				status = MI_NOTAGERR;			//??   
			}

            if (command == PCD_TRANSCEIVE)
            {
               	n = readMFRC522(FIFOLevelReg);
              	lastBits = readMFRC522(ControlReg) & 0x07;
                if (lastBits)
                {   
					*backLen = (n-1)*8 + lastBits;   
				}
                else
                {   
					*backLen = n*8;   
				}

                if (n == 0)
                {   
					n = 1;    
				}
                if (n > MAX_LEN)
                {   
					n = MAX_LEN;   
				}

				//??FIFO??????? Lea los datos recibidos en el FIFO
                for (i=0; i<n; i++)
                {   
					backData[i] = readMFRC522(FIFODataReg);    
				}
            }
        }
        else
        {   
			status = MI_ERR;  
		}

    }

    //SetBitMask(ControlReg,0x80);           //timer stops
    //Write_MFRC522(CommandReg, PCD_IDLE); 

    return status;
}
unsigned char  MFRC522Request(unsigned char reqMode, unsigned char *TagType)
{
	unsigned char status;  
	uint16_t backBits;			//   RecibiÃ?Â³ bits de datos

	writeMFRC522(BitFramingReg, 0x07);		//TxLastBists = BitFramingReg[2..0]	???

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