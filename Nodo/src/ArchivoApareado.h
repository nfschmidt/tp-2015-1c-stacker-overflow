#ifndef ARCHIVOAPAREADO_H_
#define ARCHIVOAPAREADO_H_

#include "Archivo.h"

#include <commons/collections/list.h>


typedef struct
{
	Archivo vtable;
	t_list *listaArchivos;
	t_list *listaRegistrosLeidos;
	int indiceArchivoQueEntregaRegistro;
} ArchivoApareado;

ArchivoApareado *newArchivoApareado(t_list *listaArchivos);


#endif /* ARCHIVOAPAREADO_H_ */
