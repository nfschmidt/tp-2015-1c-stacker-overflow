#ifndef SOCKETCONECTA_H_
#define SOCKETCONECTA_H_

#include <netdb.h>
#include <commons/collections/list.h>
#include <stdint.h>

#define OCURRIO_TIMEOUT -1

typedef enum {SOCKETC_CREADO, SOCKETC_CONECTADO, SOCKETC_CERRADO, SOCKETC_ERROR} EstadoSocketConecta;

typedef struct
{
	int socketFd;
	char ipServidor[16];
	char puertoServidor[6];
	EstadoSocketConecta estado;
} SocketConecta;


SocketConecta *newSocketConecta(const char *ipServidor, const char *puertoServidor);

void conectarAServidor(SocketConecta *sock);

ssize_t enviarMensaje(SocketConecta *sock, void *mensaje, size_t longitudMensaje);

ssize_t recibirMensaje(SocketConecta *sock, void *buffer, size_t longitudBuffer);

ssize_t recibirMensajeConTimeout(SocketConecta *sock, void *buffer, size_t longitudBuffer, int segundos);

int estaConectado(SocketConecta *sock);

void cerrarSocketConecta(SocketConecta *sock);

void deleteSocketConecta(SocketConecta *sock);

char *obtenerIPRemoto(SocketConecta *sock);

uint64_t enviarTodo(SocketConecta *sock, void *mensaje, uint64_t longitudMensaje);

uint64_t recibirTodo(SocketConecta *sock, void **buffer);


#endif
