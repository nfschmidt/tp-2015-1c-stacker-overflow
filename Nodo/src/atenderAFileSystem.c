/*
 * atenderAFileSystem.c
 *
 *  Created on: 2/6/2015
 *      Author: utnso
 */
#include "libsocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ArchivoLocal.h"
#include "atenderConexiones.h"
#include "responderAFileSystem.h"
#include "espacioDatos.h"
#include <pthread.h>
#include "logueo.h"

#define NOMBRE_NODO_INCORRECTO 'E'
#define TAMANIO_BLOQUE	'B'
#define ESCRIBIR_BLOQUE_GRABAR 'G'
#define QUIERE_BLOQUE 'Q'
#define SIGUIENTE_REGISTRO "S"
#define NODO_INEXISTENTE 'X'
#define BLOQUEAR_ATENDER_NODO "T"

#define CANTIDAD_MENSAJES_ACEPTO 16

void static nombreIncorrectoNodo();
static void terminarEjecucionDeTarea();
static off_t recibirYVerificarConexion(SocketConecta *conexion, void *buffer, off_t longitudBuffer);
void static nodoInexistente();
off_t static reservarTamanioBloqueGrabar(SocketConecta *conexion);
void static grabarBloqueDatosYEnEspacio(SocketConecta *conexion,char *bloqueDatosGrabar,EspacioDatos *espacio, off_t tamanioBloque);
void static verQueBloqueEnviarleAFS(SocketConecta *conexion, EspacioDatos *espacio);

void *atenderAFileSystem(void *argEspacioYConexion)
{
	EspacioYConexion *espacioYConexion = argEspacioYConexion;
	SocketConecta *conexion = espacioYConexion->conexion;
	EspacioDatos *espacio = espacioYConexion->espacioDeDatos;
	char numPaquete;
	char *bloqueDatosGrabar;
	off_t tamanioBloque;
	while(estaConectado(conexion))
	{
		recibirYVerificarConexion(conexion,&numPaquete,sizeof(char));
		if (numPaquete == NOMBRE_NODO_INCORRECTO)
			nombreIncorrectoNodo();
		else if(numPaquete == NODO_INEXISTENTE)
			nodoInexistente();
		else if(numPaquete == TAMANIO_BLOQUE){
			tamanioBloque = reservarTamanioBloqueGrabar(conexion);
			bloqueDatosGrabar = malloc(tamanioBloque + 1);
		}
		else if(numPaquete == ESCRIBIR_BLOQUE_GRABAR)
			grabarBloqueDatosYEnEspacio(conexion,bloqueDatosGrabar,espacio,tamanioBloque);
		else if(numPaquete == QUIERE_BLOQUE)
			verQueBloqueEnviarleAFS(conexion,espacio);

	}
	return 0;
}

void static nombreIncorrectoNodo()
{
	loguear(LOG_LEVEL_DEBUG, "Nombre nodo incorrecto , ya existe el nombre este");
	terminarEjecucionDeTarea();
}

void static nodoInexistente()
{
	loguear(LOG_LEVEL_DEBUG, "Nodo inexistente para poder reconectar");
	terminarEjecucionDeTarea();
}

static off_t recibirYVerificarConexion(SocketConecta *conexion, void *buffer, off_t longitudBuffer)
{
	if (!estaConectado(conexion))
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "Se perdio la conexion con el FileSystem");
		terminarEjecucionDeTarea();
	}

	ssize_t bytesRecibidos = recibirMensaje(conexion, buffer, longitudBuffer);

	if (!estaConectado(conexion))
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "Se perdio la conexion con el FileSystem");
		terminarEjecucionDeTarea();
	}

	return bytesRecibidos;
}

static void terminarEjecucionDeTarea()
{
	exit(1);
}

off_t static reservarTamanioBloqueGrabar(SocketConecta *conexion)
{
	off_t tamanioBloque;
	recibirYVerificarConexion(conexion,&tamanioBloque,sizeof(off_t));

	return tamanioBloque;;
}

void static grabarBloqueDatosYEnEspacio(SocketConecta *conexion,char *bloqueDatosGrabar,EspacioDatos *espacio, off_t tamanioBloque)
{
	off_t posicionBloque = 0;

	int numeroBloque;
	recibirYVerificarConexion(conexion, &numeroBloque, sizeof(int));

	loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Se solicito grabar datos en el bloque %d", numeroBloque);

	char *mensajeFS;
	off_t n;
	mensajeFS = malloc(tamanioBloque);
	n = recibirYVerificarConexion(conexion,mensajeFS,tamanioBloque);
	while(n > 0)
	{
		memcpy(bloqueDatosGrabar + posicionBloque,mensajeFS,n );
		posicionBloque +=n;
		if (posicionBloque < tamanioBloque)
			n = recibirYVerificarConexion(conexion,mensajeFS,tamanioBloque);
		else
			n = 0;
	}
	enviarMensaje(conexion,BLOQUEAR_ATENDER_NODO,sizeof(char));
	enviarMensaje(conexion,BLOQUEAR_ATENDER_NODO,sizeof(char));
	bloqueDatosGrabar[tamanioBloque] = '\0';
	setBloque(espacio,bloqueDatosGrabar, numeroBloque);
	free(bloqueDatosGrabar);
	free(mensajeFS);
}

void static verQueBloqueEnviarleAFS(SocketConecta *conexion, EspacioDatos *espacio)
{

	int numeroBloque;
	recibirYVerificarConexion(conexion,&numeroBloque,sizeof(int));

	off_t cantidadBytesBloque = 0;
	recibirYVerificarConexion(conexion,&cantidadBytesBloque,sizeof(off_t));

	enviarBloqueFS(conexion,numeroBloque, espacio,cantidadBytesBloque);
}

