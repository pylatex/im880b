/*
 * File:   bmp280.h
 * Author: alex
 *
 * Created on 4 de abril de 2018, 17:00
 */

#ifndef BMP280_H
#define	BMP280_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif
    // REGISTERS                      RESET STATE (reading)
    #define BMPtemp_xlsb    0xFC            //0
    #define BMPtemp_lsb     0xFB            //0
    #define BMPtemp_msb     0xFA            //0x80
    #define BMPpress_xlsb   0xF9            //0
    #define BMPpress_lsb    0xF8            //0
    #define BMPpress_msb    0xF7            //0x80
    #define BMPconfig       0xF5            //0
    #define BMPctrl_meas    0xF4            //0
    #define BMPstatus       0xF3            //0
    #define BMPreset        0xE0            //0
    #define BMPid           0xD0            //0x58
    #define BMPcalib(N)     (0x88 + (N))    //individual

    #define BMPresetKey     0xB6            //Write in BMPreset to reset device

    typedef union {
        char    stat;
        struct {
            unsigned im_update  :1; //1: Data is being copied to image registers
            unsigned            :3;
            unsigned measuring  :1; //1: Conversion is running
            unsigned            :3;
        };
    } BMPstatus_t;  //Use with BMPstatus register - See 4.3.3

    #define BMPostSkip  (0<<5)
    #define BMPostX1    (1<<5)
    #define BMPostX2    (2<<5)
    #define BMPostX4    (3<<5)
    #define BMPostX8    (4<<5)
    #define BMPostX16   (5<<5)

    #define BMPospSkip  (0<<2)
    #define BMPospX1    (1<<2)
    #define BMPospX2    (2<<2)
    #define BMPospX4    (3<<2)
    #define BMPospX8    (4<<2)
    #define BMPospX16   (5<<2)

    #define BMPsleepMode    0
    #define BMPforcedMode   1
    #define BMPnormalMode   3

    typedef union {
        char    ctrl_meas;
        struct {
            unsigned mode       :2; // See 3.6
            unsigned osrs_p     :3; // See 3.3.1
            unsigned osrs_t     :3; // See 3.3.2
        };
    } BMPctrl_meas_t;   //Use with BMPctrl_meas - See 4.3.4

    typedef union {
        char    config;
        struct {
            unsigned spi3w_en   :1; // See 5.3
            unsigned            :1;
            unsigned filter     :3; // See 3.3.3
            unsigned t_sb       :3; // See 3.6.3
        };
    } BMPconfig_t;  //Use with BMPconfig - See 4.3.5

    #define BMP280readTrimming(arr)     BMP280read(BMPcalib(0),24,arr)
    #define BMP280readValues(arr)       BMP280read(BMPpress_msb,6,arr)
    #define BMP280writeCtlMeas(mode)    BMP280write(BMPctrl_meas,(mode))

    void BMP280init(bool SDOstate);

    bool BMP280addr(char address);
    bool BMP280write(char address,char value);
    bool BMP280read(char address,char length,char *buffer);

#ifdef	__cplusplus
}
#endif

#endif	/* BMP280_H */

