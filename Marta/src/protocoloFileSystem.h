#ifndef CONEXIONCONFILESYSTEM_H_
#define CONEXIONCONFILESYSTEM_H_

#include <commons/collections/list.h>
#include <libsocket.h>

void establecerConexionConFileSystem();

int consultarArchivoDisponible(char *pathMDFS);

int consultarCantidadBloquesArchivo(char *pathMDFS);

t_list *consultarDetallesBloqueArchivo(char *pathMDFS, int bloque);

#endif /* PROTOCOLOFILESYSTEM_H_ */
