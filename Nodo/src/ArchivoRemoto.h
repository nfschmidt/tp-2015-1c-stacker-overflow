#ifndef ARCHIVOREMOTO_H_
#define ARCHIVOREMOTO_H_

#include <libsocket.h>
#include "Archivo.h"

typedef struct {
	Archivo vtable;
	SocketConecta *conexion;
	char *nombreArchivo;
	int existeArchivoRemoto;
	char *conjuntoRegistros;
	uint64_t posicionConjuntoRegistros;
} ArchivoRemoto;


ArchivoRemoto *newArchivoRemoto(char *nombreArchivo, SocketConecta *conexion);


#endif /* ARCHIVOREMOTO_H_ */
