/*
 * archivoRemoto.h
 *
 *  Created on: 14/7/2015
 *      Author: utnso
 */

#ifndef ARCHIVOREMOTO_H_
#define ARCHIVOREMOTO_H_

#include "nodo.h"

typedef struct {
	char *nombreArchivo;
	t_nodo *nodo;
}t_archivoRemoto;


t_archivoRemoto *newArchivoRemoto(t_nodo *nodo, char* nombreArchivo);

t_nodo *archivoRemotoObtenerNodo(t_archivoRemoto *archivo);

char *archivoRemotoObtenerNombreArchivo(t_archivoRemoto *archivo);


#endif /* ARCHIVOREMOTO_H_ */
