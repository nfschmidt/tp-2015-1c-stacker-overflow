/*
 * nuevaConexion.c
 *
 *  Created on: 20/5/2015
 *      Author: utnso
 */

#include <libsocket.h>

SocketConecta *conectarA(char *ipDestino,char *puertoDestino)
{
	SocketConecta *socketServer = newSocketConecta(ipDestino,puertoDestino);

	conectarAServidor(socketServer);

	return socketServer;
}
