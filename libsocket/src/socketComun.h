#ifndef SOCKETCOMUN_H_
#define SOCKETCOMUN_H_

#include <netdb.h>

#define ERROR_SOCKET -1
#define ERROR_CONNECT -1
#define ERROR_BIND -1
#define ERROR_LISTEN -1
#define ERROR_ACCEPT -1
#define ERROR_SETSOCKOPT -1
#define ERROR_RECV -1
#define ERROR_SEND -1
#define ERROR_CLOSE -1


#define GETADDRINFO_OK 0

#define FIN_DE_CONEXION 0


int crearSocket();

int conectar(int socketFd, const char *ipServidor, const char *puerto);

int bindear(int socketFd, const char *ipServidor, const char *puerto);

int escuchar(int socketFd, int longitudDeColaDeEspera);

int aceptar(int socketFd);

ssize_t recibir(int socketFd, void *buffer, size_t longitudBuffer);

ssize_t enviar(int socketFd, void *mensaje, size_t longitudMensaje);

int cerrarSocket(int socketFd);

int selecSocket(int ultimoSocket, fd_set conjunto_descrip);

int obtenerDatos(int numeroSocket, fd_set conjunto_descrip);
#endif
