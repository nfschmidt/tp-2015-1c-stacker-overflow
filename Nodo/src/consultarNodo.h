/*
 * consultarNodo.h
 *
 *  Created on: 20/5/2015
 *      Author: utnso
 */

#ifndef CONSULTARNODO_H_
#define CONSULTARNODO_H_

#include "libsocket.h"

#define CONEXION_NODO "N"
#define TENES_ARCHIVO "A"
#define SIGUIENTE_REGISTRO "R"
#define TERMINE_DE_PEDIR "T"

#define SI_TENGO_ARCHIVO 'S'
#define NO_TENGO_ARCHIVO 'N'

#define CAYO_SERVER_C	 "C"
#define CAYO_SERVER	 'C'
#define SI_TENGO_ARCHIVO 'S'
#define NO_TENGO_ARCHIVO 'N'


char tenesArchivo(SocketConecta *conexion, char *nombreArchivo);
char *getRegistroRemoto(SocketConecta *conexion);
char termineDePedir(SocketConecta *conexion);
void calloServerNodoConexion(char *mensaje);

#endif /* CONSULTARNODO_H_ */
