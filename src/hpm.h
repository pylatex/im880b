#ifndef HPM_H_INCLUDED
#define HPM_H_INCLUDED

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdbool.h>
    #include <stdint.h>

    typedef struct {
        uint16_t pm10;
        uint16_t pm25;
    } HPMdata_t;
    
     bool HPMupdated(void);//Copiado de nmeacayenne.h
    
    typedef void (*hpm_enviaSerie_t)(const char *carga,char largo);

    void InicializacionHPM(hpm_enviaSerie_t enviaSerie);

    void hpmChangeAutosend (bool enable);

    uint16_t getLastPM25(void);

    uint16_t getLastPM10(void);

    //
    void hpmSendReadMeasure(void);

    //
    void hpmSendStartMeasure(void);

    //
    void hpmSendStopMeasure(void);

    void hpmSendEnableAutoSend(void);

    void hpmSendDisableAutoSend(void);

    /**
     * Handler del procesamiento de cada octeto recibido por UART. Debe tener en
     * cuenta si el autosend se encuentra habilitado o no, para asi hacer el procesamiento.
     * @param octeto: El byte recibido en el UART.
     */
    void HPMinput(char octeto);

#ifdef	__cplusplus
}
#endif

#endif // HPM_H_INCLUDED
