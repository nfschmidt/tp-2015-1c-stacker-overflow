#include "SocketConecta.h"
#include "socketComun.h"
#include "socketError.h"
#include <commons/collections/list.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

SocketConecta *newSocketConecta(const char *ipServidor, const char *puertoServidor)
{
	SocketConecta *sock = malloc(sizeof(SocketConecta));

	strcpy(sock->ipServidor, ipServidor);
	strcpy(sock->puertoServidor, puertoServidor);

	sock->socketFd = crearSocket();

	if (sock->socketFd == ERROR_SOCKET)
		sock->estado = SOCKETC_ERROR;
	else
		sock->estado = SOCKETC_CREADO;

	return sock;
}


void conectarAServidor(SocketConecta *sock)
{
	if (sock->estado != SOCKETC_CREADO)
		manejarErrorSocket("Error: no se puede usar la funcion conectarAServidor sobre un socket"
				" en un estado distinto a SOCKETC_CREADO");


	int resultadoConectar = conectar(sock->socketFd, sock->ipServidor, sock->puertoServidor);

	if (resultadoConectar == ERROR_CONNECT)
		sock->estado = SOCKETC_ERROR;
	else
		sock->estado = SOCKETC_CONECTADO;
}


ssize_t enviarMensaje(SocketConecta *sock, void *mensaje, size_t longitudMensaje)
{
	if (sock->estado != SOCKETC_CONECTADO)
		manejarErrorSocket("Error: no se puede usar la funcion enviarMensaje sobre un socket"
				" en un estado distinto a SOCKETC_CONECTADO");

	ssize_t resultadoEnviar = enviar(sock->socketFd, mensaje, longitudMensaje);

	if (resultadoEnviar == ERROR_SEND)
		cerrarSocketConecta(sock);

	return resultadoEnviar;
}


ssize_t recibirMensaje(SocketConecta *sock, void *buffer, size_t longitudBuffer)
{
	if (sock->estado != SOCKETC_CONECTADO)
		manejarErrorSocket("Error: no se puede usar la funcion recibirMensaje sobre un socket"
				" en un estado distinto a SOCKETC_CONECTADO");

	ssize_t recibido = recibir(sock->socketFd, buffer, longitudBuffer);

	if (recibido == FIN_DE_CONEXION)
		sock->estado = SOCKETC_CERRADO;

	return recibido;
}


int estaConectado(SocketConecta *sock)
{
	return sock->estado == SOCKETC_CONECTADO;
}


void cerrarSocketConecta(SocketConecta *sock)
{
	cerrarSocket(sock->socketFd);
	sock->estado = SOCKETC_CERRADO;
}


void deleteSocketConecta(SocketConecta *sock)
{
	if(sock->estado != SOCKETC_CERRADO)
		cerrarSocketConecta(sock);

	free(sock);
}

char *obtenerIPRemoto(SocketConecta *sock)
{
	if (sock->estado != SOCKETC_CONECTADO)
		manejarErrorSocket("Error: no se puede usar la funcion obtenerIP sobre un socket"
				" en un estado distinto a SOCKETC_CONECTADO");

	struct sockaddr_in informacionConexion;
	socklen_t sizeSockAddr = sizeof informacionConexion;

	int resultado = getpeername(sock->socketFd, (struct sockaddr *) &informacionConexion, &sizeSockAddr);
	if (resultado != 0)
		return NULL;

	char *ip = strdup(inet_ntoa(informacionConexion.sin_addr));

	return ip;
}

uint64_t enviarTodo(SocketConecta *sock, void *mensaje, uint64_t longitudMensaje)
{
	if (!estaConectado(sock))
		return 0;

	enviarMensaje(sock, &longitudMensaje, sizeof(uint64_t));

	uint64_t bytesEnviados = 0;
	while (bytesEnviados < longitudMensaje)
	{
		char *mensajeRestante = ((char *) mensaje) + bytesEnviados;
		bytesEnviados += enviarMensaje(sock, mensajeRestante, longitudMensaje - bytesEnviados);
	}

	return bytesEnviados;
}

uint64_t recibirTodo(SocketConecta *sock, void **buffer)
{
	uint64_t bytesARecibir;

	recibirMensaje(sock, &bytesARecibir, sizeof(uint64_t));
	if (!estaConectado(sock))
		return 0;

	*buffer = malloc(bytesARecibir + 1);

	uint64_t bytesRecibidos = 0;
	while (bytesRecibidos < bytesARecibir)
	{
		if (!estaConectado(sock))
		{
			free(*buffer);
			return 0;
		}

		bytesRecibidos += recibirMensaje(sock, *buffer + bytesRecibidos, bytesARecibir - bytesRecibidos);
	}

	return bytesRecibidos;
}

ssize_t recibirMensajeConTimeout(SocketConecta *sock, void *buffer, size_t longitudBuffer, int segundos)
{
	if (!estaConectado(sock))
		return 0;

	struct timeval tv;
	tv.tv_sec = segundos;
	tv.tv_usec = 1;
	setsockopt(sock->socketFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	ssize_t recibido = recv(sock->socketFd, buffer, longitudBuffer, 0);
	if (recibido == FIN_DE_CONEXION)
			sock->estado = SOCKETC_CERRADO;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	setsockopt(sock->socketFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	return recibido;
}
