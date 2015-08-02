/*
 * espacioDatos.h
 *
 *  Created on: 7/5/2015
 *      Author: utnso
 */

#ifndef ESPACIODATOS_H_
#define ESPACIODATOS_H_
#include <sys/stat.h>

#define tamanioBloqueFijo 20971520

typedef struct{
	char *archivoMemoria;
	off_t Size;
	int Fd;
}EspacioDatos;

EspacioDatos *newEspacioDatos(char *ruta);
off_t calcularEspacioArchivo(char *ruta);
void obtenerArchivoMemoria(char *ruta, EspacioDatos *esp);
void cerrarEspacioDatos(EspacioDatos *esp);
int cantidadBloquesEspacio(EspacioDatos *esp);
void setBloque(EspacioDatos *esp,char *bloque, int numeroBloque);

#endif /* ESPACIODATOS_H_ */
