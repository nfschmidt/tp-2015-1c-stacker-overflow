/*
 * atenderANodo.c
 *
 *  Created on: 20/5/2015
 *      Author: utnso
 */

#include <libsocket.h>
#include <stdio.h>
#include "responderANodo.h"
#include "ArchivoLocal.h"
#include "atenderConexiones.h"
#include "logueo.h"

#define TENES_ARCHIVO 'A'
#define SIGUIENTE_REGISTRO 'R'
#define TERMINE_DE_PEDIR 'T'

void *atenderANodo(void *argEspacioYConexion)
{
	EspacioYConexion *espacioYCoexion = argEspacioYConexion;
	SocketConecta *conexion = espacioYCoexion->conexion;
	ArchivoLocal *archivoALeer;

	loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Se conecto un Nodo. IP: %s", obtenerIPRemoto(conexion));

	while(estaConectado(conexion))
	{
		char numPaquete;
		recibirMensaje(conexion, &numPaquete,1);

		if (! estaConectado(conexion))
		{
			deleteSocketConecta(conexion);
			return 0;
		}

		if (numPaquete == TENES_ARCHIVO)
			archivoALeer = tengoArchivo(conexion);
		else if(numPaquete == SIGUIENTE_REGISTRO)
			obtenerSiguienteConjuntosRegistrosRemotos(conexion,archivoALeer);
		else if(numPaquete == TERMINE_DE_PEDIR)
			terminoDePedir(conexion,archivoALeer);
	}

	return 0;
}
