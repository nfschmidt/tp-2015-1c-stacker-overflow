#include "configJob.h"
#include "archivosConfiguracion.h"
#include  <string.h>
#include  <stdlib.h>
#include <commons/collections/list.h>

Job *levantarArchivoConfiguracion(char *path)
{
	Job *job = malloc(sizeof(Job));
	t_config *config;
	config= config_create(path);
	job->ip_marta= obtenerParametroConfiguracion(config,"IP_MARTA",STRING);
	job->puerto_marta= obtenerParametroConfiguracion(config,"PUERTO_MARTA",STRING);
	job->mapper= obtenerParametroConfiguracion(config,"MAPPER",STRING);
	job->reduce= obtenerParametroConfiguracion(config,"REDUCE",STRING);
	job->combiner= obtenerParametroConfiguracion(config,"COMBINER",STRING);
	job->archivos= obtenerParametroConfiguracion(config,"ARCHIVOS",LISTA);
	job->resultado= obtenerParametroConfiguracion(config,"RESULTADO",STRING);
	job->path_log= obtenerParametroConfiguracion(config,"ARCHIVO_LOG",STRING);
	return job;

}


