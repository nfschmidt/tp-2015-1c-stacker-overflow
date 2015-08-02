/*
 * consultarNodo.c
 *
 *  Created on: 20/5/2015
 *      Author: utnso
 */

#include <stdlib.h>
#include "libsocket.h"
#include <string.h>
#include <stdio.h>
#include "consultarNodo.h"
#include "commons/log.h"

#define PATH_LOG "/home/utnso/Escritorio/log.txt"

#define CONEXION_NODO "N"
#define TENES_ARCHIVO "A"
#define SIGUIENTE_REGISTRO "R"
#define TERMINE_PEDIR "T"


static char *armarPaquete(char *tipoPaquete,char *nombreArchivo);

char tenesArchivo(SocketConecta *conexion, char *nombreArchivo)
{
	char tengoArchivo = 0;

	size_t tamanioNombreArchivo = strlen(nombreArchivo);

	char *mensajeNodo = armarPaquete(TENES_ARCHIVO,nombreArchivo);
	if (estaConectado(conexion))
		enviarMensaje(conexion,mensajeNodo,6 + tamanioNombreArchivo);
	else
	{
		calloServerNodoConexion("el nodo servidor");
		return CAYO_SERVER;
	}

	recibirMensaje(conexion, &tengoArchivo,1);
	if (! estaConectado(conexion))
	{
		calloServerNodoConexion("el nodo servidor");
		return CAYO_SERVER;
	}

	return tengoArchivo;
}

char *getRegistroRemoto(SocketConecta *conexion)
{
	char *registro;
	uint64_t tamanioRegistro;

	if (! estaConectado(conexion))
	{
		calloServerNodoConexion("el nodo servidor");
		return strdup(CAYO_SERVER_C);
	}

	enviarMensaje(conexion,SIGUIENTE_REGISTRO,1);

	tamanioRegistro = recibirTodo(conexion, (void**) &registro);
	if (! estaConectado(conexion))
	{
		calloServerNodoConexion("el nodo servidor");
		return strdup(CAYO_SERVER_C);
	}

	if (tamanioRegistro == 0)
		return NULL;

	registro[tamanioRegistro] = '\0';

	return registro;
}

char *armarPaquete(char *tipoPaquete,char *nombreArchivo)
{
	size_t tamanioNombreArchivo = strlen(nombreArchivo);

	char *mensajeNodo = malloc(6 + strlen(nombreArchivo) * sizeof(char));

	memcpy(mensajeNodo,CONEXION_NODO,1);

	memcpy(mensajeNodo + 1,tipoPaquete,1);

	memcpy(mensajeNodo + 2, &tamanioNombreArchivo,4);

	memcpy(mensajeNodo + 6,nombreArchivo,tamanioNombreArchivo);

	return mensajeNodo;
}

char termineDePedir(SocketConecta *conexion)
{
	char termine = 0;

	if (estaConectado(conexion))
		enviarMensaje(conexion,TERMINE_PEDIR,1);

	return termine;
}


void calloServerNodoConexion(char *mensaje)
{
	t_log *Log;
	Log = log_create(PATH_LOG,"NODO",0,LOG_LEVEL_ERROR);
	log_error(Log,"Se cayo %s",mensaje);
	log_destroy(Log);
}
