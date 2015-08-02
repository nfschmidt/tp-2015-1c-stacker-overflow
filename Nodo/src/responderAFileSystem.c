/*
 * responderAFileSystem.c
 *
 *  Created on: 2/6/2015
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ArchivoLocal.h"
#include "libsocket.h"
#include "espacioDatos.h"
#include "configNodo.h"
#include "bloqueDatos.h"
#include "logueo.h"

#define TAMANIO_ENVIO 512

#define CONEXION_NODO "N"
#define CANTIDAD_BLOQUES "C"
#define BLOQUE_PEDIDO "P"
#define BLOQUEAR_ATENDER_NODO "T"

void enviarFileSystemCantidadBloques(SocketConecta *conexion, EspacioDatos *espacio, ConfiguracionNodo *nodo)
{
	char *mensajeNodo;

	int cantidadBloque = cantidadBloquesEspacio(espacio);

	size_t tamanioNombre = strlen(nodo->nombre);
	size_t tamanioPuerto = strlen(nodo->puerto);

	mensajeNodo = malloc(sizeof(char) + sizeof(char) + sizeof(int) + sizeof(int) + sizeof(size_t) + tamanioNombre + sizeof(size_t) + tamanioPuerto);

	memcpy(mensajeNodo, CONEXION_NODO,sizeof(char));
	memcpy(mensajeNodo + sizeof(char) , CANTIDAD_BLOQUES,sizeof(char));
	memcpy(mensajeNodo + 2,&cantidadBloque,sizeof(int));
	memcpy(mensajeNodo + 2 + sizeof(int),&(nodo->nuevo),sizeof(int));
	memcpy(mensajeNodo + 2 + sizeof(int) + sizeof(int),&tamanioNombre,sizeof(size_t));
	memcpy(mensajeNodo + 10 + sizeof(size_t),nodo->nombre,tamanioNombre);
	memcpy(mensajeNodo + 10 + sizeof(size_t) + tamanioNombre, &tamanioPuerto,sizeof(size_t));
	memcpy(mensajeNodo + 10 + sizeof(size_t) + tamanioNombre + sizeof(size_t), nodo->puerto,tamanioPuerto);

	if (estaConectado(conexion))
		enviarMensaje(conexion,mensajeNodo, 2 + sizeof(int) + sizeof(int) + sizeof(size_t) + tamanioNombre + sizeof(size_t) + tamanioPuerto);

}

void enviarBloqueFS(SocketConecta *conexion,int numeroBloque, EspacioDatos *espacio,off_t cantidadBytesBloque)
{
	loguear(LOG_LEVEL_INFO, "Se solicito leer el bloque %d", numeroBloque);
	BloqueDatos *bloque = getBloque(espacio,numeroBloque);
	char *texto = bloque->textoBloque;
	long bytesEnviados = 0;
	long bytesPorEnviar;
	long retorno;

	bytesPorEnviar = cantidadBytesBloque;

	if (estaConectado(conexion))
		enviarMensaje(conexion,BLOQUEAR_ATENDER_NODO,sizeof(char));

	if (estaConectado(conexion))
		enviarMensaje(conexion,&bytesPorEnviar,sizeof(long));

	while(bytesEnviados < bytesPorEnviar && estaConectado(conexion))
	{
		retorno = enviarMensaje(conexion,(char*)(texto + bytesEnviados),(bytesPorEnviar - bytesEnviados));
		bytesEnviados += retorno;

	}
	loguear(LOG_LEVEL_INFO, "Se envio el bloque %d", numeroBloque);

}
