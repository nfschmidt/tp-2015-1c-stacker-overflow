/*
 * conectarAFileSystem.c
 *
 *  Created on: 7/6/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "espacioDatos.h"
#include "nuevaConexion.h"
#include "libsocket.h"
#include "conectarAFileSystem.h"
#include "configNodo.h"
#include "responderAFileSystem.h"


SocketConecta *conectarAFileSystem(ConfiguracionNodo *nodo,EspacioDatos *espacio)
{
	SocketConecta *conexion = conectarA(nodo->ip_fs,nodo->puerto_fs);

	if (!estaConectado(conexion))
		exit(1);

	enviarFileSystemCantidadBloques(conexion,espacio,nodo);

	return conexion;
}
