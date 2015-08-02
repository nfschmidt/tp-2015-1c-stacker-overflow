#ifndef ATENDERANODO_H_
#define ATENDERANODO_H_
#include "estructuraFS.h"
#include "nodo.h"
#include "libsocket.h"


void *atenderANodo(void *argConexion);
t_nodo *crearNodo(SocketConecta *conexion);
int validarExistenciaNombre(char *nombreNodo);



#endif /* ATENDERANODO_H_ */
