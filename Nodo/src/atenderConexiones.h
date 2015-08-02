/*
 * atenderConexiones.h
 *
 *  Created on: 20/5/2015
 *      Author: utnso
 */

#ifndef ATENDERCONEXIONES_H_
#define ATENDERCONEXIONES_H_

#include "espacioDatos.h"
#include <libsocket.h>

typedef struct
{
	char *puerto;
	EspacioDatos *espacioDeDatos;
} PuertoYEspacio;


typedef struct
{
	SocketConecta *conexion;
	EspacioDatos *espacioDeDatos;
} EspacioYConexion;


void *atenderConexionesEntrantes(void *puertoYEspacio);

void escucharConexionesANodo(char *Puerto, EspacioDatos *espacioDeDatos);

#endif /* ATENDERCONEXIONES_H_ */
