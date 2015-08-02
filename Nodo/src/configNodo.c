/*
 * configNodo.c
 *
 *
 *  Created on: 8/5/2015
 *      Author: utnso
 */

#include "configNodo.h"
#include "archivosConfiguracion.h"
#include  <string.h>
#include <stdlib.h>
#include "logueo.h"


static ConfiguracionNodo *configuracionNodo = 0;

ConfiguracionNodo *levantarArchivoConfiguracion(char *path)
{
	configuracionNodo = malloc(sizeof(ConfiguracionNodo));
	t_config *config;
	config= config_create(path);

	configuracionNodo->puerto_fs = obtenerParametroConfiguracion(config,"PUERTO_FS", STRING);
	configuracionNodo->ip_fs = obtenerParametroConfiguracion(config,"IP_FS",STRING);
	configuracionNodo->pathEspacioDatos = obtenerParametroConfiguracion(config,"ARCHIVO_BIN",STRING);
	configuracionNodo->dirTemp = obtenerParametroConfiguracion(config,"DIR_TEM",STRING);
	configuracionNodo->nuevo = *((int*)obtenerParametroConfiguracion(config,"NODO_NUEVO",ENTERO));
	configuracionNodo->ip = obtenerParametroConfiguracion(config,"IP_NODO",STRING);
	configuracionNodo->puerto = obtenerParametroConfiguracion(config,"PUERTO_NODO",STRING);
	configuracionNodo->nombre = obtenerParametroConfiguracion(config,"NOMBRE",STRING);
	configuracionNodo->pathArchivoLog = obtenerParametroConfiguracion(config,"ARCHIVO_LOG",STRING);

	setPathArchivoDeLog(configuracionNodo->pathArchivoLog);

	return configuracionNodo;
}


char *obtenerPathEspacioTemporal()
{
	char *pathEspacioTemporal;

	// Si no hay nada seteado, usamos /tmp/
	if (configuracionNodo == 0 || configuracionNodo->dirTemp == NULL)
		pathEspacioTemporal = strdup("/tmp/");
	else if (configuracionNodo->dirTemp[strlen(configuracionNodo->dirTemp)] == '/') // Me fijo que tenga una '/' al final
		pathEspacioTemporal = strdup(configuracionNodo->dirTemp);
	else
	{ // si no tiene la '/' al final, se la agrego
		pathEspacioTemporal = malloc(strlen(configuracionNodo->dirTemp) + 2);
		strcpy(pathEspacioTemporal, configuracionNodo->dirTemp);
		strcat(pathEspacioTemporal, "/");
	}

	return pathEspacioTemporal;
}

ConfiguracionNodo *obtenerConfiguracionNodo()
{
	return configuracionNodo;
}
