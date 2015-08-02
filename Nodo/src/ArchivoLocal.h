#ifndef ARCHIVOLOCAL_H_
#define ARCHIVOLOCAL_H_

#include <stdio.h>
#include "Archivo.h"


typedef enum
{
	LECTURA,
	ESCRITURA
} ModoApertura;


typedef struct ArchivoLocal
{
	Archivo vtable;
	FILE *fileHandler;
} ArchivoLocal;


ArchivoLocal *newArchivoLocal(const char *path, ModoApertura modo);
size_t obtenerTamanioSiguienteRegistro(ArchivoLocal *archivoLocal);

#endif
