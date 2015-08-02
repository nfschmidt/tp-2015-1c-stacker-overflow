/*
 * configFileSystem.h
 *
 *  Created on: 6/5/2015
 *      Author: utnso
 */

#ifndef CONFIGFILESYSTEM_H_
#define CONFIGFILESYSTEM_H_
#include <commons/collections/list.h>

typedef struct {
	char *puerto;
	int cantidadNodosConfiguracion;
	char *pathArchivoLog;
}t_fileSystem;

t_fileSystem *levantarArchivoConfiguracion(char *path);


#endif /* CONFIGFILESYSTEM_H_ */
