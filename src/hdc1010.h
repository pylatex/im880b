/* 
 * File:   hdc1010.h
 * Author: alex
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
    #define TRES_14         0
    #define TRES_11         1
    #define HRES_14         0
    #define HRES_11         1
    #define HRES_8          2

    #define HDC_I2C_BASE_ADDR   0x40

    typedef union {
        unsigned short reg;
        struct {
            unsigned        :8;
            unsigned HRES   :2; //8..9
            unsigned TRES   :1; //10
            unsigned BTST   :1; //11
            unsigned MODE   :1; //12
            unsigned HEAT   :1; //13
            unsigned        :1;
            unsigned RST    :1; //14
        };
    } HDC_CONFIG_REG_t;

    /**
     * Initializes the library working mode
     * @param tempAdd   points to temperature variable by user
     * @param humAdd    points to humidity variable by user
     */
    void HDCinit (unsigned short *tempAdd,unsigned short *humAdd);
    bool HDCtemp ();
    bool HDChumidity ();
    bool HDCboth ();

#ifdef	__cplusplus
}
#endif

#endif	/* HDC1010_H */

