#include "nodo.h"
#include <stdlib.h>
#include <string.h>
#include "estadoFileSystem.h"
#include <stdio.h>
#include <commons/log.h>
#include "logueo.h"
#include "asignarBloque.h"
#include <pthread.h>

t_nodo *newNodo(char *ip, char *puerto, char *nombre, int bloquesTotales, SocketConecta *conexion)
{

	t_nodo *nodo = malloc(sizeof(t_nodo));

	nodo->ip = strdup(ip);
	nodo->estado = 0;
	nodo->puerto = strdup(puerto);
	nodo->nombre = strdup(nombre);
	nodo->bloquesDisponibles = bloquesTotales;
	nodo->bloquesUsados = 0;
	nodo->conexionNodoServidor = conexion;
	nodo->bloques = inicilizarArrayBloques(bloquesTotales);
	pthread_mutex_init(&(nodo->mutex_conexionNodo),NULL);
	return nodo;
}

t_nodo *newNodoPersistido(char *nombreNodo,int bloquesTotales,int bloquesUsados)
{
	t_nodo *nodo = malloc(sizeof(t_nodo));

	nodo->ip = strdup("0");
	nodo->puerto= strdup("0");
	nodo->nombre = strdup(nombreNodo);
	nodo->estado = -1;
	nodo->bloquesDisponibles = bloquesTotales;
	nodo->bloquesUsados = bloquesUsados;
	nodo->conexionNodoServidor = NULL;
	nodo->bloques = inicilizarArrayBloques(bloquesTotales);
	pthread_mutex_init(&(nodo->mutex_conexionNodo),NULL);
	return nodo;
}

int *inicilizarArrayBloques(int bloquesTotales)
{
	int *array = malloc(sizeof(int)*bloquesTotales);
	ponerArrayBloquesEn0(array,bloquesTotales);
	return array;
}

void ponerArrayBloquesEn0(int *array,int bloquesTotales)
{
	int i;
	for(i=0; i < bloquesTotales; i++)
		array[i] = 0;
}

void habilitarNodo(t_nodo *nodo)
{
	loguear(LOG_LEVEL_INFO, "El Nodo %s cambio de estado %d a 1\n", nodo->nombre,nodo->estado);
	nodo->estado = 1;
	mostrarEstadoDelFSEnBytesSoloLog();

}

void deshabilitarNodo(t_nodo *nodo)
{
	reservarListaNodosConectados();
	loguear(LOG_LEVEL_INFO, "El Nodo %s cambio de estado %d a 0\n", nodo->nombre,nodo->estado);
	nodo->estado = 0;
	devolverListaNodosConectados();
	mostrarEstadoDelFSEnBytesSoloLog();
}

void deshabilitarNodoPorDesconexion(t_nodo *nodo)
{
	reservarListaNodosConectados();
	loguear(LOG_LEVEL_INFO, "El Nodo %s cambio de estado %d a -1\n", nodo->nombre,nodo->estado);
	nodo->estado = -1;
	devolverListaNodosConectados();
	mostrarEstadoDelFSEnBytesSoloLog();
}


void incrementarBloquesUsados(t_nodo *nodo)
{
	nodo->bloquesUsados++;
}


int tieneNombre(t_nodo *nodo,char *nombre)
{
	return strcmp(nodo->nombre,nombre)==0;
}

void liberarNodo(t_nodo *nodo)
{
	free(nodo->ip);
	free(nodo->puerto);
	free(nodo->nombre);
	free(nodo->bloques);
	deleteSocketConecta(nodo->conexionNodoServidor);
	free(nodo);

}

int estaHabilitado(t_nodo *nodo)
{
	return nodo->estado == 1;

}

int estaHabilitadoNodo(char *nombreNodo, t_list *listaNodosConectados)
{
	int contador = 0;
	t_nodo *nodo;
	reservarListaNodosConectados();
	while(contador < list_size(listaNodosConectados))
	{
		nodo = list_get(listaNodosConectados,contador);
		if(tieneNombre(nodo,nombreNodo))
		{
			if(nodo->estado == 1)
			{
				devolverListaNodosConectados();
				return 1;
			}
		}

		contador++;
	}
	devolverListaNodosConectados();
	return 0;
}



t_nodo *buscarNodoHabilitado(char *nombreNodo)
{

	int contador = 0;
	t_nodo *nodo;

	reservarListaNodosConectados();
	while(contador < list_size(listaNodosConectados))
	{

		nodo = list_get(listaNodosConectados,contador);
		if(tieneNombre(nodo,nombreNodo))
		{
			if(nodo->estado == 1)
			{
				devolverListaNodosConectados();
				return nodo;

			}
		}
		contador++;
	}
	devolverListaNodosConectados();
	return NULL;
}

void devolverConexionNodo(t_nodo *nodo)
{
	pthread_mutex_unlock(&(nodo->mutex_conexionNodo));

}

void reservarConexionNodo(t_nodo *nodo)
{
	pthread_mutex_lock(&(nodo->mutex_conexionNodo));

}
