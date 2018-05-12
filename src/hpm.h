#ifndef HPM_H_INCLUDED
#define HPM_H_INCLUDED

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdbool.h>

    typedef int (*hpm_enviaSerie_t)(const char *carga,char largo);

    void InicializacionHPM(hpm_enviaSerie_t enviaSerie);

    //
    void SolicitarMedida(void);

    //
    void InciarMedicion(void);

    //
    void PararMedicion(void);

    //
    void RespuestaSensor(char *carga,char peso);
    
    typedef void (*hpm_timStarter)(int msdelay, bool *runningFlag);

#ifdef	__cplusplus
}
#endif

#endif // HPM_H_INCLUDED
