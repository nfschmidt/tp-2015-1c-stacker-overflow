#ifndef TAREA_H_
#define TAREA_H_
#include "tiposJob.h"
#include "nodo.h"
#include <commons/collections/list.h>
#include <inttypes.h>

typedef enum {TAREA_MAP, TAREA_REDUCE} TipoTarea;

typedef enum {TAREA_NO_EJECUTADA, TAREA_EN_EJECUCION, TAREA_FINALIZADA} EstadoTarea;

typedef struct {
	char *nombreArchivoMDFS;
	t_nodo *nodo;
	char *nombreResultadoTemporal;
	EstadoTarea estado;
	TipoTarea tipo;
	uint32_t id;

	// Solo para MAP
	int numeroBloque;

	// Solo para REDUCE
	t_list *archivosRemotos;
	TipoJob soportaCombiner;
} t_tarea;


t_tarea *newTareaMap(char *archivo, int bloque);

t_tarea *newTareaReduce(t_list *archivosRemotos, TipoJob soportaCombiner);


t_nodo *tareaObtenerNodoQueEjecuto(t_tarea *tarea);

char *tareaObtenerNombreResultadoTemporal(t_tarea *tarea);

char *tareaObtenerNombreArchivoMDFS(t_tarea *tarea);

int tareaEstaNoEjecutada(t_tarea *tarea);

int tareaEstaEnEjecucion(t_tarea *tarea);

int tareaEstaFinalizada(t_tarea *tarea);


void tareaMarcarEnEjecucion(t_tarea *tarea, t_nodo *nodo ,char *nombreResultadoTemporal);

void tareaMarcarNoEjecutada(t_tarea *tarea);

void tareaMarcarFinalizada(t_tarea *tarea);


int tareaEsMap(t_tarea *tarea);

int tareaMapObtenerBloque(t_tarea *tarea);


int tareaEsReduce(t_tarea *tarea);

t_list *tareaReduceObtenerArchivosRemotos(t_tarea *tarea);

int tareaReduceEsSinCombiner(t_tarea *tarea);

int tareaReduceEsConCombiner(t_tarea *tarea);

t_nodo *tareaReduceObtenerNodoConMasArchivosRemotos(t_tarea *tarea);


#endif /* TAREA_H_ */
