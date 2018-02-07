#ifndef HPM_H_INCLUDED
#define HPM_H_INCLUDED

#ifdef	__cplusplus
extern "C" {
#endif

    typedef int (*hpm_enviaSerie_t)(unsigned char *carga,char largo);

    void InicializacionHPM(hpm_enviaSerie_t enviaSerie);

    //
    void SolicitarMedida(void);

    //
    void InciarMedicion(void);

    //
    void PararMedicion(void);

    //
    void RespuestaSensor(unsigned char *carga,unsigned char peso);

#ifdef	__cplusplus
}
#endif

#endif // HPM_H_INCLUDED
