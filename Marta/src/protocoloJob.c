#include "protocoloJob.h"
#include "logueo.h"
#include <string.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include "protocoloFileSystem.h"
#include "archivoRemoto.h"


#define MENSAJE_SOPORTA_COMBINER "SI"
#define MENSAJE_NO_SOPORTA_COMBINER "NO"
#define MENSAJE_FIN_ENVIO_LISTA_ARCHIVOS "Fin archivos"

#define MENSAJE_ES_TAREA_MAP "map"
#define MENSAJE_ES_TAREA_REDUCE "red"

#define MENSAJE_FIN_DE_CONEXION_CON_JOB "fin"

#define MENSAJE_SUBIDA_ARCHIVO_OK 1
#define MENSAJE_SUBIDA_ARCHIVO_NO_HAY_ESPACIO 2
#define MENSAJE_SUBIDA_ARCHIVO_SE_DESCONECTO_NODO 0

static void loguearEnvioDeTarea(t_job *job, t_tarea *tarea);

t_job *protocoloJobObtenerDatosJob(SocketConecta *conexion)
{
	char soporteCombiner[3];
	recibirMensaje(conexion, soporteCombiner, 2);
	if (!estaConectado(conexion))
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "El job se desconecto. Se cancela el job.");
		return NULL;
	}

	soporteCombiner[2] = '\0';

	t_list *listaArchivos = list_create();
	do
	{
		int longitudMensaje;
		recibirMensaje(conexion, &longitudMensaje, sizeof(int));
		if (!estaConectado(conexion))
		{
			loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "El job se desconecto. Se cancela el job.");
			return NULL;
		}

		char *pathArchivo = malloc(longitudMensaje + 1);
		recibirMensaje(conexion, pathArchivo, longitudMensaje);
		if (!estaConectado(conexion))
		{
			loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "El job se desconecto.");
			return NULL;
		}

		pathArchivo[longitudMensaje] = '\0';

		if (strcmp(pathArchivo, MENSAJE_FIN_ENVIO_LISTA_ARCHIVOS) == 0)
			break;
		else
			list_add(listaArchivos, pathArchivo);
	} while (1);


	TipoJob tipoJob = string_equals_ignore_case(soporteCombiner, MENSAJE_SOPORTA_COMBINER) ? CON_COMBINER : SIN_COMBINER;
	t_job *job = newJob(tipoJob);

	while (list_size(listaArchivos) > 0)
	{
		char *pathArchivoMDFS = list_get(listaArchivos, 0);

		int archivoEstaDisponible = consultarArchivoDisponible(pathArchivoMDFS);
		if (! archivoEstaDisponible)
		{
			loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "El archivo %s no se encuentra disponible. Se cancela el job.", pathArchivoMDFS);
			free(job);
			return NULL;
		}

		int cantidadBloquesDeArchivo = consultarCantidadBloquesArchivo(pathArchivoMDFS);

		jobAgregarArchivoMDFS(job, pathArchivoMDFS, cantidadBloquesDeArchivo);
		free(pathArchivoMDFS);
		list_remove(listaArchivos, 0);
	}

	job->conexion = conexion;

	return job;
}


int protocoloJobNotificarTarea(t_job *job, t_tarea *tarea)
{
	SocketConecta *conexion = job->conexion;

	if(!estaConectado(conexion))
		return 0;

	if (tareaEsMap(tarea))
		enviarMensaje(conexion, MENSAJE_ES_TAREA_MAP, strlen(MENSAJE_ES_TAREA_MAP) + 1);
	else
		enviarMensaje(conexion, MENSAJE_ES_TAREA_REDUCE, strlen(MENSAJE_ES_TAREA_REDUCE) + 1);

	int longitudMensaje = strlen(tarea->nodo->ip) + 1;
	if(!estaConectado(conexion))
		return 0;
	enviarMensaje(conexion, &longitudMensaje, sizeof(int));

	if(!estaConectado(conexion))
		return 0;
	enviarMensaje(conexion, tarea->nodo->ip, longitudMensaje);

	longitudMensaje = strlen(tarea->nodo->puerto) + 1;

	if(!estaConectado(conexion))
		return 0;
	enviarMensaje(conexion, &longitudMensaje, sizeof(int));

	if(!estaConectado(conexion))
		return 0;
	enviarMensaje(conexion, tarea->nodo->puerto, longitudMensaje);

	longitudMensaje = strlen(tarea->nombreResultadoTemporal) + 1;

	if(!estaConectado(conexion))
		return 0;
	enviarMensaje(conexion, &longitudMensaje, sizeof(int));

	if(!estaConectado(conexion))
		return 0;
	enviarMensaje(conexion, tarea->nombreResultadoTemporal, longitudMensaje);

	if (tareaEsMap(tarea))
	{
		if(!estaConectado(conexion))
			return 0;

		enviarMensaje(conexion, &(tarea->nodo->bloque), sizeof(int));
	}
	else
	{
		t_list *archivosRemotos = tareaReduceObtenerArchivosRemotos(tarea);
		int contador = 0;
		char *stringListaArchivosRemotos = string_new();
		while (contador < list_size(archivosRemotos))
		{
			char strArchivoRemoto[200];
			t_archivoRemoto *archivoRemoto = list_get(archivosRemotos, contador);

			sprintf(strArchivoRemoto, "%s:%s:%s|",
					archivoRemoto->nodo->ip,
					archivoRemoto->nodo->puerto,
					archivoRemotoObtenerNombreArchivo(archivoRemoto));

			string_append(&stringListaArchivosRemotos, strArchivoRemoto);

			contador ++;
		}
		stringListaArchivosRemotos[strlen(stringListaArchivosRemotos) - 1] = '\0';
		longitudMensaje = strlen(stringListaArchivosRemotos) + 1;

		if(!estaConectado(conexion))
			return 0;
		enviarTodo(conexion, stringListaArchivosRemotos, longitudMensaje);

		if(!estaConectado(conexion))
			return 0;
	}

	if(!estaConectado(conexion))
		return 0;
	enviarMensaje(conexion, &(tarea->id), sizeof(int));

	if (tareaEsReduce(tarea))
	{
		int esReduceFinal = (job->reduceFinal != NULL && tarea->id == job->reduceFinal->id) ? 1 : 0;

		if(!estaConectado(conexion))
			return 0;
		enviarMensaje(conexion, &esReduceFinal, sizeof(int));
	}

	if(!estaConectado(conexion))
		return 0;

	loguearEnvioDeTarea(job, tarea);

	return 1;
}

int protocoloJobObtenerResultadoDeSubidaDeArchivo(t_job *job)
{
	SocketConecta *conexion = job->conexion;

	int resultadoSubidaArchivo;
	int idTarea;

	if(!estaConectado(conexion))
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "El Job se desconecto al intentar recibir el resultado de la subida del archivo al MDFS. Cancelando el job");
		return -1;
	}

	recibirMensaje(conexion, &resultadoSubidaArchivo, sizeof(int));
	if (!estaConectado(job->conexion))
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "El Job se desconecto al intentar recibir el resultado de la subida del archivo al MDFS. Cancelando el job");
		return -1;
	}

	recibirMensaje(conexion, &idTarea, sizeof(int));
	if (!estaConectado(job->conexion))
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "El Job se desconecto al intentar recibir el resultado de la subida del archivo al MDFS. Cancelando el job");
		return -1;
	}

	switch (resultadoSubidaArchivo)
	{
	case MENSAJE_SUBIDA_ARCHIVO_OK:
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "El resultado del job fue subido correctamente al MDFS. Job finalizado.");
		break;
	case MENSAJE_SUBIDA_ARCHIVO_NO_HAY_ESPACIO:
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "El resultado del job no pudo ser subido al MDFS. Job finalizado.");
		break;
	case MENSAJE_SUBIDA_ARCHIVO_SE_DESCONECTO_NODO:
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "El resultado del job no pudo ser subido al MDFS. Nodo desconectado. Job finalizado.");
		break;
	}

	return resultadoSubidaArchivo;
}

void protocoloJobNotificarFin(t_job *job)
{
	if (estaConectado(job->conexion))
	{
		enviarMensaje(job->conexion, MENSAJE_FIN_DE_CONEXION_CON_JOB, 4);
		//loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "Se envio mensaje de finalizacion de job.");
	}
}

static void loguearEnvioDeTarea(t_job *job, t_tarea *tarea)
{
	loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Se envio pedido de ejecucion de tarea de %s a job %d", tareaEsMap(tarea) ? "MAP" : "REDUCE", tarea->id >> 16);
	loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Nodo: %s - %s:%s", tarea->nodo->nombre, tarea->nodo->ip, tarea->nodo->puerto);

	if (tareaEsMap(tarea))
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Archivo: %s - Bloque: %d", tarea->nombreArchivoMDFS, tarea->numeroBloque);
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Bloque en nodo: %d", tarea->nodo->bloque);
	}
	else
	{
		if(job->reduceFinal != NULL && job->reduceFinal->id == tarea->id)
			loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Reduce final");
		else
			loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Reduce local");
	}

	loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Archivo temporal: %s", tarea->nombreResultadoTemporal);
}
