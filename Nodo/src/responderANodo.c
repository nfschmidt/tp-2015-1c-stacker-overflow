/*
 * responderANodo.c
 *
 *  Created on: 20/5/2015
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libsocket.h>
#include "ArchivoLocal.h"
#include "Archivo.h"
#include "configNodo.h"
#include "atenderConexiones.h"
#include <pthread.h>
#include "logueo.h"

#define SI_TENGO_ARCHIVO "S"
#define NO_TENGO_ARCHIVO "N"
#define SI_TERMINO 'S'

#define SIZE_CONJUNTO_REGISTROS (64 * 1024)

static char *obtenerPathCompleto(SocketConecta *conexion);
static void cerrarConexionYArchivo(SocketConecta *conexion, ArchivoLocal *archivoALeer);
static void finalizarAtencionAConexion(SocketConecta *conexion);


ArchivoLocal *tengoArchivo(SocketConecta *conexion)
{
	char *pathCompleto = obtenerPathCompleto(conexion);

	loguear(LOG_LEVEL_INFO, "Se recibio un pedido de archivo de parte de un nodo. Archivo: %s", pathCompleto);

	ArchivoLocal *archivoALeer = newArchivoLocal(pathCompleto, LECTURA);

	if (! estaConectado(conexion))
	{
		if (archivoALeer != NULL)
			cerrarArchivo((Archivo *)archivoALeer);

		finalizarAtencionAConexion(conexion);
	}

	if (archivoALeer == NULL)
		enviarMensaje(conexion,NO_TENGO_ARCHIVO,1);
	else
		enviarMensaje(conexion,SI_TENGO_ARCHIVO,1);

	return archivoALeer;
}

void obtenerSiguienteConjuntosRegistrosRemotos(SocketConecta *conexion, ArchivoLocal *archivoALeer)
{
	if(! estaConectado(conexion))
	{
		cerrarArchivo((Archivo *)archivoALeer);
		finalizarAtencionAConexion(conexion);
	}

	uint64_t tamanioCojuntoRegistros = 0;
	char conjuntoRegistros[SIZE_CONJUNTO_REGISTROS];
	conjuntoRegistros[0] = '\0';

	uint64_t tamanioSiguienteRegistro = obtenerTamanioSiguienteRegistro(archivoALeer);
	while(tamanioSiguienteRegistro > 0 && tamanioCojuntoRegistros + tamanioSiguienteRegistro + 1 < SIZE_CONJUNTO_REGISTROS)
	{
		char *registro = obtenerRegistro((Archivo *) archivoALeer);
		tamanioCojuntoRegistros += strlen(registro);
		strcat(conjuntoRegistros, registro);
		free(registro);

		tamanioSiguienteRegistro = obtenerTamanioSiguienteRegistro(archivoALeer);
	}

	if (tamanioCojuntoRegistros == 0)
		enviarMensaje(conexion, &tamanioCojuntoRegistros, sizeof(uint64_t));
	else
		enviarTodo(conexion, conjuntoRegistros, tamanioCojuntoRegistros);
}

static char *obtenerPathCompleto(SocketConecta *conexion)
{
	char *nombreArchivo;
	size_t tamanioNombreArchivo;
	char *pathCompleto;


	recibirMensaje(conexion,&tamanioNombreArchivo,4 );

	if (! estaConectado(conexion))
		finalizarAtencionAConexion(conexion);

	nombreArchivo = malloc(tamanioNombreArchivo + 1);

	recibirMensaje(conexion,nombreArchivo,tamanioNombreArchivo);

	if (! estaConectado(conexion))
		finalizarAtencionAConexion(conexion);

	nombreArchivo[tamanioNombreArchivo] = '\0';

	pathCompleto = malloc(strlen(nombreArchivo) + strlen(obtenerPathEspacioTemporal()) + 1);

	strcpy(pathCompleto,obtenerPathEspacioTemporal());
	strcat(pathCompleto,nombreArchivo);

	return pathCompleto;
}

void terminoDePedir(SocketConecta *conexion, ArchivoLocal *archivoALeer)
{
	loguear(LOG_LEVEL_INFO, "Se termino la transferencia del archivo");

	if(! estaConectado(conexion))
	{
		cerrarArchivo((Archivo*) archivoALeer);
		finalizarAtencionAConexion(conexion);
	}

	char termino = SI_TERMINO;

	enviarMensaje(conexion,&termino,1);

	cerrarConexionYArchivo(conexion,archivoALeer);
}

static void cerrarConexionYArchivo(SocketConecta *conexion, ArchivoLocal *archivoALeer)
{
	cerrarArchivo((Archivo *)archivoALeer);
	cerrarSocketConecta(conexion);
}

static void finalizarAtencionAConexion(SocketConecta *conexion)
{
	deleteSocketConecta(conexion);
	pthread_cancel(pthread_self());
}
