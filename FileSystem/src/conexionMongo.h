/*
 * conexionMongo.h
 *
 *  Created on: 6/7/2015
 *      Author: utnso
 */

#ifndef CONEXIONMONGO_H_
#define CONEXIONMONGO_H_

#include "estructuraFS.h"

void inicializarMongoParaElementos(t_elementoFS *elemento);
void borrarDatosElementos();
void recuperarDatosPersistidos();
void inicializarDirectorios(t_list *listaElementosMongo);
void limpiarTodoMongo(t_list *listaElementosMongo);
void persistirNodos();
void borrarDatosNodos();
void leerNodosDeMongo();
int hayDatosEnMongo();


typedef struct {
	t_elementoFS *elemento;
	int id;
	int idPadre;
}t_elemento_mongo;



#endif /* CONEXIONMONGO_H_ */
