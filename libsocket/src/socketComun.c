#include "socketComun.h"
#include "socketError.h"

#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <stdlib.h>

static struct addrinfo *obtenerInformacionDeServer(const char *ipServidor, const char *puerto);
static int habilitarReusoDePuerto(int socketFd);



int crearSocket()
{
	int socketFd = socket(AF_INET, SOCK_STREAM, 0);

	if (socketFd == ERROR_SOCKET)
		manejarWarningSocket("Error al ejecutar socket()", errno);

	return socketFd;
}


int conectar(int socketFd, const char *ipServidor, const char *puerto)
{
	struct addrinfo *infoDeServer = obtenerInformacionDeServer(ipServidor, puerto);

	int resultadoConnect = connect(socketFd, infoDeServer->ai_addr, infoDeServer->ai_addrlen);

	if(resultadoConnect == ERROR_CONNECT)
		manejarWarningSocket("Error al ejecutar connect()", errno);

	return resultadoConnect;
}


int bindear(int socketFd, const char *ipServidor, const char *puerto)
{
	struct addrinfo *infoDeServer = obtenerInformacionDeServer(ipServidor, puerto);

	habilitarReusoDePuerto(socketFd);

	int resultadoBind = bind(socketFd, infoDeServer->ai_addr, infoDeServer->ai_addrlen);

	if (resultadoBind == ERROR_BIND)
		manejarWarningSocket("Error al ejecutar bind()", errno);

	return resultadoBind;
}


int escuchar(int socketFd, int longitudDeColaDeEspera)
{
	int resultadoListen = listen(socketFd, longitudDeColaDeEspera);

	if (resultadoListen == ERROR_LISTEN)
		manejarWarningSocket("Error al ejecutar listen()", errno);


	return resultadoListen;
}


int aceptar(int socketFd)
{
	struct sockaddr addr;
	socklen_t addrlen = sizeof(struct sockaddr);

	int resultadoAccept = accept(socketFd, &addr, &addrlen);

	if (resultadoAccept == ERROR_ACCEPT)
		manejarWarningSocket("Error al ejecutar accept()", errno);

	return resultadoAccept;
}


ssize_t recibir(int socketFd, void *buffer, size_t longitudBuffer)
{
	ssize_t recibido = recv(socketFd, buffer, longitudBuffer, 0);

	if(recibido == ERROR_RECV)
	{
		manejarWarningSocket("Error al ejecutar recv()", errno);
		recibido = 0;
	}

	return recibido;
}


ssize_t enviar(int socketFd, void *mensaje, size_t longitudMensaje)
{
	ssize_t enviado = send(socketFd, mensaje, longitudMensaje, MSG_NOSIGNAL);

	if(enviado == ERROR_SEND)
		manejarWarningSocket("Error al ejecutar send()", errno);

	return enviado;
}


int cerrarSocket(int socketFd)
{
	int resultadoClose = close(socketFd);

	if(resultadoClose == ERROR_CLOSE)
		manejarWarningSocket("Error al ejecutar close()", errno);

	return resultadoClose;
}


static struct addrinfo *obtenerInformacionDeServer(const char *ipServidor, const char *puerto)
{
	struct addrinfo hints = {
			.ai_family = AF_UNSPEC,
			.ai_socktype = SOCK_STREAM,
			.ai_flags = AI_PASSIVE
	};

	struct addrinfo *infoDeServer;

	int resultadoGetaddrinfo = getaddrinfo(ipServidor, puerto, &hints, &infoDeServer);
	if (resultadoGetaddrinfo != GETADDRINFO_OK)
		manejarWarningSocket("Error al ejecutar getaddrinfo()", resultadoGetaddrinfo);

	return infoDeServer;
}


static int habilitarReusoDePuerto(int socketFd)
{
	int reusarSi = 1;
	int resultadoSetsockopt = setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &reusarSi, sizeof(int));

	if(resultadoSetsockopt == ERROR_SETSOCKOPT)
		manejarWarningSocket("Error al ejecutar connect()", errno);

	return resultadoSetsockopt;
}

int selecSocket(int ultimoSocket, fd_set conjunto_descrip)
{
	int numSelect;
	numSelect = select(ultimoSocket + 1, &conjunto_descrip, NULL, NULL, NULL);
	if( numSelect  == -1)
	{
		manejarWarningSocket(" error al ejecutar sentencia select",errno);
		exit(1);
	}
	return numSelect;
}

int obtenerDatos(int numeroSocket, fd_set conjunto_descrip)
{
	return FD_ISSET(numeroSocket, &conjunto_descrip);
}
