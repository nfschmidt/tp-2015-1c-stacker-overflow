/*
 * enviarBloqueANodos.h
 *
 *  Created on: 21/6/2015
 *      Author: utnso
 */

#ifndef ENVIARBLOQUEANODOS_H_
#define ENVIARBLOQUEANODOS_H_

#include  <stdlib.h>
#include  <string.h>
#include  <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include "nodo.h"
#include <semaphore.h>
#include "asignarBloque.h"
#include "responderANodo.h"
#include "libsocket.h"
#include "estructuraFS.h"

typedef struct {
	char *bloque;
	t_elementoFS *archivoNuevo;
	t_nodo *nodo;
	int contadorBloques;
}enviarDatosNodo;

int enviarBloqueANodos(char *bloque,t_elementoFS *archivoNuevo,int contadorBloques);

#endif /* ENVIARBLOQUEANODOS_H_ */
