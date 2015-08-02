/*
 * estadoFileSystem.c
 *
 *  Created on: 6/5/2015
 *      Author: utnso
 */
#include "estadoFileSystem.h"
#include "configFileSystem.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "nodo.h"
#include <commons/config.h>
#include "libsocket.h"
#include "asignarBloque.h"
#include <pthread.h>
#include "logueo.h"

t_list *listaNodosConectados;

static t_fileSystem *fileSystem;
static int estadoOperativo;
static pthread_mutex_t mutex_listaNodosConectados;

static t_nodo *obtenerNodoConectado(char *nombre);
static int actualizarEstadoFileSystem();
static int cantidadNodosHabilitados();


//Crea las listas necesarias
void crearListas()
{
	pthread_mutex_init(&mutex_listaNodosConectados,NULL);
	listaNodosConectados = list_create();

}
//inicializa el FS creando las listas necesarias
void inicializarFileSystem(t_fileSystem *FS)
{
	crearListas();
	estadoOperativo = 0;
	fileSystem = FS;
}


//agrega nodos a la lista de nodos conectados
t_nodo *agregarNodoConectado(char *ip, char *puerto, char *nombre, int bloquesDisponibles,SocketConecta *conexion)
{
	t_nodo *nodo = newNodo(ip,puerto,nombre,bloquesDisponibles,conexion);
	reservarListaNodosConectados();
	list_add(listaNodosConectados,nodo);
	devolverListaNodosConectados();
	loguear(LOG_LEVEL_INFO,"Se conecto el nodo %s\n",nombre);
	mostrarEstadoDelFSEnBytesSoloLog();
	return nodo;

}

int habilitarNodoConectado(char *nombre)
{
	int exito = 0;
	t_nodo *nodo = obtenerNodoConectado(nombre);

	if(nodo == NULL)
		return exito;

	habilitarNodo(nodo);
	exito = 1;
	actualizarEstadoFileSystem();
	return exito;
}

static t_nodo *obtenerNodoConectado(char *nombre)
{
	t_nodo *nodo;
	int indiceNodo = posicionElemento(nombre);
	if(indiceNodo != -1)
		{
			nodo = list_get(listaNodosConectados,indiceNodo);
			if(nodo->estado == 0 || nodo->estado == 1)
			{
				return nodo;
			}
		}
	return NULL;

}



static int actualizarEstadoFileSystem()
{
	int cantidadMinimaNodos;
	int totalNodosHabilitados;
	totalNodosHabilitados = cantidadNodosHabilitados();
	cantidadMinimaNodos = fileSystem->cantidadNodosConfiguracion;
	if(totalNodosHabilitados >= cantidadMinimaNodos)
		estadoOperativo = 1;

	return estadoOperativo;

}

static int cantidadNodosHabilitados()
{
	reservarListaNodosConectados();
	int contadorNodos = 0;
	int indice = 0;
	while(indice < list_size(listaNodosConectados))
	{
		t_nodo *nodo = list_get(listaNodosConectados,indice);
		if(estaHabilitado(nodo))
			contadorNodos++;

		indice++;
	}
	devolverListaNodosConectados();
	return contadorNodos;
}

//devuelve el estado operativo del FS
int esEstadoOperativo()
{
	return estadoOperativo;
}



//si el nodo estaba conectado lo desconecta
void desconectarNodo(char *nombre)
{
	t_nodo *nodo = obtenerNodoConectado(nombre);
	reservarListaNodosConectados();
	nodo->estado = -1;
	devolverListaNodosConectados();
	actualizarEstadoFileSystem();


}

int posicionElemento(char *nombre)
{
	int contadorListaNodos = 0;

	while(contadorListaNodos < list_size(listaNodosConectados)){

		t_nodo *nodo = list_get(listaNodosConectados,contadorListaNodos);
		if(tieneNombre(nodo,nombre))
			return contadorListaNodos;

		contadorListaNodos++;
	}
	return -1;
}

int deshabilitarNodoConectado(char *nombre)
{
	int exito = 0;
	t_nodo *nodo = obtenerNodoConectado(nombre);

	if(nodo == NULL)
		return exito;

	deshabilitarNodo(nodo);
	exito = 1;
	actualizarEstadoFileSystem();

	return exito;
}

int actualizarNodoReconectado(char *nombre, SocketConecta *conexion, char *puerto)
{
	t_nodo *nodo = obtenerNodoConectado(nombre);

	if(nodo->estado == -1)
	{
		nodo->conexionNodoServidor = conexion;
		nodo->estado = 0;
		nodo->ip = obtenerIPRemoto(conexion);
		nodo->puerto = puerto;

		return 1;
	}
	return 0;

}


void devolverListaNodosConectados()
{
	pthread_mutex_unlock(&mutex_listaNodosConectados);

}

void reservarListaNodosConectados()
{
	pthread_mutex_lock(&mutex_listaNodosConectados);

}

