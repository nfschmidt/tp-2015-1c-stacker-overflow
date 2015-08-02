/*
 * configFileSystem.c
 *
 *  Created on: 6/5/2015
 *      Author: utnso
 */
#include "configFileSystem.h"
#include "archivosConfiguracion.h"
#include  <string.h>
#include "estadoFileSystem.h"
#include <stdio.h>
#include <stdlib.h>
#include "logueo.h"



//levanta del archivo de configuracion el puerto y la cantidad nodos necesarios para que el FS pueda pasar a operativo
t_fileSystem *levantarArchivoConfiguracion(char *path)
{
	t_config *config;
	t_fileSystem *fileSystem = malloc(sizeof(t_fileSystem));
	config= config_create(path);

	fileSystem->puerto = obtenerParametroConfiguracion(config,"PUERTO_LISTEN",STRING);
	fileSystem->cantidadNodosConfiguracion = *(int*)obtenerParametroConfiguracion(config,"LISTA_NODOS",ENTERO);
	fileSystem->pathArchivoLog = obtenerParametroConfiguracion(config,"ARCHIVO_LOG",STRING);

	setPathArchivoDeLog(fileSystem->pathArchivoLog);
	return fileSystem;
}
