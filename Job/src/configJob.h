#ifndef CONFIGJOB_H_
#define CONFIGJOB_H_

#include <commons/config.h>
#include <commons/collections/list.h>

typedef struct
{
	char *ip_marta;
	char *puerto_marta;
	char *mapper;
	char *reduce;
	char *combiner; //bool
	char **archivos;
	char *resultado;
	char *path_log;

}Job;

Job *levantarArchivoConfiguracion(char *path);

#endif /* CONFIGJOB_H_ */
