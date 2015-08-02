#include <libsocket.h>
#include <stdio.h>
#include "estructuraFS.h"
#include "nodo.h"
#include <pthread.h>
#include <stdlib.h>
#include "estadoFileSystem.h"
#include "logueo.h"
#include <string.h>

#define MENSAJE_ESTADO_ARCHIVO 'E'
#define MENSAJE_BLOQUES_ARCHIVO 'B'
#define MENSAJE_DETALLE_BLOQUE 'D'

#define ARCHIVO_NO_DISPONIBLE 'N'
#define ARCHIVO_DISPONIBLE 'S'

#define MENSAJE_HAY_OTRO_BLOQUE 'O'
#define MENSAJE_NO_HAY_OTRO_BLOQUE 'P'

static void responderEstadoDeArchivoAMarta(SocketConecta *conexion);
static char *obtenerPathArchivo(SocketConecta *conexion);
static void enviarCantidadDeBloquesDelArchivo(SocketConecta *conexion);
static void responderAMartaSolicitudDetalleBloque(SocketConecta *conexion);

void *atenderAMarta(void *argConexion)
{
	SocketConecta *conexion = argConexion;

	while(estaConectado(conexion))
	{
		char tipoMensaje;
		recibirMensaje(conexion,&tipoMensaje,1);

		if(!estaConectado(conexion))
		{
			loguear(LOG_LEVEL_INFO,"Se desconecto Marta");
			return 0;
		}
		if(tipoMensaje == MENSAJE_ESTADO_ARCHIVO)
			responderEstadoDeArchivoAMarta(conexion);

		else if(tipoMensaje == MENSAJE_BLOQUES_ARCHIVO)
			enviarCantidadDeBloquesDelArchivo(conexion);

		else if(tipoMensaje == MENSAJE_DETALLE_BLOQUE)
			responderAMartaSolicitudDetalleBloque(conexion);
	}
	return 0;
}



static void responderEstadoDeArchivoAMarta(SocketConecta *conexion)
{
	char *path = obtenerPathArchivo(conexion);
	char disponible = ARCHIVO_DISPONIBLE;
	char noDisponible = ARCHIVO_NO_DISPONIBLE;
	if(estaDisponibleArchivo(path))
	{
		if(estaConectado(conexion))
			enviarMensaje(conexion,&disponible,1);
	}else
	{
		if(estaConectado(conexion))
			enviarMensaje(conexion,&noDisponible,1);
	}

}


static char *obtenerPathArchivo(SocketConecta *conexion)
{
	int logitudPath;
	if(estaConectado(conexion))
		recibirMensaje(conexion,&logitudPath,sizeof(int));

	char *path = malloc(logitudPath + 1);

	if(estaConectado(conexion))
		recibirMensaje(conexion,path,logitudPath);

	path[logitudPath] = '\0';

	return path;
}


static void enviarCantidadDeBloquesDelArchivo(SocketConecta *conexion)
{
	char *path = obtenerPathArchivo(conexion);
	int cantidadBloquesDelArchivo;
	t_elementoFS *archivo = buscarElemento(path,root);
	if(archivo != NULL && esArchivo(archivo) && estaDisponibleArchivo(path))
		cantidadBloquesDelArchivo = archivo->cantidadBloques;
	else
		cantidadBloquesDelArchivo = 0;

	if(estaConectado(conexion))
		enviarMensaje(conexion,&cantidadBloquesDelArchivo,sizeof(int));

}

static void responderAMartaSolicitudDetalleBloque(SocketConecta *conexion)
{
	char *path = obtenerPathArchivo(conexion);
	t_elementoFS *archivo = buscarElemento(path,root);

	if(archivo != NULL && esArchivo(archivo) && estaDisponibleArchivo(path))
	{
		int contador = 0;
		int numeroDeBloqueSolicitado;
		if(estaConectado(conexion))
			recibirMensaje(conexion,&numeroDeBloqueSolicitado,sizeof(int));

		while(contador < list_size(archivo->listaCopias))
		{
			t_bloque *bloque = list_get(archivo->listaCopias,contador);

			if(bloque->numeroDeBloqueDelArchivo == numeroDeBloqueSolicitado && estaHabilitadoNodo(bloque->nombreNodo,listaNodosConectados))
			{
				char hayBloque  = MENSAJE_HAY_OTRO_BLOQUE;
				if(estaConectado(conexion))
					enviarMensaje(conexion,&hayBloque,1);

				t_nodo *nodo = buscarNodoHabilitado(bloque->nombreNodo);
				if(estaConectado(conexion))
				{
					int longitudNombre = strlen(nodo->nombre);
					enviarMensaje(conexion,&longitudNombre,sizeof(int));
					enviarMensaje(conexion,nodo->nombre,longitudNombre);

					int longitudIp = strlen(nodo->ip);
					enviarMensaje(conexion,&longitudIp,sizeof(int));
					enviarMensaje(conexion,nodo->ip,longitudIp);

					int longitudPuerto = strlen(nodo->puerto);
					enviarMensaje(conexion,&longitudPuerto,sizeof(int));
					enviarMensaje(conexion,nodo->puerto,longitudPuerto);

					enviarMensaje(conexion,&(bloque->numeroDeBloqueEnNodo),sizeof(int));
				}
			}
			contador++;
		}
	}
	char noHayMasBloque = MENSAJE_NO_HAY_OTRO_BLOQUE;
	if(estaConectado(conexion))
		enviarMensaje(conexion,&noHayMasBloque,1);
}

