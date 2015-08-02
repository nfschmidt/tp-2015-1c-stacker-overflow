/*
 * copiarArchivoAMDFS.h
 *
 *  Created on: 20/6/2015
 *      Author: utnso
 */

#ifndef COPIARARCHIVOAMDFS_H_
#define COPIARARCHIVOAMDFS_H_

#include <commons/collections/list.h>

#define TAMANIO_MAXIMO_BLOQUE 20971520
#define TAMANIO_ANTEULTIMO_BLOQUE 20971519


int copiarArchivoAMDFS(char *ruta,char *rutaMDFS, char *nombreArchivo);
#endif /* COPIARARCHIVOAMDFS_H_ */
