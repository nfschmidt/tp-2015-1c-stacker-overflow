#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "logs.h"
#include "configJob.h"
#include "archivosConfiguracion.h"
#include <libsocket.h>
#include <commons/collections/list.h>
#include <string.h>
#include "enviarArchivoDeRutina.h"
#include "hiloMapper.h"
#include "Job.h"

static void liberarEstructuraHiloMap(t_hiloMapper *hiloM);
static void mostrarFinalizacion(int resultado,int id);
static void recibirYChequearConexionNodo(SocketConecta *socket,void * mensaje, int tam,SocketConecta *marta,int idOp);


void *enviarMapperYObtenerResultado(void *arghiloM)
{

	t_hiloMapper *hiloM = arghiloM;
	logOperacion("Comienza hilo mapper, ID_TAREA: %d...",hiloM->idMap);

	SocketConecta *conectarANodo = newSocketConecta(hiloM->ipNodo,hiloM->puertoNodo);

	conectarAServidor(conectarANodo);
	if(!estaConectado(conectarANodo))
	{
		//Le aviso a marta que la rutina fallo porque se desconecto el nodo
		pthread_mutex_lock(&semaforoMarta);
		int dos=2;
		enviarMensaje(hiloM->conectarAMarta,&dos,sizeof(int));
		enviarMensaje(hiloM->conectarAMarta,&hiloM->idMap,sizeof(int));
		pthread_mutex_unlock(&semaforoMarta);
		logCabeceraEnvia("Fallo map, se desconecto el nodo");
		mostrarFinalizacion(2,hiloM->idMap);
		pthread_exit(0);
	}

	if(!estaConectado(hiloM->conectarAMarta))
	{
		logOperacion("Finalizo el hilo mapper (ID_TAREA: %d) porque se desconecto marta", hiloM->idMap);
		//El hilo se finaliza a si mismo
		pthread_exit(0);
	}else
	{
		//Aviso al nodo que soy un job
		char soyJob = 'J';
		enviarMensaje(conectarANodo,&soyJob,1);
		logCabeceraEnvia("Soy job");

		//Mando el contenido de la rutina al Nodo
		enviarArchivoANodo(conectarANodo,hiloM->mapper);

		//Le aviso que es un map y le mando el bloque
		int tamanioMap = 4;
		enviarMensaje(conectarANodo,&tamanioMap, sizeof(int));
		enviarMensaje(conectarANodo,"map",tamanioMap);
		logCabeceraEnvia("Hacer un map");
		enviarMensaje(conectarANodo,&(hiloM->bloque),sizeof(int));
		logCabeceraEnvia("Bloque del nodo");

		//Envio al nodo donde se guarda el resultado del map
		int tamanio = strlen(hiloM->resulMap);
		enviarMensaje(conectarANodo,&tamanio,sizeof(int));
		enviarMensaje(conectarANodo,hiloM->resulMap,tamanio);
		logCabeceraEnvia("Nombre de archivo de resultado");

		//Recibo el resultado de la operacion
		int resultado=0;

		/*0:Salio bien
		 *1:salio mal, fallo la rutina
		 *2:se desconecto el nodo, lo mando al principio
		 */
		recibirYChequearConexionNodo(conectarANodo,&resultado,sizeof(int),hiloM->conectarAMarta,hiloM->idMap);
		logCabeceraRecibe("Resultado de map");


		//Envio el resultado a Marta (Si se desconecto el nodo tambien mando mensaje aparte)
		pthread_mutex_lock(&semaforoMarta);
		enviarMensaje(hiloM->conectarAMarta,&resultado,sizeof(int));
		logCabeceraEnvia("Resultado de map");
		enviarMensaje(hiloM->conectarAMarta,&hiloM->idMap,sizeof(int));
		logCabeceraEnvia("Id de map");
		pthread_mutex_unlock(&semaforoMarta);

		mostrarFinalizacion(resultado,hiloM->idMap);

		//Borro el socket y el semaforo del nodo
		deleteSocketConecta(conectarANodo);

		//Libero la estructura del hilo
		liberarEstructuraHiloMap(hiloM);
	}
	return 0;
}

static void liberarEstructuraHiloMap(t_hiloMapper *hiloM)
{
	free(hiloM->ipNodo);
	free(hiloM->puertoNodo);
	free(hiloM->mapper);
	free(hiloM->resulMap);
	free(hiloM);
}

static void mostrarFinalizacion(int resultado,int id)
{
	switch(resultado)
	{
	case 0:
		logOperacion("Finalizo correctamente el hilo mapper, ID_TAREA: %d", id);
		break;
	case 1:
		logOperacion("Finalizo el hilo mapper (ID_TAREA: %d) porque fallo la rutina",id);
		break;
	}
}

static void recibirYChequearConexionNodo(SocketConecta *socket,void * mensaje, int tam,SocketConecta *marta,int idOp)
{
		recibirMensaje(socket,mensaje,tam);
		if(!estaConectado(socket))
		{
			//Le aviso a marta que la rutina fallo porque se desconecto el nodo
			pthread_mutex_lock(&semaforoMarta);
			int dos=2;
			enviarMensaje(marta,&dos,sizeof(int));
			enviarMensaje(marta,&idOp,sizeof(int));
			logCabeceraEnvia("Id de map");
			pthread_mutex_unlock(&semaforoMarta);
			logCabeceraEnvia("Fallo map, se desconecto el nodo");
			mostrarFinalizacion(1,idOp);
			pthread_exit(0);
		}
}
