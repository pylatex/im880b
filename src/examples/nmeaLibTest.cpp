#include <cstring>
#include <cstdio>
#include "nmea.h"

void nmeatest(unsigned char item,char *frase);

int main () {
    NMEAstatus_t statreg;
    NMEAinit(&statreg);

    const char fr1[]="$GPGLL,4916.45,N,12311.12,W,225444,A,*1D";
    NMEAload(fr1);
    while (!statreg.reg);
    if (statreg.complete) {
        printf("%s\n\r",NMEAselect(1));
    }

    const char fr2[]="$GPGSV,3,2,10,02,30,309,,09,20,088,,30,10,157,,13,07,206,*79";
    NMEAload(fr2);
    while (!statreg.reg);
    if (statreg.complete) {
        printf("%s\n\r",NMEAselect(1));
    }

    //TODO: No debe tomar la exclamacion
    const char frase[]="0,10,157,,13,07,206,*79bc$!puto,el que lo,,compiled*4c";
    NMEAload(frase);
    while (!statreg.reg);
    if (statreg.complete) {
        printf("%s\n\r",NMEAselect(0));
    }
    //nmeatest(0,frase);

    return 0;
}

void nmeatest(unsigned char item,char *frase){
    char *resultado;
    NMEAload(frase);
    //resultado=NMEAselect(item,&largo);
    resultado=NMEAselect(item);
    //printf("Elemento %d: \"%.*s\"[%d]\n",item, largo, resultado, largo);
    printf("Elemento %d: \"%s\"\n",item, resultado);
}
