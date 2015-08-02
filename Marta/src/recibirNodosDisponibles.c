#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <libsocket.h>
#include <commons/collections/list.h>
#include <string.h>
#include "logueo.h"
#include "archivosConfiguracion.h"
#include "configMarta.h"
/*
extern SocketConecta  *conectarAFS;

t_list *recibirNodosDisponibles()
{
int cantidadNodos;
t_list *listaNodosDisponiblestmp=list_create();

recibirMensaje(conectarAFS,&cantidadNodos,sizeof(int));
int i;
	for (i=0; i < cantidadNodos;i++)
	{


  t_infoNodoDisponible  *infoNodDisponible = malloc(sizeof(t_infoNodoDisponible));
	//Recibo Nombre del nodo
	int tamanio_nombre;

		recibirMensaje(conectarAFS,&tamanio_nombre,sizeof(int));

		infoNodDisponible->nombre = malloc(sizeof(char)+1);

		recibirMensaje(conectarAFS,infoNodDisponible->nombre,tamanio_nombre);
		infoNodDisponible->nombre[tamanio_nombre]='\0';

	int tamanio_ipNodo;
		recibirMensaje(conectarAFS,&tamanio_ipNodo,sizeof(int));

		infoNodDisponible->ip = malloc(sizeof(char)+1);

		recibirMensaje(conectarAFS,infoNodDisponible->ip,tamanio_ipNodo);
		infoNodDisponible->ip[tamanio_ipNodo]='\0';

	int tamanio_puerto;
		recibirMensaje(conectarAFS,&tamanio_puerto,sizeof(int));

		infoNodDisponible->puerto = malloc(sizeof(char)+1);

		recibirMensaje(conectarAFS,infoNodDisponible->puerto,tamanio_puerto);
		infoNodDisponible->puerto[tamanio_puerto]='\0';

		recibirMensaje(conectarAFS,&(infoNodDisponible->status),sizeof(int));

		infoNodDisponible->peso=0;

		list_add(listaNodosDisponiblestmp,infoNodDisponible);
	}
	return listaNodosDisponiblestmp;
}
*/
