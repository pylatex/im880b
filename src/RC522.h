/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef RC522_H
#define	RC522_H

#include <xc.h> // include processor files - each processor file is guarded.  


#define MAX_LEN 16   // Largo maximo largo de la matrix
//MF522 comando palabra
#define PCD_IDLE              0x00               // NO action; Y cancelar el comando
#define PCD_AUTHENT           0x0E               // autenticación de clave
#define PCD_RECEIVE           0x08               // recepción de datos
#define PCD_TRANSMIT          0x04               // Enviar datos
#define PCD_TRANSCEIVE        0x0C               // Enviar y recibir datos
#define PCD_RESETPHASE        0x0F               // reajustar
#define PCD_CALCCRC           0x03               // CRC calcular

//Mifare_One  Tarjeta Mifare_One comando palabra
#define PICC_REQIDL           0x26               // Área de la antena no está tratando de entrar en el estado de reposo
#define PICC_REQALL           0x52               // Todas las cartas para encontrar el área de la antena
#define PICC_ANTICOLL         0x93               // anti-colisión
#define PICC_SElECTTAG        0x93               // elección de tarjeta
#define PICC_AUTHENT1A        0x60               // verificación key A
#define PICC_AUTHENT1B        0x61               // verificación Key B
#define PICC_READ             0x30               // leer bloque
#define PICC_WRITE            0xA0               // Escribir en el bloque 
#define PICC_DECREMENT        0xC0               // cargo
#define PICC_INCREMENT        0xC1               // recargar
#define PICC_RESTORE          0xC2               // Transferencia de datos de bloque de buffer
#define PICC_TRANSFER         0xB0               // Guardar los datos en el búfer
#define PICC_HALT             0x50               // inactividad

//MF522 Código de error de comunicación cuando regresó
#define MI_OK                 0
#define MI_NOTAGERR           1
#define MI_ERR                2

//------------------ MFRC522 registro---------------
//Page 0:Command and Status
#define     Reserved00            0x00    
#define     CommandReg            0x01    
#define     CommIEnReg            0x02    
#define     DivlEnReg             0x03    
#define     CommIrqReg            0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     Reserved01            0x0F
//Page 1:Command     
#define     Reserved10            0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     Reserved11            0x1A
#define     Reserved12            0x1B
#define     MifareReg             0x1C
#define     Reserved13            0x1D
#define     Reserved14            0x1E
#define     SerialSpeedReg        0x1F
//Page 2:CFG    
#define     Reserved20            0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     Reserved21            0x23
#define     ModWidthReg           0x24
#define     Reserved22            0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsPReg	          0x28
#define     ModGsPReg             0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
//Page 3:TestRegister     
#define     Reserved30            0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     Reserved31            0x3C   
#define     Reserved32            0x3D   
#define     Reserved33            0x3E   
#define     Reserved34			  0x3F
//-----------------------------------------------
#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    bool isCard(void);
	bool readCardSerial(void);

    void init(void);
	void reset(void);
	

    void writeMFRC522(unsigned char addr, unsigned char val);
	    /*
 *  Read_MFRC52
 *  Descripci�n: Desde el MFRC522 leer un byte de un registro de datos
 *  @param addr - la direccion de registro
 *  @return val - Devuelve la respuesta 
 */   
    unsigned char readMFRC522(unsigned char addr);
    
    void antennaOn(void);
	unsigned char readMFRC522(unsigned char addr);
	void setBitMask(unsigned char reg, unsigned char mask);
	void clearBitMask(unsigned char reg, unsigned char mask);
	void calculateCRC(unsigned char *pIndata, unsigned char len, unsigned char *pOutData);
	unsigned char MFRC522Request(unsigned char reqMode, unsigned char *TagType);
	unsigned char MFRC522ToCard(unsigned char command, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, uint16_t *backLen);
	unsigned char anticoll(unsigned char *serNum);
	unsigned char auth(unsigned char authMode, unsigned char BlockAddr, unsigned char *Sectorkey, unsigned char *serNum);
	void halt();

	unsigned char serNum[5];       // Constante para guardar el numero de serie leido.
	unsigned char AserNum[5];      // Constante para guardar el numero d serie de la secion actual.


#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

