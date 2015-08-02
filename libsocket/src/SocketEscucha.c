#include "SocketEscucha.h"
#include "socketComun.h"
#include "socketError.h"

#include <stdlib.h>

#define LONGITUD_COLA_DEFAULT 100

static SocketConecta *newSocketConectaDesdeFileDescriptor(int socketFd);


SocketEscucha *newSocketEscucha(const char *ip, const char *puerto)
{
	SocketEscucha *sock = malloc(sizeof(SocketEscucha));

	sock->socketFd = crearSocket();

	int resultadoBind = bindear(sock->socketFd, NULL, puerto);

	if (resultadoBind == ERROR_BIND)
		sock->estado = SOCKETE_ERROR;
	else
		sock->estado = SOCKETE_BINDEADO;

	return sock;
}

void escucharConexiones(SocketEscucha *sock)
{
	if (sock->estado != SOCKETE_BINDEADO)
	{
		manejarErrorSocket("Error: no se puede ejecutar escucharConexiones sobre un socket"
				" que no esta en estado SOCKETE_BINDEADO");
		return;
	}

	int resultadoEscuchar = escuchar(sock->socketFd, LONGITUD_COLA_DEFAULT);

	if (resultadoEscuchar == ERROR_LISTEN)
		sock->estado = SOCKETE_ERROR;
	else
		sock->estado = SOCKETE_ESCUCHANDO;
}


SocketConecta *aceptarConexion(SocketEscucha *sock)
{
	if (sock->estado != SOCKETE_ESCUCHANDO)
	{
		manejarErrorSocket("Error: no se puede ejecutar aceptarConexion sobre un socket"
				" que no esta en estado SOCKETE_ESCUCHANDO");
		return 0;
	}

	int socketFd = aceptar(sock->socketFd);

	return newSocketConectaDesdeFileDescriptor(socketFd);
}


SocketConecta *newSocketConectaDesdeFileDescriptor(int socketFd)
{
	SocketConecta *sock = malloc(sizeof(SocketConecta));
	sock->socketFd = socketFd;
	sock->estado = SOCKETC_CONECTADO;

	return sock;
}


void cerrarSocketEscucha(SocketEscucha *sock)
{
	cerrarSocket(sock->socketFd);
	sock->estado = SOCKETE_CERRADO;
}


void deleteSocketEscucha(SocketEscucha *sock)
{
	if (sock->estado != SOCKETE_CERRADO)
		cerrarSocketEscucha(sock);

	free(sock);
}
