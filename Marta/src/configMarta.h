#ifndef CONFIGFS_H_
#define CONFIGFS_H_

#include <commons/config.h>
#include <commons/collections/list.h>
#include <libsocket.h>

typedef struct
{
	char *ipFileSystem;
	char *puertoFileSystem;
	char *pathArchivoLog;
	char *puertoMarta;

	int pesoMap;
	int pesoReduce;
	int cargaMaxima;
} t_configuracionMarta;

t_configuracionMarta *levantarArchivoConfiguracion(char *path);

#endif /* CONFIGJOB_H_ */
