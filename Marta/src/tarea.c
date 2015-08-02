/*
 * tareas.c
 *
 *  Created on: 14/7/2015
 *      Author: utnso
 */
#include "tarea.h"
#include "archivoRemoto.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int obtenerCantidadArchivosRemotosDeNodo(t_tarea *tarea, t_nodo *nodo);


t_tarea *newTareaMap(char *archivo, int bloque)
{
	t_tarea *tarea = malloc(sizeof(t_tarea));
	tarea->nombreArchivoMDFS = strdup(archivo);
	tarea->numeroBloque = bloque;
	tarea->tipo = TAREA_MAP;
	tarea->estado = TAREA_NO_EJECUTADA;
	tarea->nodo = NULL;
	tarea->id = 0;

	return tarea;
}

t_tarea *newTareaReduce(t_list *archivosRemotos, TipoJob soportaCombiner)
{
	t_tarea *tarea = malloc(sizeof(t_tarea));
	tarea->tipo = TAREA_REDUCE;
	tarea->archivosRemotos = archivosRemotos;
	tarea->soportaCombiner = soportaCombiner;
	tarea->nodo = NULL;
	tarea->estado = TAREA_NO_EJECUTADA;

	return tarea;
}


int tareaEsMap(t_tarea *tarea)
{
	return tarea->tipo == TAREA_MAP;
}


int tareaEsReduce(t_tarea *tarea)
{
	return tarea->tipo == TAREA_REDUCE;
}



int tareaMapObtenerBloque(t_tarea *tarea)
{
	return tarea->numeroBloque;
}


char *tareaObtenerNombreArchivoMDFS(t_tarea *tarea)
{

	return tarea->nombreArchivoMDFS;
}


void tareaMarcarEnEjecucion(t_tarea *tarea, t_nodo *nodo ,char *nombreResultadoTemporal)
{
	tarea->estado = TAREA_EN_EJECUCION;
	tarea->nodo = nodo;
	tarea->nombreResultadoTemporal = strdup(nombreResultadoTemporal);
}

int tareaEstaEnEjecucion(t_tarea *tarea)
{
	return tarea->estado == TAREA_EN_EJECUCION;
}

int tareaEstaNoEjecutada(t_tarea *tarea)
{
	return tarea->estado == TAREA_NO_EJECUTADA;
}

void tareaMarcarNoEjecutada(t_tarea *tarea)
{
	tarea->estado = TAREA_NO_EJECUTADA;
}

void tareaMarcarFinalizada(t_tarea *tarea)
{
	tarea->estado = TAREA_FINALIZADA;
}

int tareaEstaFinalizada(t_tarea *tarea)
{
	return tarea->estado == TAREA_FINALIZADA;
}

t_list *tareaReduceObtenerArchivosRemotos(t_tarea *tarea)
{
	return tarea->archivosRemotos;
}

t_nodo *tareaObtenerNodoQueEjecuto(t_tarea *tarea)
{
	return tarea->nodo;
}

char *tareaObtenerNombreResultadoTemporal(t_tarea *tarea)
{
	return tarea->nombreResultadoTemporal;
}

int tareaReduceEsSinCombiner(t_tarea *tarea)
{
	return tarea->soportaCombiner == SIN_COMBINER;
}

int tareaReduceEsConCombiner(t_tarea *tarea)
{
	return tarea->soportaCombiner == CON_COMBINER;
}

t_nodo *tareaReduceObtenerNodoConMasArchivosRemotos(t_tarea *tarea)
{
	t_nodo *nodoConMasArchivosRemotos = NULL;
	int mayorCantidadArchivosRemotos = 0;

	int contador = 0;
	while(contador < list_size(tarea->archivosRemotos))
	{
		t_nodo *nodo = archivoRemotoObtenerNodo(list_get(tarea->archivosRemotos, contador));
		int cantidadArchivosRemotos = obtenerCantidadArchivosRemotosDeNodo(tarea, nodo);

		if(cantidadArchivosRemotos > mayorCantidadArchivosRemotos)
		{
			nodoConMasArchivosRemotos = nodo;
			mayorCantidadArchivosRemotos = cantidadArchivosRemotos;
		}

		contador ++;
	}

	return nodoConMasArchivosRemotos;
}

static int obtenerCantidadArchivosRemotosDeNodo(t_tarea *tarea, t_nodo *nodo)
{
	int cantidadArchivosRemotos = 0;
	int contador = 0;
	while(contador < list_size(tarea->archivosRemotos))
	{
		t_nodo *nodoDeLista = archivoRemotoObtenerNodo(list_get(tarea->archivosRemotos, contador));
		if(strcmp(nodo->nombre, nodoDeLista->nombre) == 0)
			cantidadArchivosRemotos ++;

		contador ++;
	}

	return cantidadArchivosRemotos;
}
