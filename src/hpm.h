#ifndef HPM_H_INCLUDED
#define HPM_H_INCLUDED

typedef int (*hpm_enviaSerie_t)(unsigned char *carga,int largo);

void InicializacionHPM(hpm_enviaSerie_t enviaSerie);

//
void SolicitarMedida(void);

//
void InciarMedicion(void);

//
void PararMedicion(void);

//
void RespuestaSensor(unsigned char *carga,unsigned char peso);

#endif // HPM_H_INCLUDED
