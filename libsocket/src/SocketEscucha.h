#ifndef SOCKETESCUCHA_H_
#define SOCKETESCUCHA_H_

#include "SocketConecta.h"

typedef enum {SOCKETE_BINDEADO, SOCKETE_ESCUCHANDO, SOCKETE_CERRADO, SOCKETE_ERROR} EstadoSocketEscucha;

typedef struct
{
	int socketFd;
	EstadoSocketEscucha estado;
} SocketEscucha;


SocketEscucha *newSocketEscucha(const char *ip, const char *puerto);

void escucharConexiones(SocketEscucha *sock);

SocketConecta *aceptarConexion(SocketEscucha *sock);

void cerrarSocketEscucha(SocketEscucha *sock);

void deleteSocketEscucha(SocketEscucha *sock);


#endif
