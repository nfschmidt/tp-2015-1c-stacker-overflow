#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <libsocket.h>
#include <string.h>
#include "logs.h"
#include "Job.h"

void almacenarResultadoUltimoNodo(SocketConecta *conectarAMarta,SocketConecta *socketNodo, char *resultadoFS,int idOp)
{
	//Envio al nodo la ruta donde se almacena el resultado definitivo (estaba en mi configuracion)
	int tamResultadoFS = strlen(resultadoFS);
	enviarMensaje(socketNodo,&tamResultadoFS,sizeof(int));
	enviarMensaje(socketNodo,resultadoFS,tamResultadoFS);


	/* SE ALMACENO CORRECTAMENTE EL RESULTADO (RECIBO 1) Y MANDO A MARTA
	 * SE LLENA EL FILESYSTEM (LE AVISO A MARTA Y TERMINO) (RECIBO 2) Y MANDO A MARTA
	 * EL NODO SE DESCONECTA (MANDO A MARTA UN 0)
	 */

	int finalizacion = 0;
	recibirMensaje(socketNodo,&finalizacion,sizeof(int));
	if(!estaConectado(socketNodo))
		{
			int cero = 0;
			logConMensaje("Se desconecto el nodo");
			pthread_mutex_lock(&semaforoMarta);
			enviarMensaje(conectarAMarta,&cero,sizeof(int));
			enviarMensaje(conectarAMarta,&idOp,sizeof(int));
			logCabeceraEnvia("Id de reduce");
			pthread_mutex_unlock(&semaforoMarta);
		}else
		{
			if(finalizacion==1)
				{
					logConMensaje("El resultado del job fue almacenado correctamente");
					pthread_mutex_lock(&semaforoMarta);
					enviarMensaje(conectarAMarta,&finalizacion,sizeof(int));
					enviarMensaje(conectarAMarta,&idOp,sizeof(int));
					logCabeceraEnvia("Id de reduce");
					pthread_mutex_unlock(&semaforoMarta);
				}else
				{
					logConMensaje("Se lleno el filesystem");
					pthread_mutex_lock(&semaforoMarta);
					enviarMensaje(conectarAMarta,&finalizacion,sizeof(int));
					enviarMensaje(conectarAMarta,&idOp,sizeof(int));
					logCabeceraEnvia("Id de reduce");
					pthread_mutex_unlock(&semaforoMarta);
				}
		}
}
