/* 
 * File:   hdc1010.h
 * Author: Alex Bustos - alexfbp at gmail dot com
 *
 * Created on 10 de febrero de 2018, 15:49
 */

#ifndef HDC1010_H
#define	HDC1010_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdbool.h>

    //Register Pointers
    #define HDC_TEMPERATURE 0x00
    #define HDC_HUMIDITY    0x01
    #define HDC_CONFIG      0x02
    #define HDC_SID_H       0xFB
    #define HDC_SID_M       0xFC
    #define HDC_SID_L       0xFD
    #define HDC_MID         0xFE
    #define HDC_DID         0xFF

    //Configuration bitfield values
    #define RST_RST         1
    #define HEAT_DISABLE    0
    #define HEAT_ENABLE     1
    #define HDC_MODE_SINGLE 0
    #define HDC_MODE_BOTH   1
    #define TRES_14         0   //Wait ~6.35 ms before reading
    #define TRES_11         1   //Wait ~3.65 ms before reading
    #define HRES_14         0   //Wait ~6.50 ms before reading
    #define HRES_11         1   //Wait ~3.85 ms before reading
    #define HRES_8          2   //Wait ~2.50 ms before reading

    #define HDC_I2C_BASE_ADDR   0x40

    /**
     * Initializes the library working mode
     * @param tempAdd   Address in which will be saved the temperature readings
     * @param humAdd    Address in which will be saved the humidity readings
     */
    void HDCinit (unsigned short *tempAdd,unsigned short *humAdd);

    /**
     * Updates the last temperature result in the pointed variable
     * @return true if success
     */
    bool HDCtemp ();

    /**
     * Updates the last humidity result in the pointed variable
     * @return true if success
     */
    bool HDChumidity ();

    /**
     * Updates the last humidity and temperature results in the pointed variables
     * @return true if success
     */
    bool HDCboth ();
    
    unsigned HDCgetTempResolution (void);
    void HDCsetTempResolution (unsigned const TempResolution);
    unsigned HDCgetHumidityResolution (void);
    void HDCsetHumidityResolution (unsigned const HumidityResolution);

#ifdef	__cplusplus
}
#endif

#endif	/* HDC1010_H */

