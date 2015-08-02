#include  <stdlib.h>
#include  <string.h>
#include  <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include "nodo.h"
#include <semaphore.h>
#include "asignarBloque.h"
#include "responderANodo.h"
#include "libsocket.h"
#include "estructuraFS.h"
#include "estadoFileSystem.h"
#include "logueo.h"

#define TAMANIO_MAXIMO 20971520

static t_list *removerNodosDesahabilitadosYSinEspacio(t_list *listaNodosConectados);
static void espacioDeFSEnBytes(off_t *espacioTotal,off_t *espacioUsado);

bool compararCantidadBloquesNodo(void *nodo1arg, void *nodo2arg)
{
	t_nodo *nodo1 = (t_nodo*) nodo1arg;
	t_nodo *nodo2 = (t_nodo*) nodo2arg;


	return nodo1->bloquesUsados >= nodo2->bloquesUsados;
}


t_nodo *seleccionarNodoMenosCargado(t_list *listaNodosConectados, int posicion)
{

	t_nodo *nodoDevolver;
	reservarListaNodosConectados();
	list_sort(listaNodosConectados,compararCantidadBloquesNodo);
	devolverListaNodosConectados();
	t_list *listaAux = removerNodosDesahabilitadosYSinEspacio(listaNodosConectados);
	if(posicion > 0 && posicion <= list_size(listaAux))
	{
		nodoDevolver = (t_nodo*)list_get(listaAux,(list_size(listaAux) - posicion));
		list_destroy(listaAux);
		return nodoDevolver;
	}
	else
	{
		list_destroy(listaAux);
		return NULL;
	}
}

static t_list *removerNodosDesahabilitadosYSinEspacio(t_list *listaNodosConectados)
{
	reservarListaNodosConectados();
	t_list *listaAux = list_create();
	t_nodo *nodo;
	int i = 0;
	for (i = 0;i < list_size(listaNodosConectados);i++)
	{
		nodo = (t_nodo*)list_get(listaNodosConectados,i);
		if (nodo->estado == 1 && bloquesDisponibleNodo(nodo))
			list_add(listaAux,nodo);
	}

	devolverListaNodosConectados();
	return listaAux;
}

int posicionLibreEnNodo(int *bloques)
{
	int i;
	for(i=0 ; bloques[i] != '\n'; i++)
	{
		if(bloques[i] == 0)
			return i;
	}
	return -1;
}

int bloquesDisponibleNodo(t_nodo *nodo)
{
	return nodo->bloquesDisponibles - nodo->bloquesUsados;

}

FILE *abrirArchivoParaBloque(char *ruta)
{
	FILE *archivo = fopen(ruta,"r");
	if(archivo != NULL)
		return archivo;

	return 0;

}

void copiarArchivoALinux(char *ruta,char *rutaDestino)
{
	FILE *archivo = fopen(ruta,"r");
	FILE *archivoNuevo = fopen(rutaDestino,"w");
	int caracter;
	if(archivo != NULL)
	{
		caracter = fgetc(archivo);
		while(caracter != EOF)
		{
			fputc(caracter,archivoNuevo);
			caracter = fgetc(archivo);

		}

		fclose(archivo);
		fclose(archivoNuevo);
	}
}


void generarArchivoPesado(char *ruta)
{
	off_t numero = 0;
	char carac;
	FILE *archivo = fopen(ruta,"w");
	if(archivo != NULL)
	{
		while(numero < 41943040)
		{
			if (numero < 20971520)
				carac = 'j';
			else if ( numero < 41943040)
				carac = 'b';
			else
				carac = 'c';
			fputc(carac,archivo);
			numero++;
		}
		printf("Se grabo el archivo\n"); fflush(stdout);

	}
	fclose(archivo);
}


void eliminarLista(t_list *lista)
{
	int contador = 0;

	while(contador < list_size(lista))
	{
		free(list_get(lista,contador));
		contador++;
	}
	list_destroy(lista);
}

void mostrarEstadoDelFSEnBytesSoloLog()
{
	off_t espacioTotal = 0;
	off_t espacioUsado = 0;
	espacioDeFSEnBytes(&espacioTotal,&espacioUsado);
	off_t espacioDisponible = espacioTotal - espacioUsado;
	loguear(LOG_LEVEL_INFO,"Espacio Total del FileSystem: %lu megas\n",espacioTotal);
	loguear(LOG_LEVEL_INFO,"Espacio Usado del FileSystem: %lu megas\n", espacioUsado);
	loguear(LOG_LEVEL_INFO,"Espacio Disponible del FileSystem: %lu megas\n",espacioDisponible);
}

void mostrarEstadoDelFSEnBytesLogYPantalla()
{
	off_t espacioTotal = 0;
	off_t espacioUsado = 0;
	espacioDeFSEnBytes(&espacioTotal,&espacioUsado);
	off_t espacioDisponible = espacioTotal - espacioUsado;
	loguearYMostrarEnPantalla(LOG_LEVEL_INFO,"Espacio Total del FileSystem: %lu megas\n",espacioTotal);
	loguearYMostrarEnPantalla(LOG_LEVEL_INFO,"Espacio Usado del FileSystem: %lu megas\n", espacioUsado);
	loguearYMostrarEnPantalla(LOG_LEVEL_INFO,"Espacio Disponible del FileSystem: %lu megas\n",espacioDisponible);
}

static void espacioDeFSEnBytes(off_t *espacioTotal,off_t *espacioUsado)
{
		int contador = 0;

		off_t memoriaNodoUsado = 0;
		off_t memoriaNodoDisponible = 0;
		t_nodo *nodo;

		reservarListaNodosConectados();
		int cantidadNodosConectados = list_size(listaNodosConectados);
		devolverListaNodosConectados();

		while(contador < cantidadNodosConectados)
		{
			reservarListaNodosConectados();
			nodo = list_get(listaNodosConectados,contador);
			devolverListaNodosConectados();

			if(estaHabilitadoNodo(nodo->nombre,listaNodosConectados))
			{
				memoriaNodoUsado = memoriaNodoUsado +  ((nodo->bloquesUsados) * 20);
				memoriaNodoDisponible = memoriaNodoDisponible + (((nodo->bloquesDisponibles) * 20));

			}
			contador++;
		}
		*espacioUsado = memoriaNodoUsado;
		*espacioTotal = memoriaNodoDisponible;
}

