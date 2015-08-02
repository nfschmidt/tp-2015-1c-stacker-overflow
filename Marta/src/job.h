#ifndef JOB_H_
#define JOB_H_
#include "tarea.h"
#include "tiposJob.h"
#include <commons/collections/list.h>
#include <libsocket.h>

typedef struct {
	t_list *listaTareasMap;
	t_tarea *reduceFinal;
	t_list *listaTareasReduce;
	TipoJob tipo;
	SocketConecta *conexion;
}t_job;


t_job *newJob(TipoJob tipo);

void jobAgregarArchivoMDFS(t_job *job,char *nombreArchivo, int cantidadBloques);

t_tarea *jobObtenerSiguienteTarea(t_job *job);

int jobEstaTerminado(t_job *job);

t_tarea *jobObtenerTareaConId(t_job *job, uint32_t id);

void jobDarDeBajaNodo(t_job *job, char *nodo);

#endif /* JOB_H_ */
