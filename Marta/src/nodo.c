/*
 * nodo.c
 *
 *  Created on: 14/7/2015
 *      Author: utnso
 */
#include "nodo.h"
#include <stdlib.h>
#include <string.h>
#include "cargaDeRutinas.h"

t_nodo *newNodo(char *nombre,char *ip, char *puerto)
{
	t_nodo *nodo = malloc(sizeof(t_nodo));
	nodo->nombre = strdup(nombre);
	nodo->ip = strdup(ip);
	nodo->puerto = strdup(puerto);
	nodo->carga = 0;

	return nodo;
}

void nodoAsignarCargaMap(t_nodo *nodo)
{
	nodo->carga += getCargaMap();
}

void nodoAsignarCargaReduce(t_nodo *nodo)
{
	nodo->carga += getCargaReduce();
}

void nodoLiberarCargaMap(t_nodo *nodo)
{
	nodo->carga -= getCargaMap();
}

void nodoLiberarCargaReduce(t_nodo *nodo)
{
	nodo->carga -= getCargaReduce();
}

void nodoSetBloque(t_nodo *nodo, int bloque)
{
	nodo->bloque = bloque;
}

int nodoObtenerBloque(t_nodo *nodo, int bloque)
{
	return nodo->bloque;
}
