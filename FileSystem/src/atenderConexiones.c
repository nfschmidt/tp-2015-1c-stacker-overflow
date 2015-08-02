#include  "libsocket.h"
#include <pthread.h>
#include <stdio.h>
#include "estadoFileSystem.h"
#include "atenderANodo.h"
#include "atenderAMarta.h"
#include "logueo.h"
#include "recibirArchivoFinalNodo.h"

#define CONEXION_NODO_ARCHIVO 'A'
#define CONEXION_NODO 'N'
#define CONEXION_MARTA 'M'

#define OPERATIVO 'S'
#define NO_OPERATIVO 'N'


static void atenderConexionFileSystem(SocketConecta *conexion);

void *escucharConexionesFileSystem(void *argPuerto)
{
	char *puerto = argPuerto;
	SocketEscucha *escucha = newSocketEscucha("127.0.0.1",puerto);
	escucharConexiones(escucha);

	while(1)
	{
		SocketConecta *conexion = aceptarConexion(escucha);
		atenderConexionFileSystem(conexion);
	}
}

static void atenderConexionFileSystem(SocketConecta *conexion)
{
	char tipoConexion;
	pthread_t hiloProcesoConectado;
	if(estaConectado(conexion))
		recibirMensaje(conexion,&tipoConexion,sizeof(char));

	if (!estaConectado(conexion))
	{
		deleteSocketConecta(conexion);
		return;
	}

	///ES NODO
	if(tipoConexion == CONEXION_NODO)
		pthread_create(&hiloProcesoConectado,NULL,atenderANodo,conexion);

	///ES MARTA
	else if(tipoConexion == CONEXION_MARTA)
	{
		if(esEstadoOperativo() == 1)
		{
			char estoyOperativo = OPERATIVO;
			enviarMensaje(conexion,&estoyOperativo,sizeof(char));
			loguear(LOG_LEVEL_INFO,"Se acepto la conexion de Marta");
			pthread_create(&hiloProcesoConectado,NULL,atenderAMarta,conexion);
		}else
		{
			char noOperativo = NO_OPERATIVO;
			enviarMensaje(conexion,&noOperativo,sizeof(char));
			loguear(LOG_LEVEL_INFO,"Se rechazo la conexion de Marta");
			cerrarSocketConecta(conexion);
		}
	}else if(tipoConexion == CONEXION_NODO_ARCHIVO)
	{
		pthread_create(&hiloProcesoConectado,NULL,atenderANodoParaArchivo,conexion);
	}else
	{
		deleteSocketConecta(conexion);
	}
}
