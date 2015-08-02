/*
 * responderAFileSystem.h
 *
 *  Created on: 2/6/2015
 *      Author: utnso
 */

#ifndef RESPONDERAFILESYSTEM_H_
#define RESPONDERAFILESYSTEM_H_
#include "ArchivoLocal.h"
#include "libsocket.h"
#include "espacioDatos.h"
#include "configNodo.h"

ArchivoLocal *abrirArchivoGrabar(SocketConecta *conexion);
void escribirInformacion(SocketConecta *conexion,ArchivoLocal *archivoAEscribir);
void enviarFileSystemCantidadBloques(SocketConecta *conexion, EspacioDatos *espacio,ConfiguracionNodo *nodo);
void enviarBloqueFS(SocketConecta *conexion,int numeroBloque, EspacioDatos *espacio,off_t cantidadBytesBloque);

#endif /* RESPONDERAFILESYSTEM_H_ */
