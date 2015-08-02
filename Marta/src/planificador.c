#include "planificador.h"
#include "cargaNodos.h"
#include "generadorNombreArchivo.h"
#include "commons/collections/dictionary.h"
#include <commons/string.h>
#include <string.h>
#include <stdio.h>
#include "logueo.h"

#include "protocoloFileSystem.h"

static uint32_t obtenerIdTareaDeJob(uint32_t idJob);
static void darDeBajaNodoEnTodosLosJobsActivos(char *nombreNodo);

void (*notificarFinalizacionJob)(t_job *job) = NULL;
int (*notificarTareaAJob)(t_job *job, t_tarea *tarea) = NULL;
t_list *(*obtenerDetalleDeBloques)(char *pathMDFS, int bloque) = consultarDetallesBloqueArchivo;
int (*recibirResultadoDeSubidaDeArchivoAMDFS)(t_job *job) = NULL;

static uint32_t _idUltimoJobCreado;
static uint32_t _idUltimoJobPlanificado;
static uint32_t _idUltimaTarea;
static t_dictionary *_diccionarioJobs;
static t_cargaNodos *_cargaNodos;


static void darDeBajaJob(uint32_t idJob);


void planificadorInicializar()
{
	_cargaNodos = newCargaNodos();
	_idUltimoJobCreado = 0;
	_idUltimoJobPlanificado = 0;
	_idUltimaTarea = 0;
	_diccionarioJobs = dictionary_create();
}

void planificadorAgregarNodo(t_nodo *nodo)
{
	cargaNodosAgregarNodo(_cargaNodos, nodo);
}

void planificadorAgregarJob(t_job *job)
{
	_idUltimoJobCreado ++;
	char *keyJob = string_itoa(_idUltimoJobCreado);

	dictionary_put(_diccionarioJobs, keyJob, job);
}

t_tarea *planificadorObtenerSiguienteTareaYComunicarlaAJob()
{
	uint32_t idJobInicial = (_idUltimoJobPlanificado + 1) % (_idUltimoJobCreado + 1);
	uint32_t idJob = idJobInicial;

	do
	{
		char *jobKey = string_itoa(idJob);
		if (dictionary_has_key(_diccionarioJobs, jobKey))
		{
			t_job *job = dictionary_get(_diccionarioJobs, jobKey);
			t_tarea *tarea = jobObtenerSiguienteTarea(job);
			if (tarea != NULL)
			{
				t_nodo *nodo;
				if (tareaEsMap(tarea))
				{
					t_list *listaNodosConBloques = obtenerDetalleDeBloques(tareaObtenerNombreArchivoMDFS(tarea), tareaMapObtenerBloque(tarea));
					if (list_is_empty(listaNodosConBloques))
					{
						loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "El archivo %s no se encuentra disponible. Se finaliza el JOB.", tarea->nombreArchivoMDFS);
						darDeBajaJob(idJob);
						notificarFinalizacionJob(job);
						nodo = NULL;
					}
					else
						nodo = cargaNodosObtenerNodoDisponibleParaMap(_cargaNodos, listaNodosConBloques);
				}
				else
					nodo = cargaNodosObtenerNodoDisponibleParaReduce(_cargaNodos, tarea);

				if (nodo != NULL)
				{
					if (tareaEsMap(tarea))
					{
						tareaMarcarEnEjecucion(tarea, nodo, obtenerNombreArchivoResultadoTemporal(TAREA_MAP));
						nodoAsignarCargaMap(nodo);
					}
					else
					{
						tareaMarcarEnEjecucion(tarea, nodo, obtenerNombreArchivoResultadoTemporal(TAREA_REDUCE));
						nodoAsignarCargaReduce(nodo);
					}

					tarea->id = obtenerIdTareaDeJob(idJob);
					if(notificarTareaAJob(job, tarea))
					{
						_idUltimoJobPlanificado = idJob;
						return tarea;
					}
					else
					{
						loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "No se pudo establecer conexion con el job %d. Se finaliza el job.", idJob);
						darDeBajaJob(idJob);
					}
				}
			}
		}

		idJob = (idJob + 1) % (_idUltimoJobCreado + 1);
	} while(idJob != idJobInicial);

	return NULL;
}


void planificadorMarcarTareaFinalizada(uint32_t idTarea)
{
	t_tarea *tarea = planificadorObtenerTareaDeId(idTarea);

	if (tarea != NULL)
	{
		tareaMarcarFinalizada(tarea);
		t_nodo *nodoQueEjecuto = cargaNodosObtenerNodoPorNombre(_cargaNodos, tareaObtenerNodoQueEjecuto(tarea)->nombre);
		if (nodoQueEjecuto != NULL)
		{
			if (tareaEsMap(tarea))
				nodoLiberarCargaMap(nodoQueEjecuto);
			else
				nodoLiberarCargaReduce(nodoQueEjecuto);
		}
	}

	t_job *job = planificadorObtenerJobDeTarea(idTarea);
	if (job != NULL && jobEstaTerminado(job))
	{
		int resultadoDeSubida = recibirResultadoDeSubidaDeArchivoAMDFS(job);
		if (resultadoDeSubida > -1)
			notificarFinalizacionJob(job);

		uint32_t idJob = idTarea >> 16;
		darDeBajaJob(idJob);
	}
}

void planificadorMarcarTareaFallida(uint32_t idTarea)
{
	t_tarea *tarea = planificadorObtenerTareaDeId(idTarea);

	if (tarea != NULL)
	{
		tareaMarcarNoEjecutada(tarea);
		t_nodo *nodoQueEjecuto = cargaNodosObtenerNodoPorNombre(_cargaNodos, tareaObtenerNodoQueEjecuto(tarea)->nombre);

		if (nodoQueEjecuto != NULL)
		{
			if (tareaEsMap(tarea))
				nodoLiberarCargaMap(nodoQueEjecuto);
			else
				nodoLiberarCargaReduce(nodoQueEjecuto);
		}

		if (tareaEsReduce(tarea))
		{
			uint32_t idJob = idTarea >> 16;
			darDeBajaJob(idJob);
		}
	}
}

void planificadorMarcarTareaFallidaPorBajaDeNodo(uint32_t idTarea)
{
	t_tarea *tarea = planificadorObtenerTareaDeId(idTarea);

	if (tarea == NULL)
		return;

	//t_job *job = planificadorObtenerJobDeTarea(idTarea);
	darDeBajaNodoEnTodosLosJobsActivos(tarea->nodo->nombre);
	planificadorMarcarTareaFallida(idTarea);
	cargaNodosDarDeBajaNodo(_cargaNodos, tarea->nodo->nombre);
}

static void darDeBajaNodoEnTodosLosJobsActivos(char *nombreNodo)
{
	uint32_t idJob;

	for (idJob = 1 ; idJob <= _idUltimoJobCreado; idJob ++)
	{
		char *jobKey = string_itoa(idJob);
		if (dictionary_has_key(_diccionarioJobs, jobKey))
		{
			t_job *job = dictionary_get(_diccionarioJobs, jobKey);
			jobDarDeBajaNodo(job, nombreNodo);
		}
	}
}

static uint32_t obtenerIdTareaDeJob(uint32_t idJob)
{
	_idUltimaTarea ++;
	return (idJob << 16) | _idUltimaTarea;
}

t_tarea *planificadorObtenerTareaDeId(uint32_t idTarea)
{
	t_tarea *tarea = NULL;
	t_job *job = planificadorObtenerJobDeTarea(idTarea);

	if (job != NULL)
		tarea = jobObtenerTareaConId(job, idTarea);

	return tarea;
}

t_job *planificadorObtenerJobDeTarea(uint32_t idTarea)
{
	uint32_t idJob = idTarea >> 16;
	char *jobKey = string_itoa(idJob);
	t_job *job = NULL;

	if (dictionary_has_key(_diccionarioJobs, jobKey))
		job = dictionary_get(_diccionarioJobs, jobKey);

	return job;
}

static void darDeBajaJob(uint32_t idJob)
{
	char *jobKey = string_itoa(idJob);
	if (dictionary_has_key(_diccionarioJobs, jobKey))
	{
		t_job *job = dictionary_remove(_diccionarioJobs, jobKey);
		int i;for (i = 0; i < list_size(job->listaTareasMap); i ++)
		{
			t_tarea *tarea = list_get(job->listaTareasMap, i);
			if (tareaEstaEnEjecucion(tarea))
			{
				t_nodo *nodo = cargaNodosObtenerNodoPorNombre(_cargaNodos, tarea->nodo->nombre);
				if(nodo != NULL)
					nodoLiberarCargaMap(nodo);
			}
		}

		for (i = 0; i < list_size(job->listaTareasReduce); i ++)
		{
			t_tarea *tarea = list_get(job->listaTareasReduce, i);
			if (tareaEstaEnEjecucion(tarea))
			{
				t_nodo *nodo = cargaNodosObtenerNodoPorNombre(_cargaNodos, tarea->nodo->nombre);
				if(nodo != NULL)
					nodoLiberarCargaReduce(nodo);
			}
		}

		if (job->reduceFinal != NULL && tareaEstaEnEjecucion(job->reduceFinal))
		{
			t_nodo *nodo = cargaNodosObtenerNodoPorNombre(_cargaNodos, job->reduceFinal->nodo->nombre);
			if(nodo != NULL)
				nodoLiberarCargaReduce(nodo);
		}

		notificarFinalizacionJob(job);

	}

}

uint32_t planificadorDarDeBajaJobPorDesconexion(SocketConecta *conexion)
{
	uint32_t idJob = 0;

	do
	{
		char *keyJob = string_itoa(idJob);
		if(dictionary_has_key(_diccionarioJobs, keyJob))
		{
			t_job *job = dictionary_get(_diccionarioJobs, keyJob);
			if (job->conexion->socketFd == conexion->socketFd)
			{
				darDeBajaJob(idJob);
				return idJob;
			}
		}

		idJob ++;
	} while(idJob <= _idUltimoJobCreado);

	return -1;
}
