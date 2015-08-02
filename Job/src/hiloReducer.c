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
#include "almacenarResultadoUltimoNodo.h"
#include "hiloReducer.h"
#include "hiloMapper.h"
#include "Job.h"

static void liberarEstructuraHiloReduce(t_hiloReducer *hiloR, int soyUltimo);
//static void mostrarFinalizacion(int resultado);
static void recibirYChequearConexionNodo(SocketConecta *socket,void * mensaje, int tam,SocketConecta *marta,int idOp);


void *enviarReducerYObtenerResultado(void *arghiloR)
{
	t_hiloReducer *hiloR = arghiloR;
	logOperacion("Comienza hilo reducer, ID_TAREA: %d...",hiloR->idReduce);
	SocketConecta *conectarANodo = newSocketConecta(hiloR->ipNodo,hiloR->puertoNodo);
	conectarAServidor(conectarANodo);

	if(!estaConectado(conectarANodo))
		{
			//Le aviso a marta que la rutina fallo porque se desconecto el nodo
			pthread_mutex_lock(&semaforoMarta);
			int dos=2;
			enviarMensaje(hiloR->conectarAMarta,&dos,sizeof(int));
			enviarMensaje(hiloR->conectarAMarta,&hiloR->idReduce,sizeof(int));
			pthread_mutex_unlock(&semaforoMarta);
			logCabeceraEnvia("Fallo reduce, se desconecto el nodo");
			logOperacion("ERROR: Fallo rutina de reduce (ID_TAREA: %d), finalizo el job",hiloR->idReduce);
			exit(1);
		}

	if(!estaConectado(hiloR->conectarAMarta))
	{
		logOperacion("Finalizo el hilo reducer (ID_TAREA: %d) porque se desconecto marta",hiloR->idReduce);
		//El hilo se finaliza a si mismo
		pthread_exit(0);
	}else
	{
		//Aviso al nodo que soy un job (si es el ultimo reduce le mando una U)
		if(!hiloR->soyUltimo)
		{
			char soyJob = 'J';
			enviarMensaje(conectarANodo,&soyJob,1);
		}else
		{
			char soyJob = 'U';
			enviarMensaje(conectarANodo,&soyJob,1);
		}
		logCabeceraEnvia("Soy job");

		//Mando el contenido de la rutina al Nodo
		enviarArchivoANodo(conectarANodo,hiloR->reducer);

		//Le aviso que es un reduce
		int tamanioReduce = 6;
		enviarMensaje(conectarANodo,&tamanioReduce, sizeof(int));
		enviarMensaje(conectarANodo,"reduce",6);
		logCabeceraEnvia("Hacer un reduce");

		//Le aviso al nodo el TAMAÑO de la lista de archivos y nodos y se la mando
//		int tamArch = strlen(hiloR->archivosYNodos);
//		enviarMensaje(conectarANodo,&tamArch,sizeof(int));
//		enviarMensaje(conectarANodo,hiloR->archivosYNodos,strlen(hiloR->archivosYNodos));
		enviarTodo(conectarANodo, hiloR->archivosYNodos, strlen(hiloR->archivosYNodos) + 1);
		logCabeceraEnvia("Archivos y nodos");

		//Envio al nodo donde se guarda el resultado del reduce en espacio temporal
		int tamanio = strlen(hiloR->resulReduce);
		enviarMensaje(conectarANodo,&tamanio,sizeof(int));
		enviarMensaje(conectarANodo,hiloR->resulReduce,tamanio);
		logCabeceraEnvia("Nombre de archivo de resultado");

		//Recibo el resultado de la operacion
		int resultado=0;
		/*0:Salio bien
		 *1:salio mal, fallo la rutina
		 *2:salio mal, se desconecto el nodo
		 */
		recibirYChequearConexionNodo(conectarANodo,&resultado,sizeof(int),hiloR->conectarAMarta,hiloR->idReduce);
		logCabeceraRecibe("Resultado de reduce");

		if(resultado==1)
		{
			logOperacion("ERROR: Fallo rutina de reduce (ID_TAREA: %d), finalizo el job",hiloR->idReduce);
			exit(1);
		}

		//Envio el resultado a Marta (Si se desconecto el nodo tambien mando mensaje aparte)
		pthread_mutex_lock(&semaforoMarta);
		enviarMensaje(hiloR->conectarAMarta,&resultado,sizeof(int));
		logCabeceraEnvia("Resultado de reduce");
		enviarMensaje(hiloR->conectarAMarta,&hiloR->idReduce,sizeof(int));
		logCabeceraEnvia("Id de reduce");
		pthread_mutex_unlock(&semaforoMarta);

		//mostrarFinalizacion(resultado);
		logOperacion("Finalizo correctamente el hilo reducer, ID_TAREA: %d", hiloR->idReduce);


		if(hiloR->soyUltimo && resultado==0)
		{
			almacenarResultadoUltimoNodo(hiloR->conectarAMarta,conectarANodo,hiloR->resultado,hiloR->idReduce);
		}

		//Borro el socket del nodo y el de marta
		deleteSocketConecta(conectarANodo);

		//Libero la estructura del hilo
		liberarEstructuraHiloReduce(hiloR, hiloR->soyUltimo);
	}
	return 0;
}

static void liberarEstructuraHiloReduce(t_hiloReducer *hiloR, int soyUltimo)
{
	free(hiloR->ipNodo);
	free(hiloR->puertoNodo);
	free(hiloR->reducer);
	free(hiloR->archivosYNodos);
	free(hiloR->resulReduce);

	if(soyUltimo)
	{
	free(hiloR->resultado);
	}
	free(hiloR);
}

/*static void mostrarFinalizacion(int resultado)
{
	switch(resultado)
	{
	case 0:
		logConMensaje("Finalizo correctamente el hilo reducer");
		break;
	case 1:
		logConMensaje("Finalizo el hilo reducer porque fallo la rutina");
		break;
	case 2:
		logConMensaje("Finalizo el hilo reducer porque se desconecto uno de los nodos a los que consultaba el nodo principal");
		break;
	}
}*/

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
			logCabeceraEnvia("Id de reduce");
			pthread_mutex_unlock(&semaforoMarta);
			logCabeceraEnvia("Fallo reduce, se desconecto el nodo");
			logOperacion("ERROR: Fallo rutina de reduce (ID_TAREA: %d), finalizo el job",idOp);
			exit(1);
		}
}
