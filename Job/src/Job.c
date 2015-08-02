#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <libsocket.h>
#include <commons/collections/list.h>
#include <string.h>
#include "logs.h"
#include "configJob.h"
#include "archivosConfiguracion.h"
#include "hiloMapper.h"
#include "hiloReducer.h"

void recibirYChequearConexion(SocketConecta *socket,void * mensaje, int tam);
void finalizarJob(int cantidadHilos,t_list *listaHilos,SocketConecta *conectarAMarta,Job *job,int i);
extern pthread_mutex_t semaforoMarta;

int main(int argc, char *argv[]) {

	if(argc!=2)//Validar que reciba archivo de configuracion por parametro
	{
		fprintf(stderr,"ERROR: el job debe recibir por parametro el path del archivo de configuracion \n");
		exit(1);
	}
	pthread_mutex_init(&semaforoMarta,NULL);

	Job *job = levantarArchivoConfiguracion(argv[1]);
	iniciarYEstablecerRutaArchivoLog(job->path_log);

	logDebug("Comenzando el job...");

	//Conectarse a marta
	SocketConecta *conectarAMarta = newSocketConecta(job->ip_marta, job->puerto_marta);
	conectarAServidor(conectarAMarta);
	if(estaConectado(conectarAMarta))
	{
		//Muestro por pantalla la conexion a marta
		logConexionMarta("Se ha conectado correctamente a Marta con IP: %s y PUERTO: %s \n",job->ip_marta, job->puerto_marta);
	}else {logDebug("No se pudo conectar a Marta");exit(1);}

	enviarMensaje(conectarAMarta,job->combiner,2);/*envio si soporto combiner(string) si o no*/
	logCabeceraEnvia("Soporte de combiner");

	//Envio archivos
	int i=0;
	while(job->archivos[i]!=NULL)
	{
		//Envio la longitud del string del archivo
		int tamanio = strlen(job->archivos[i]);
		enviarMensaje(conectarAMarta,&tamanio,sizeof(int));
		logCabeceraEnvia("Longitud de path de archivo");

		//Envio el archivo actual
		enviarMensaje(conectarAMarta,job->archivos[i],strlen(job->archivos[i]));
		logCabeceraEnvia("Envio de archivo");
		i++;
	}
	//Aviso a marta que termine de mandar los archivos
	int finArch =12;
	enviarMensaje(conectarAMarta,&finArch,sizeof(int));
	enviarMensaje(conectarAMarta,"Fin archivos",12);
	logCabeceraEnvia("Finalizacion de envio de archivos");

	//Recibo un 0 si no termina, recibo 1 cuando es el fin del job (YA NO ES ASI)
	//int fin = 0;
	int cantidadHilos=0;
	t_list *listaHilos = list_create();

	while(1)
	{
		char *rutina=malloc(4);
		//Recibo que rutina se va a ejecutar (map o red) con '\0' al final, si recibo "fin" termino el job
		recibirYChequearConexion(conectarAMarta,rutina,4);
		logCabeceraRecibe("Rutina a ejecutar");

		// SOLO ACA PUEDE TERMINAR EL JOB, RECIBO UNA RUTINA A EJECUTAR QUE SEA "fin"
		if(strcmp(rutina,"fin")==0)
		{
			free(rutina);
			finalizarJob(cantidadHilos,listaHilos,conectarAMarta,job,i);
			return EXIT_SUCCESS;
		}

		//Recibo ip del nodo al que le comunico
		int tamIp=0;
		recibirYChequearConexion(conectarAMarta,&tamIp,sizeof(int));
		char *ipNodo=malloc(tamIp);
		recibirYChequearConexion(conectarAMarta,ipNodo,tamIp);
		logCabeceraRecibe("Ip del nodo");

		//Recibo puerto del nodo
		int tamPuerto=0;
		recibirYChequearConexion(conectarAMarta,&tamPuerto,sizeof(int));
		char *puertoNodo=malloc(tamPuerto);
		recibirYChequearConexion(conectarAMarta,puertoNodo,tamPuerto);
		logCabeceraRecibe("Puerto del nodo");

		//Recibo donde se guarda el resultado de la operacion en espacio temporal
		int tamResul = 0;
		recibirYChequearConexion(conectarAMarta,&tamResul,sizeof(int));
		char *resulOperacion=malloc(tamResul);
		recibirYChequearConexion(conectarAMarta,resulOperacion,tamResul);
		logCabeceraRecibe("Archivo de resultado de la operacion");
			//Segun que rutina sea creo un hilo para enviar la rutina al nodo
			if(strcmp(rutina,"map")==0)
			{
				free(rutina);

				//Recibo bloque e id de la operacion
				int bloque = 0;
				int idMap = 0;
				recibirYChequearConexion(conectarAMarta,&bloque,sizeof(int));
				logCabeceraRecibe("Bloque del nodo");
				recibirYChequearConexion(conectarAMarta,&idMap,sizeof(int));
				logCabeceraRecibe("Id de operacion map");

				//Cargo la estructura hiloM
				t_hiloMapper *hiloM= malloc(sizeof(t_hiloMapper));
				hiloM->ipNodo=strdup(ipNodo);
				hiloM->puertoNodo=strdup(puertoNodo);
				hiloM->mapper=strdup(job->mapper);
				hiloM->conectarAMarta=conectarAMarta;
				hiloM->bloque=bloque;
				hiloM->resulMap=strdup(resulOperacion);
				hiloM->idMap=idMap;
				pthread_t hiloMapper;
				pthread_create(&hiloMapper,NULL,enviarMapperYObtenerResultado,hiloM);
				list_add(listaHilos,&hiloMapper);
				cantidadHilos++;

				free(resulOperacion);

				//Muestro que se creo un hilo
				logCreacionHiloMapper("Se creo un hilo mapper con parametros: Ip_Nodo: %s, Puerto_Nodo: %s, Rutina de map: %s",ipNodo,puertoNodo,job->mapper);

			}else if(strcmp(rutina,"red")==0)
			{
				free(rutina);
				//Recibo los archivos y nodos en los que se ejecuta el reduce
//				int tamArchivosYNodos=0;
//				recibirYChequearConexion(conectarAMarta,&tamArchivosYNodos,sizeof(int));
//				char *archivosYNodos=malloc(tamArchivosYNodos);
//				recibirYChequearConexion(conectarAMarta,archivosYNodos,tamArchivosYNodos);
				char *archivosYNodos;
				recibirTodo(conectarAMarta, (void**)&archivosYNodos);
				logCabeceraRecibe("Archivos y nodos");
				int idReduce=0;
				recibirYChequearConexion(conectarAMarta,&idReduce,sizeof(int));
				logCabeceraRecibe("Id de operacion reduce");

				//Cargo la estructura hiloR
				t_hiloReducer *hiloR= malloc(sizeof(t_hiloReducer));

				//Recibo si soy el ultimo reduce o no (Si: 1, No: 0)
				int soyUltimo = 0;
				recibirYChequearConexion(conectarAMarta,&soyUltimo,sizeof(int));
				//Si soy el ultimo cargo en la estructura del hilo el path del resultado en FS
				if(soyUltimo)
				{
					hiloR->resultado=strdup(job->resultado);
				}
				hiloR->soyUltimo = soyUltimo;
				hiloR->ipNodo=strdup(ipNodo);
				hiloR->puertoNodo=strdup(puertoNodo);
				hiloR->reducer=strdup(job->reduce);
				hiloR->conectarAMarta=conectarAMarta;
				hiloR->archivosYNodos=strdup(archivosYNodos);
				hiloR->idReduce=idReduce;
				hiloR->resulReduce=strdup(resulOperacion);

				pthread_t hiloReducer;
				pthread_create(&hiloReducer,NULL,enviarReducerYObtenerResultado,hiloR);
				list_add(listaHilos,&hiloReducer);
				cantidadHilos++;

				free(archivosYNodos);
				free(resulOperacion);

				//Muestro que se creo un hilo
				logCreacionHiloReducer("Se creo un hilo reducer con parametros: Ip_Nodo: %s, Puerto_Nodo: %s, Rutina de reduce: %s, Archivo de resultado: %s",ipNodo,puertoNodo,job->reduce,job->resultado);
			}
			//recibirYChequearConexion(conectarAMarta,&fin,sizeof(int));
			//logCabeceraRecibe("Termino el job?");
			//Libero ip y puerto del nodo
			free(ipNodo);
			free(puertoNodo);
	}
	return EXIT_SUCCESS;
}

void recibirYChequearConexion(SocketConecta *socket,void * mensaje, int tam)
{

	recibirMensaje(socket,mensaje,tam);
	if(!estaConectado(socket))
	{
		fprintf(stderr,"ERROR: Se desconecto marta \n");
		exit(1);
	}

}

void finalizarJob(int cantidadHilos,t_list *listaHilos,SocketConecta *conectarAMarta,Job *job,int i)
{
		//Espero a que todos los hilos hayan terminado
		while(cantidadHilos > 0)
		{
			pthread_t thread = *((pthread_t *) list_get(listaHilos,cantidadHilos - 1));
			pthread_join(thread,NULL);
			cantidadHilos--;
		}
		list_destroy(listaHilos);

		//Borro el socket y muestro que se desconecto
		deleteSocketConecta(conectarAMarta);

		logConexionMarta("Se ha desconectado correctamente de Marta con IP: %s y PUERTO: %s \n",job->ip_marta, job->puerto_marta);

		//Libero la estructura del job
		free(job->mapper);
		free(job->reduce);
		free(job->ip_marta);
		free(job->puerto_marta);
		free(job->combiner);
		//Hago free de archivos (VER QUE NO TIRE SEGMENTATION FAULT)
		/*while(job->archivos[i]!=NULL)
		{
			free(job->archivos[i]);
			i++;
		}*/
		free(job->resultado);
		free(job->path_log);
		free(job);

		//Borro semaforo de logs
		borrarSemaforoLogs();
		pthread_mutex_destroy(&semaforoMarta);

		logConMensaje("FINALIZO EL JOB");

}


