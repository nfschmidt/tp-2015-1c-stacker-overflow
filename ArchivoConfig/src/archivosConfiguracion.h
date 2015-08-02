#ifndef ARCHIVOSCONFIGURACION_H_
#define ARCHIVOSCONFIGURACION_H_

#include <commons/config.h>


typedef enum {ENTERO,STRING,DOUBLE,LISTA,LONG} tipoDeDato;


void *obtenerParametroConfiguracion(t_config *conf, char *parametro, tipoDeDato tipo);


#endif /* ARCHIVOSCONFIGURACION_H_ */
