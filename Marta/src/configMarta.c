#include "configMarta.h"
#include "archivosConfiguracion.h"
#include  <string.h>
#include  <stdlib.h>
#include <commons/collections/list.h>

t_configuracionMarta *levantarArchivoConfiguracion(char *path)
{
	t_configuracionMarta *configMarta = malloc(sizeof(t_configuracionMarta));

	t_config *config;
	config = config_create(path);

	configMarta->puertoMarta = obtenerParametroConfiguracion(config,"PUERTO", STRING);
	configMarta->ipFileSystem = obtenerParametroConfiguracion(config, "IP_FS", STRING);
	configMarta->puertoFileSystem = obtenerParametroConfiguracion(config, "PUERTO_FS", STRING);
	configMarta->pathArchivoLog = obtenerParametroConfiguracion(config, "ARCHIVO_LOG", STRING);

	configMarta->pesoMap = *((int*)(obtenerParametroConfiguracion(config, "PESO_MAP", ENTERO)));
	configMarta->pesoReduce = *((int*)(obtenerParametroConfiguracion(config, "PESO_REDUCE", ENTERO)));
	configMarta->cargaMaxima = *((int*)(obtenerParametroConfiguracion(config, "CARGA_MAXIMA", ENTERO)));

	return configMarta;
}
