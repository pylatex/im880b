/* 
 * File:   bh1750fvi.h
 * Author: alex
 *
 * Created on 3 de abril de 2018, 8:41
 */

#ifndef BH1750FVI_H
#define	BH1750FVI_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    #define BH1750_ADDR_H   0x5Cu
    #define BH1750_ADDR_L   0x23u

    #define BH1750_PWR_OFF      0
    #define BH1750_PWR_ON       1
    #define BH1750_RESET        7

    //Resolutions                           Resolution  Divider (for real lux value)
    #define BH1750_HR           0       //  1 lx.       1.2
    #define BH1750_HR2          1       //  0.5 lx      2.4
    #define BH1750_LR           3       //  4 lx        1.2
    //Modes
    #define BH1750_CONTINOUS    0x10
    #define BH1750_SINGLE       0x20

    /**
     * Loads the address to be used with the sensor.
     * @param ADDRstat: the logic state of ADDR pin
     */
    void BHinit(bool ADDRstat);

    /**
     * Writes a command to the sensor.
     * @param command:
     * A bitwise OR (|) between a resolution and a mode. Example:
     * High Resolution, Mode 2: BHwrite(BH1750_CONTINOUS | BH1750_HR)
     * @return true on success write
     */
    bool BHwrite (char command);

    /**
     * Reads the last value, always depending on the BH1750 power state and last
     * configuration setup sent.
     * @param resultAddr: Divided by the quantity associated to the configured
     * resolution, gives the illuminance reading in lux.
     * @return true on success read
     */
    bool BHread (unsigned short *resultAddr);

#ifdef	__cplusplus
}
#endif

#endif	/* BH1750FVI_H */
