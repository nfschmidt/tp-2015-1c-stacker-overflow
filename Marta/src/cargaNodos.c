#include "cargaNodos.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "archivoRemoto.h"

static bool nodoConMenorCargaPrimero(void *argNodo1, void* argNodo2);
static t_nodo *obtenerNodoConNombre(t_cargaNodos *cargaNodos, char *nombreNodo);
static t_list *obtenerNodosQueEstanEnLista(t_cargaNodos *cargaNodos, t_list *listaNodos);
static void agregarNodosQueNoEstanEnListaInterna(t_cargaNodos *cargaNodos, t_list *listaNodos);
static t_list *obtenerListaDeNodosQueTienenResultadosTemporales(t_cargaNodos *cargaNodos, t_tarea *tarea);

t_cargaNodos *newCargaNodos()
{
	t_cargaNodos *cargaNodos = malloc(sizeof(t_cargaNodos));
	cargaNodos->listaNodos = list_create();
	return cargaNodos;

}

void cargaNodosAgregarNodo(t_cargaNodos *cargaNodo,t_nodo *nodo)
{
	int contador = 0;
	while(contador < list_size(cargaNodo->listaNodos))
	{
		t_nodo *nodoEnLista = list_get(cargaNodo->listaNodos, contador);
		if(strcmp(nodoEnLista->nombre, nodo->nombre) == 0)
			return;

		contador++;
	}

	list_add(cargaNodo->listaNodos,nodo);
}

t_list *cargaNodosObtenerListaNodos(t_cargaNodos *cargaNodo)
{
	return cargaNodo->listaNodos;
}

t_nodo *cargaNodosObtenerNodoDisponibleParaReduce(t_cargaNodos *cargaNodos, t_tarea *tarea)
{
	t_nodo *nodoDisponible = NULL;
	if(tareaReduceEsConCombiner(tarea))
	{
		t_list *listaDeNodosQueEjecutaronTareas = obtenerListaDeNodosQueTienenResultadosTemporales(cargaNodos, tarea);

		list_sort(listaDeNodosQueEjecutaronTareas, nodoConMenorCargaPrimero);
		t_nodo *nodoConMenosCarga =  list_get(listaDeNodosQueEjecutaronTareas, 0);
		if(nodoConMenosCarga != NULL && nodoConMenosCarga->carga < getCargaMaxima())
			nodoDisponible = nodoConMenosCarga;
		else
			nodoDisponible = NULL;
	}
	else
	{
		t_nodo *nombreNodoConMasArchivos = tareaReduceObtenerNodoConMasArchivosRemotos(tarea);
		t_nodo * nodoConMasArchivos = obtenerNodoConNombre(cargaNodos, nombreNodoConMasArchivos->nombre);

		if(nodoConMasArchivos != NULL && nodoConMasArchivos->carga < getCargaMaxima())
			nodoDisponible = nodoConMasArchivos;
		else
			nodoDisponible = NULL;
	}

	return nodoDisponible;
}

t_nodo *cargaNodosObtenerNodoDisponibleParaMap(t_cargaNodos *cargaNodos, t_list *listaNodosParaMap)
{
	agregarNodosQueNoEstanEnListaInterna(cargaNodos, listaNodosParaMap);

	t_list *listaDeNodosCandidatos = obtenerNodosQueEstanEnLista(cargaNodos, listaNodosParaMap);
	list_sort(listaDeNodosCandidatos, nodoConMenorCargaPrimero);

	t_nodo *nodo =  list_get(listaDeNodosCandidatos, 0);

	if(nodo->carga + getCargaMap() > getCargaMaxima())
		return NULL;

	int contador = 0;
	t_nodo *nodoAUsar;
	while (contador < list_size(listaNodosParaMap))
	{
		nodoAUsar = list_get(listaNodosParaMap, contador);
		if (strcmp(nodoAUsar->nombre, nodo->nombre) == 0)
			break;

		contador ++;
	}

	nodo->bloque = nodoAUsar->bloque;
	return nodo;
}

static bool nodoConMenorCargaPrimero(void *argNodo1, void* argNodo2)
{
	return ((t_nodo*)argNodo1)->carga <=  ((t_nodo*)argNodo2)->carga;
}

static t_nodo *obtenerNodoConNombre(t_cargaNodos *cargaNodos, char *nombreNodo)
{
	int i;
	for (i = 0; i < list_size(cargaNodos->listaNodos); i ++)
	{
		t_nodo *nodo = list_get(cargaNodos->listaNodos, i);
		if (strcmp(nodo->nombre, nombreNodo) == 0)
			return nodo;
	}

	return NULL;
}

static t_list *obtenerNodosQueEstanEnLista(t_cargaNodos *cargaNodos, t_list *listaNodos)
{
	t_list *resultado = list_create();

	int contador = 0;
	while (contador < list_size(listaNodos))
	{
		t_nodo *nodoEnLista = list_get(listaNodos, contador);
		list_add(resultado, obtenerNodoConNombre(cargaNodos, nodoEnLista->nombre));

		contador ++;
	}

	return resultado;
}

static void agregarNodosQueNoEstanEnListaInterna(t_cargaNodos *cargaNodos, t_list *listaNodos)
{
	int contador = 0;
	while (contador < list_size(listaNodos))
	{
		t_nodo *nodoEnLista = list_get(listaNodos, contador);
		cargaNodosAgregarNodo(cargaNodos, nodoEnLista);

		contador ++;
	}
}

void cargaNodosAgregarListaDeNodos(t_cargaNodos *cargaNodo, t_list *listaNodos)
{
	int contador = 0;
	while (contador < list_size(listaNodos))
	{
		t_nodo *nodo = list_get(listaNodos, contador);

		cargaNodosAgregarNodo(cargaNodo, nodo);
		contador ++;
	}
}

t_nodo *cargaNodosObtenerNodoPorNombre(t_cargaNodos *cargaNodos, char *nombreNodo)
{
	int contador = 0;
	while (contador < list_size(cargaNodos->listaNodos))
	{
		t_nodo *nodo = list_get(cargaNodos->listaNodos, contador);

		if (strcmp(nodo->nombre, nombreNodo) == 0)
			return nodo;

		contador ++;
	}

	return NULL;
}

void cargaNodosDarDeBajaNodo(t_cargaNodos *cargaNodos, char *nombreNodo)
{
	int contador = 0;
	while (contador < list_size(cargaNodos->listaNodos))
	{
		t_nodo *nodo = list_get(cargaNodos->listaNodos, contador);

		if(strcmp(nodo->nombre, nombreNodo) == 0)
		{
			list_remove(cargaNodos->listaNodos, contador);
			break;
		}

		contador ++;
	}
}

static t_list *obtenerListaDeNodosQueTienenResultadosTemporales(t_cargaNodos *cargaNodos, t_tarea *tarea)
{
	t_list *archivosResultadosTemporales = tareaReduceObtenerArchivosRemotos(tarea);
	t_list *resultado = list_create();

	int contador = 0;
	while (contador < list_size(archivosResultadosTemporales))
	{
		t_archivoRemoto *archivoRemoto = list_get(archivosResultadosTemporales, contador);
		t_nodo *nodo = cargaNodosObtenerNodoPorNombre(cargaNodos, archivoRemotoObtenerNodo(archivoRemoto)->nombre);
		if (nodo != NULL)
			list_add(resultado, nodo);

		contador ++;
	}

	return resultado;
}
