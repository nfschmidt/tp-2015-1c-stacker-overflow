/*
 * archivoRemoto.c
 *
 *  Created on: 14/7/2015
 *      Author: utnso
 */
#include "archivoRemoto.h"
#include <stdlib.h>

t_archivoRemoto *newArchivoRemoto(t_nodo *nodo, char *nombreArchivo)
{
	t_archivoRemoto *archivo = malloc(sizeof(t_archivoRemoto));
	archivo->nodo = nodo;
	archivo->nombreArchivo = nombreArchivo;

	return archivo;
}


t_nodo *archivoRemotoObtenerNodo(t_archivoRemoto *archivo)
{
	return archivo->nodo;
}


char *archivoRemotoObtenerNombreArchivo(t_archivoRemoto *archivo)
{
	return archivo->nombreArchivo;
}
