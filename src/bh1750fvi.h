/* 
 * File:   bh1750fvi.h
 * Author: alex
 *
 * Created on 3 de abril de 2018, 8:41
 */

#ifndef BH1750FVI_H
#define	BH1750FVI_H

#ifdef	__cplusplus
extern "C" {
#endif

    #define BH1750_ADDR_H   0x5Cu
    #define BH1750_ADDR_L   0x23u

    #define BH1750_PWR_OFF      0
    #define BH1750_PWR_ON       1
    #define BH1750_RESET        7

    #define BH1750_HR           0       //Resolution: 1 lx
    #define BH1750_HR2          1       //Resolution: 0.5 lx
    #define BH1750_LR           3       //Resolution: 4 lx
    #define BH1750_CONTINOUS    0x10
    #define BH1750_SINGLE       0x20

    typedef void (*IOWriter)(bool IOstate);

    void BHinit(bool ADDRpinstatus);
    bool BHread (unsigned short *result);

#ifdef	__cplusplus
}
#endif

#endif	/* BH1750FVI_H */
