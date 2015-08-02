/*
 * atenderConexiones.c
 *
 *  Created on: 20/5/2015
 *      Author: utnso
 */

#include <libsocket.h>
#include <pthread.h>
#include <stdlib.h>
#include "atenderANodo.h"
#include <stdio.h>
#include "commons/log.h"
#include "atenderConexiones.h"
#include "atenderAFileSystem.h"
#include "atenderAJob.h"
#include "logueo.h"

#define CONEXION_NODO 'N'
#define PEDIDO_EJECUCION_TAREA 'J'
#define PEDIDO_EJECUCION_ULTIMA_TAREA 'U'

static void atenderConexionANodo(SocketConecta *conexion, EspacioDatos *espacioDeDatos);

void *atenderConexionesEntrantes(void *argPuertoYEspacio)
{
	PuertoYEspacio *puertoYEspacio = argPuertoYEspacio;

	escucharConexionesANodo(puertoYEspacio->puerto, puertoYEspacio->espacioDeDatos);

	return 0;
}

void escucharConexionesANodo(char *Puerto, EspacioDatos *espacioDeDatos)
{
	SocketEscucha *escucha = newSocketEscucha("127.0.0.1", Puerto);

	escucharConexiones(escucha);

	loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Escuchando conexiones en puerto %s", Puerto);

	while(1)
	{
		SocketConecta *conexion = aceptarConexion(escucha);
		atenderConexionANodo(conexion, espacioDeDatos);
	}
}

void atenderConexionANodo(SocketConecta *conexion, EspacioDatos *espacioDeDatos)
{
	char tipoConexion;
	recibirMensaje(conexion, &tipoConexion,1);

	if (!estaConectado(conexion))
		return;

	EspacioYConexion *espacioYConexion = malloc(sizeof(EspacioYConexion));
	espacioYConexion->espacioDeDatos = espacioDeDatos;
	espacioYConexion->conexion = conexion;

	pthread_t hiloAtencion;

	/*
	 *  Agrego atributo detached a los threads para que se liberen los recursos cuando terminan
	 *  sin que hagamos join
	 */
	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setdetachstate(&attrs,PTHREAD_CREATE_DETACHED);

	int resultadoCreacionThread;
	if(tipoConexion == CONEXION_NODO)
		resultadoCreacionThread = pthread_create(&hiloAtencion, &attrs, atenderANodo, espacioYConexion);
	else if (tipoConexion == PEDIDO_EJECUCION_TAREA)
		resultadoCreacionThread = pthread_create(&hiloAtencion, &attrs, ejecutarTareaDeJob, espacioYConexion);
	else if (tipoConexion == PEDIDO_EJECUCION_ULTIMA_TAREA)
		resultadoCreacionThread = pthread_create(&hiloAtencion, &attrs, ejecutarUltimaTareaDeJob, espacioYConexion);

	if (resultadoCreacionThread != 0)
		loguearYMostrarEnPantalla(LOG_LEVEL_ERROR, "Ocurrio un error al intentar crear un nuevo thread. Codigo de error: %d", resultadoCreacionThread);

	pthread_attr_destroy(&attrs);
}
