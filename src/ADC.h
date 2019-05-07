/* 
 * File:   MQ2.h
 * Author: GITUD
 *
 * Created on 5 de enero de 2018, 05:19 PM
 */

#ifndef ADC_H
#define	ADC_H

#ifndef _XTAL_FREQ
#define	_XTAL_FREQ 8000
#endif

#ifdef	__cplusplus
extern "C" {
#endif

    float valorPropano(void);
    float valorozono(void);
    float valorCO(void);
    float valor135(void);
    void Config_ADC(void);

#ifdef	__cplusplus
}
#endif

#endif	/* MQ2_H */

