#include "Selector.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>

#define NO_EXISTE_SOCKET_EN_LISTA 0

static void agregarSocketsAListaDeEspera(Selector *selector);
static SocketConecta *obtenerSocketConectaConFD(Selector *selector, int fd);
static bool conexionEstaCaida(void *arg);

Selector *newSelector(SocketEscucha *listener)
{
	Selector *selector = malloc(sizeof(Selector));
	selector->listener = listener;
	selector->conexiones = list_create();

	return selector;
}

void esperarEventos(Selector *selector)
{
	agregarSocketsAListaDeEspera(selector);

	select(FD_SETSIZE, &(selector->fdSet), NULL, NULL, NULL);
}

static void agregarSocketsAListaDeEspera(Selector *selector)
{
	FD_ZERO(&(selector->fdSet));

	FD_SET(selector->listener->socketFd, &(selector->fdSet));

	int i;
	for (i = 0; i < list_size(selector->conexiones); i ++)
	{
		SocketConecta *conexion = list_get(selector->conexiones, i);
		FD_SET(conexion->socketFd, &(selector->fdSet));
	}
}

int hayNuevaConexion(Selector *selector)
{
	int socketFD;
	for (socketFD = 0; socketFD < FD_SETSIZE; ++socketFD)
		if (FD_ISSET(socketFD, &(selector->fdSet)))
			if (socketFD == selector->listener->socketFd)
			{
				SocketConecta *conexion = aceptarConexion(selector->listener);
				list_add(selector->conexiones, conexion);
				selector->nuevaConexion = conexion;
				return 1;
			}

	return 0;
}

int hayMensajesEntrantes(Selector *selector)
{
	int socketFD;
	for (socketFD = 0; socketFD < FD_SETSIZE; ++socketFD)
		if (FD_ISSET(socketFD, &(selector->fdSet)))
			if (socketFD != selector->listener->socketFd)
				return 1;

	return 0;
}

int hayConexionesCaidas(Selector *selector)
{
	int i;
	for (i = 0; i < list_size(selector->conexiones); i ++)
		if (! estaConectado(list_get(selector->conexiones, i)))
			return 1;

	return 0;
}

SocketConecta *obtenerNuevaConexion(Selector *selector)
{
	return selector->nuevaConexion;
}

t_list *obtenerConexionesActivas(Selector *selector)
{
	t_list *conexionesActivas = list_create();

	int i;
	for (i = 0; i < list_size(selector->conexiones); i ++)
	{
		SocketConecta *conexion = list_get(selector->conexiones, i);
		if (estaConectado(conexion))
			list_add(conexionesActivas, conexion);
	}

	return conexionesActivas;
}

t_list *obtenerConexionesConMensajesEntrantes(Selector *selector)
{
	t_list *conexionesConMensajesEntrantes = list_create();
	int socketFD;
	for (socketFD = 0; socketFD < FD_SETSIZE; socketFD ++)
	{
		if (FD_ISSET(socketFD, &(selector->fdSet)))
		{
			if (socketFD != selector->listener->socketFd)
			{
				SocketConecta *conexion = obtenerSocketConectaConFD(selector, socketFD);
				if (conexion != NO_EXISTE_SOCKET_EN_LISTA && estaConectado(conexion))
					list_add(conexionesConMensajesEntrantes, conexion);
			}
		}
	}

	return conexionesConMensajesEntrantes;
}

static SocketConecta *obtenerSocketConectaConFD(Selector *selector, int fd)
{
	int i;
	for (i = 0; i < list_size(selector->conexiones); i ++)
	{
		SocketConecta *sock = list_get(selector->conexiones, i);
		if (sock->socketFd == fd)
			return sock;
	}

	return NO_EXISTE_SOCKET_EN_LISTA;
}

t_list *obtenerConexionesCaidas(Selector *selector)
{
	t_list *conexionesCaidas = list_create();
	int i;
	for (i = 0; i < list_size(selector->conexiones); i ++)
	{
		SocketConecta *sock = list_get(selector->conexiones, i);
		if (! estaConectado(sock))
			list_add(conexionesCaidas, sock);
	}

	list_remove_by_condition(selector->conexiones, conexionEstaCaida);
	return conexionesCaidas;
}

static bool conexionEstaCaida(void *arg)
{
	return ! estaConectado((SocketConecta *) arg);
}

