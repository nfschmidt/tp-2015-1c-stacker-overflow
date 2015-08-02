/*
 * cargaNodos.h
 *
 *  Created on: 14/7/2015
 *      Author: utnso
 */

#ifndef CARGANODOS_H_
#define CARGANODOS_H_
#include "nodo.h"
#include <commons/collections/list.h>
#include "cargaDeRutinas.h"
#include "tarea.h"

typedef struct {
	t_list *listaNodos;
} t_cargaNodos;


t_cargaNodos *newCargaNodos();

void cargaNodosAgregarNodo(t_cargaNodos *cargaNodo,t_nodo *nodo);

void cargaNodosAgregarListaDeNodos(t_cargaNodos *cargaNodo, t_list *listaNodos);

t_list *cargaNodosObtenerListaNodos(t_cargaNodos *cargaNodo);

t_nodo *cargaNodosObtenerNodoDisponibleParaMap(t_cargaNodos *cargaNodos, t_list *listaNodosParaMap);

t_nodo *cargaNodosObtenerNodoDisponibleParaReduce(t_cargaNodos *cargaNodos, t_tarea *tarea);

t_nodo *cargaNodosObtenerNodoPorNombre(t_cargaNodos *cargaNodos, char *nombreNodo);

void cargaNodosDarDeBajaNodo(t_cargaNodos *cargaNodos, char *nombreNodo);



#endif /* CARGANODOS_H_ */
