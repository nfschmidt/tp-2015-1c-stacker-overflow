#ifndef RESPONDERANODO_H_
#define RESPONDERANODO_H_

#include "estructuraFS.h"
#include "libsocket.h"

int grabarBloques(t_nodo *nodo, char *bloque,t_bloque *bloqueCopia);
off_t tamanioDelBloque(char *bloque);
int enviarTamanioBloque(SocketConecta *conexion,char *bloque);
int enviarBloque(t_nodo *nodo,char *bloque, t_bloque *copiaBloque,SocketConecta *conexion);

#endif /* RESPONDERANODO_H_ */
