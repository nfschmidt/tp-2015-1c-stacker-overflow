/*
 * nodo.h
 *
 *  Created on: 14/7/2015
 *      Author: utnso
 */

#ifndef NODO_H_
#define NODO_H_

typedef struct {

	char *nombre;
	char *ip;
	char *puerto;
	int carga;
	int bloque;

}t_nodo;


t_nodo *newNodo(char *nombre,char *ip, char *puerto);

void nodoAsignarCargaMap(t_nodo *nodo);

void nodoAsignarCargaReduce(t_nodo *nodo);

void nodoLiberarCargaMap(t_nodo *nodo);

void nodoLiberarCargaReduce(t_nodo *nodo);


void nodoSetBloque(t_nodo *nodo, int bloque);

int nodoObtenerBloque(t_nodo *nodo, int bloque);


#endif /* NODO_H_ */
