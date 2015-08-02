/*
 * job.c
 *
 *  Created on: 14/7/2015
 *      Author: utnso
 */
#include <stdlib.h>
#include "job.h"
#include "tarea.h"
#include <string.h>
#include "archivoRemoto.h"
#include <stdio.h>

static int todosLosMapsFinalizados(t_job *job);
static t_list *obtenerArchivosRemotosJob(t_job *job);
static int reduceFinalNoEjecutado(t_job *job);
static t_tarea *obtenerTareaReduceSinCombiner(t_job *job);
static t_tarea *obtenerTareaReduceConCombiner(t_job *job);
static t_list *obtenerListaArchivosResultadosDeMapDeNodo(t_job *job, char *nombreNodo);
static int nodoFueReducido(t_job *job,char *nombreNodo);
static t_tarea *obtenerReduceNoEjecutado(t_job *job);
static t_list *obtenerArchivosResultadosDeReduce(t_job *job);
static int hayReducesEnEjecucion(t_job *job);
static int hayMapsEnEjecucion(t_job *job);


t_job *newJob(TipoJob tipo)
{
	t_job *job = malloc(sizeof(t_job));
	job->tipo = tipo;
	job->listaTareasMap = list_create();
	job->listaTareasReduce = list_create();
	job->reduceFinal = NULL;
	return job;
}



void jobAgregarArchivoMDFS(t_job *job,char *nombreArchivo, int cantidadBloques)
{
	int numeroDeBloque = 0;
	for(numeroDeBloque = 0;numeroDeBloque < cantidadBloques;numeroDeBloque++)
	{
		list_add(job->listaTareasMap,newTareaMap(nombreArchivo,numeroDeBloque));
	}
}


t_tarea *jobObtenerSiguienteTarea(t_job *job)
{
	int contador = 0;

	while(contador < list_size(job->listaTareasMap))
	{
		t_tarea *tarea = list_get(job->listaTareasMap,contador);
		if(tareaEstaNoEjecutada(tarea))
			return tarea;

		contador++;
	}

	if (! hayMapsEnEjecucion(job))
	{
		if(job->tipo == SIN_COMBINER)
			return obtenerTareaReduceSinCombiner(job);
		else
			return obtenerTareaReduceConCombiner(job);
	}

	return NULL;
}

static t_tarea *obtenerTareaReduceSinCombiner(t_job *job)
{
	if(reduceFinalNoEjecutado(job))
	{
		if(todosLosMapsFinalizados(job))
		{
			t_list *archivosRemotos = obtenerArchivosRemotosJob(job);
			job->reduceFinal = newTareaReduce(archivosRemotos, job->tipo);
			return job->reduceFinal;
		}
	}

	return NULL;
}

static t_tarea *obtenerTareaReduceConCombiner(t_job *job)
{
	int contador = 0;

	t_tarea *reduceNoEjecutado = obtenerReduceNoEjecutado(job);
	if (reduceNoEjecutado != NULL)
		return reduceNoEjecutado;

	while (contador < list_size(job->listaTareasMap))
	{
		t_tarea *tareaMap = list_get(job->listaTareasMap, contador);
		if (! tareaEstaFinalizada(tareaMap))
			return NULL;
		t_nodo *nodo = tareaObtenerNodoQueEjecuto(tareaMap);
		t_list *archivosTemporalesEnNodo = obtenerListaArchivosResultadosDeMapDeNodo(job, nodo->nombre);
		if(! nodoFueReducido(job,nodo->nombre))
		{
			t_tarea *tareaAEjecutar = newTareaReduce(archivosTemporalesEnNodo, job->tipo);
			list_add(job->listaTareasReduce, tareaAEjecutar);
			return tareaAEjecutar;
		}
		list_destroy(archivosTemporalesEnNodo);
		contador ++;
	}

	if((! hayReducesEnEjecucion(job)) && reduceFinalNoEjecutado(job))
	{
		t_list *listaArchivosAReducir = obtenerArchivosResultadosDeReduce(job);
		job->reduceFinal = newTareaReduce(listaArchivosAReducir, job->tipo);
		return job->reduceFinal;
	}

	return NULL;
}

static t_tarea *obtenerReduceNoEjecutado(t_job *job)
{
	int contador = 0;

	while(contador < list_size(job->listaTareasReduce))
	{
		t_tarea *tarea = list_get(job->listaTareasReduce,contador);

		if(tareaEstaNoEjecutada(tarea))
			return tarea;

		contador ++;
	}

	return NULL;
}

static int nodoFueReducido(t_job *job,char *nombreNodo)
{
	int contador = 0;
	while(contador < list_size(job->listaTareasReduce))
	{
		t_tarea *tarea = list_get(job->listaTareasReduce,contador);
		t_nodo *nodoQueEjecuto = tareaObtenerNodoQueEjecuto(tarea);
		if(nodoQueEjecuto != NULL && strcmp(nodoQueEjecuto->nombre, nombreNodo) == 0)
		{
			return 1;
		}
		contador++;
	}
	return 0;
}

static t_list *obtenerListaArchivosResultadosDeMapDeNodo(t_job *job, char *nombreNodo)
{
	int contador = 0;
	t_list *listaArchivosResultadosDeNodos = list_create();

	while (contador < list_size(job->listaTareasMap))
	{
		t_tarea *tarea = list_get(job->listaTareasMap, contador);

		t_nodo *nodoTarea = tareaObtenerNodoQueEjecuto(tarea);

		if (strcmp(nombreNodo, nodoTarea->nombre) == 0)
		{
			t_archivoRemoto *archivo = newArchivoRemoto(nodoTarea, tareaObtenerNombreResultadoTemporal(tarea));
			list_add(listaArchivosResultadosDeNodos, archivo);
		}

		contador ++;
	}

	return listaArchivosResultadosDeNodos;
}

static int reduceFinalNoEjecutado(t_job *job)
{
	if(job->reduceFinal == NULL || tareaEstaNoEjecutada(job->reduceFinal))
		return 1;

	return 0;

}


static int todosLosMapsFinalizados(t_job *job)
{
	int contador = 0;
	while(contador < list_size(job->listaTareasMap))
	{
		t_tarea *tarea = list_get(job->listaTareasMap,contador);
		if(!tareaEstaFinalizada(tarea))
			return 0;

		contador++;
	}
	return 1;
}

static t_list *obtenerArchivosRemotosJob(t_job *job)
{
	t_list *archivosRemotos = list_create();

	int indice;
	for (indice = 0; indice < list_size(job->listaTareasMap); indice ++)
	{
		t_tarea *tareaMap = list_get(job->listaTareasMap, indice);
		list_add(archivosRemotos,
				newArchivoRemoto(tareaObtenerNodoQueEjecuto(tareaMap),
						tareaObtenerNombreResultadoTemporal(tareaMap)));
	}
	return archivosRemotos;
}

static t_list *obtenerArchivosResultadosDeReduce(t_job *job)
{
	t_list *archivosRemotos = list_create();

	int indice;
	for (indice = 0; indice < list_size(job->listaTareasReduce); indice ++)
	{
		t_tarea *reduce = list_get(job->listaTareasReduce, indice);
		list_add(archivosRemotos,
				newArchivoRemoto(tareaObtenerNodoQueEjecuto(reduce),
						tareaObtenerNombreResultadoTemporal(reduce)));
	}

	return archivosRemotos;
}

int jobEstaTerminado(t_job *job)
{
	return job->reduceFinal != NULL && tareaEstaFinalizada(job->reduceFinal);
}

t_tarea *jobObtenerTareaConId(t_job *job, uint32_t id)
{
	int contador = 0;
	while (contador < list_size(job->listaTareasMap))
	{
		t_tarea *tarea = list_get(job->listaTareasMap, contador);
		if (tarea != NULL)
		{
			if (tarea->id == id)
				return tarea;
		}

		contador ++;
	}

	contador = 0;
	while (contador < list_size(job->listaTareasReduce))
	{
		t_tarea *tarea = list_get(job->listaTareasReduce, contador);
		if (tarea != NULL)
		{
			if (tarea->id == id)
				return tarea;
		}

		contador ++;
	}

	if (job->reduceFinal != NULL && job->reduceFinal->id == id)
		return job->reduceFinal;

	return NULL;
}

void jobDarDeBajaNodo(t_job *job, char *nodo)
{
	int contador = 0;
	while (contador < list_size(job->listaTareasMap))
	{
		t_tarea *map = list_get(job->listaTareasMap, contador);

		if ((tareaEstaEnEjecucion(map) || tareaEstaFinalizada(map)) && map->nodo != NULL && strcmp(map->nodo->nombre, nodo) == 0)
			tareaMarcarNoEjecutada(map);

		contador ++;
	}
}

static int hayReducesEnEjecucion(t_job *job)
{
	int contador = 0;
	while(contador < list_size(job->listaTareasReduce))
	{
		t_tarea *reduce = list_get(job->listaTareasReduce, contador);
		if (tareaEstaEnEjecucion(reduce))
			return 1;

		contador ++;
	}

	return 0;
}

static int hayMapsEnEjecucion(t_job *job)
{
	int contador = 0;
	while(contador < list_size(job->listaTareasMap))
	{
		t_tarea *map = list_get(job->listaTareasMap, contador);
		if (tareaEstaEnEjecucion(map))
			return 1;

		contador ++;
	}

	return 0;
}
