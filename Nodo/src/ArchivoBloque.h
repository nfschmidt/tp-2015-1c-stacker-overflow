#ifndef ARCHIVOBLOQUE_H_
#define ARCHIVOBLOQUE_H_

#include "bloqueDatos.h"
#include "ArchivoLocal.h"

typedef struct
{
	Archivo vtable;
	BloqueDatos *bloque;
	int primerRegistroEstaLeido;
} ArchivoBloque;


ArchivoBloque *newArchivoBloque(BloqueDatos *bloque);

#endif
