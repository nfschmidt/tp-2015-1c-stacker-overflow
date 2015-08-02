#include "commons/log.h"
#include "logs.h"
#include "commons/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define TAMANIOMENSAJE 1000

pthread_mutex_t semaforo;
static char *path_log;

void iniciarYEstablecerRutaArchivoLog(char *ruta)
{
	path_log = strdup(ruta);
	pthread_mutex_init(&semaforo,NULL);
}

void borrarSemaforoLogs()
{
	pthread_mutex_destroy(&semaforo);
}

void logDebug(char *mensaje)
{
	pthread_mutex_lock(&semaforo);
	t_log *Log;
	Log = log_create(path_log,"JOB",1,LOG_LEVEL_DEBUG);
	log_debug(Log,mensaje);
	log_destroy(Log);
	pthread_mutex_unlock(&semaforo);
}

void logConexionMarta(char *mensaje,char *ipMarta,char *puertoMarta)
{
	pthread_mutex_lock(&semaforo);
	t_log *Log;
	Log = log_create(path_log,"JOB",1,LOG_LEVEL_INFO);
	log_info(Log,mensaje,ipMarta,puertoMarta);
	log_destroy(Log);
	pthread_mutex_unlock(&semaforo);
}

void logCreacionHiloMapper(char *mensaje,char *ipNodo,char *puertoNodo,char *mapper)
{
	pthread_mutex_lock(&semaforo);
	t_log *Log;
	Log = log_create(path_log,"JOB",1,LOG_LEVEL_INFO);
	log_info(Log,mensaje,ipNodo,puertoNodo,mapper);
	log_destroy(Log);
	pthread_mutex_unlock(&semaforo);
}

void logCreacionHiloReducer(char *mensaje,char *ipNodo,char *puertoNodo,char *reduce,char *resultado)
{
	pthread_mutex_lock(&semaforo);
	t_log *Log;
	Log = log_create(path_log,"JOB",1,LOG_LEVEL_INFO);
	log_info(Log,mensaje,ipNodo,puertoNodo,reduce,resultado);
	log_destroy(Log);
	pthread_mutex_unlock(&semaforo);
}

void logConMensaje(char *mensaje)
{
	pthread_mutex_lock(&semaforo);
	t_log *Log;
	Log = log_create(path_log,"JOB",1,LOG_LEVEL_INFO);
	log_info(Log,mensaje);
	log_destroy(Log);
	pthread_mutex_unlock(&semaforo);
}

void logCabeceraEnvia(char *cabecera)
{
	pthread_mutex_lock(&semaforo);
	t_log *Log;
	Log = log_create(path_log,"JOB",0,LOG_LEVEL_INFO);
	char *mensaje=malloc(TAMANIOMENSAJE);
	strcpy(mensaje,"Se envio un mensaje con cabecera: ");
	strcat(mensaje,cabecera);
	log_info(Log,mensaje);
	log_destroy(Log);
	free(mensaje);
	pthread_mutex_unlock(&semaforo);
}

void logCabeceraRecibe(char *cabecera)
{
	pthread_mutex_lock(&semaforo);
	t_log *Log;
	Log = log_create(path_log,"JOB",0,LOG_LEVEL_INFO);
	char *mensaje=malloc(TAMANIOMENSAJE);;
	strcpy(mensaje,"Se recibio un mensaje con cabecera: ");
	strcat(mensaje,cabecera);
	log_info(Log,mensaje);
	log_destroy(Log);
	free(mensaje);
	pthread_mutex_unlock(&semaforo);
}

void logOperacion(char *mensaje,int idOp)
{
	pthread_mutex_lock(&semaforo);
	t_log *Log;
	Log = log_create(path_log,"JOB",1,LOG_LEVEL_INFO);
	log_info(Log,mensaje,idOp);
	log_destroy(Log);
	pthread_mutex_unlock(&semaforo);
}

void logFinJob(char *mensaje,char *rutinaMap,char *rutinaReduce,char *resultado)
{
	pthread_mutex_lock(&semaforo);
	t_log *Log;
	Log = log_create(path_log,"JOB",1,LOG_LEVEL_INFO);
	log_info(Log,rutinaMap, rutinaReduce,resultado);
	log_destroy(Log);
	pthread_mutex_unlock(&semaforo);
}


