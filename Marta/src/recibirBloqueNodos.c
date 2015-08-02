#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <libsocket.h>
#include <commons/collections/list.h>
#include <string.h>
#include "logueo.h"
#include "archivosConfiguracion.h"
#include "configMarta.h"
#include "configMarta.h"

/*
t_list *recibirBloqueNodos(int cantidadBloque)
{
	t_list *listaBloqueNodotmp=list_create();

	int cantidadBloques;
	int tamanio_copiasNodos;

	//recibirMensaje(conectarAFS,&cantidadBloques,sizeof(int));
	int i;
		for (i=0;i<cantidadBloques;i++)
		{
		 recibirMensaje(conectarAFS,&tamanio_copiasNodos,sizeof(int));

		 int j;

		 for (j=0; j < tamanio_copiasNodos;j++)
		 		{

	    t_infoBloqueNodo  *infoBloqueNodo = malloc(sizeof(t_infoBloqueNodo));


		int tamanio_nombre;

			recibirMensaje(conectarAFS,&tamanio_nombre,sizeof(int));

			infoBloqueNodo->nombreNodo = malloc(sizeof(char)+1);

			recibirMensaje(conectarAFS,infoBloqueNodo->nombreNodo,tamanio_nombre);
			infoBloqueNodo->nombreNodo[tamanio_nombre]='\0';

		int tamanio_ipNodo;
			recibirMensaje(conectarAFS,&tamanio_ipNodo,sizeof(int));

			infoBloqueNodo->ip_nodo = malloc(sizeof(char)+1);

			recibirMensaje(conectarAFS,infoBloqueNodo->ip_nodo,tamanio_ipNodo);
			infoBloqueNodo->ip_nodo[tamanio_ipNodo]='\0';

		int tamanio_puerto;
			recibirMensaje(conectarAFS,&tamanio_puerto,sizeof(int));

			infoBloqueNodo->puerto_nodo = malloc(sizeof(char)+1);

			recibirMensaje(conectarAFS,infoBloqueNodo->puerto_nodo,tamanio_puerto);
			infoBloqueNodo->puerto_nodo[tamanio_puerto]='\0';

			recibirMensaje(conectarAFS,&(infoBloqueNodo->nro_bloqueNodo),sizeof(int));

			infoBloqueNodo->nro_bloqueArchivo=i;

			list_add(listaBloqueNodotmp,infoBloqueNodo);
		}
		}
return listaBloqueNodotmp;
}
*/
