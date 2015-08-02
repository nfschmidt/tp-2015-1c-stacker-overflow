/*
 * conexionMongo.c
 *
 *  Created on: 6/7/2015
 *      Author: utnso
 */
#include "conexionMongo.h"
#include <mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include "estructuraFS.h"
#include <bson.h>
#include <commons/collections/list.h>
#include "logueo.h"
#include "estadoFileSystem.h"
#include "nodo.h"




static int agregarElementoAMongo(t_elementoFS *elemento, int idPadre, int idPropio, mongoc_collection_t *collection);
static void agregarCopia(t_bloque *bloque,int idPropio);
static void agregarNodosAColeccion(mongoc_collection_t *colleccionNodo);
static void agregarArrayDeBloquesNodo(char *nodoNombre,int *arrayBloques, int bloquesDisponibles);


////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////Persistencia Nodos////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


void persistirNodos()
{
	/* INICIALIZO */
	mongoc_init();

	/* ME CONECTO */
	mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017/");

	/* DIGO QUE BASE Y SOBRE QUE COLECCION VOY A TRABAJAR */
	mongoc_collection_t *coleccionNodo = mongoc_client_get_collection(client,"MDFS","nodos");


	agregarNodosAColeccion(coleccionNodo);
	mongoc_collection_destroy(coleccionNodo);
	mongoc_client_destroy(client);
}

static void agregarNodosAColeccion(mongoc_collection_t *coleccionNodo)
{

	int contador = 0;
	reservarListaNodosConectados();
	while(contador < list_size(listaNodosConectados))
	{
		t_nodo *nodo = list_get(listaNodosConectados,contador);

		bson_t *documento = BCON_NEW(
					"_id", BCON_UTF8(nodo->nombre),
					"bloquesDisponibles", BCON_INT32(nodo->bloquesDisponibles),
					"bloquesUsados", BCON_INT32(nodo->bloquesUsados));


		/* ACA ESTOY AGREGANDO UNA LISTA VACIA DEL ARRAY DE BLOQUES */
		bson_t arrayBloques;
		bson_append_array_begin(documento, "arrayBloques", strlen("arrayBloques"), &arrayBloques);
		bson_append_array_end(documento, &arrayBloques);


		/* ACA INSERTO EL DOCUMENTO EN LA BASE */
		bson_error_t error;
		mongoc_collection_insert(coleccionNodo, MONGOC_INSERT_NONE, documento, NULL, &error);

		bson_destroy(documento);

		agregarArrayDeBloquesNodo(nodo->nombre,nodo->bloques,nodo->bloquesDisponibles);

		contador++;
	}
	devolverListaNodosConectados();

}

static void agregarArrayDeBloquesNodo(char *nodoNombre,int *arrayBloques,int bloquesDisponibles)
{
	int contador = 0;
	mongoc_init();
	mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017/");
	mongoc_collection_t *collection = mongoc_client_get_collection(client, "MDFS", "nodos");

	bson_t *query = BCON_NEW ("_id", BCON_UTF8(nodoNombre));

	while(contador < bloquesDisponibles)
	{
		// CREO EL UPDATE QUE VOY A REALIZAR
		bson_t *update = BCON_NEW ("$push", "{",  // "$push" se usa para insertar en una lista
									"arrayBloques", "{",  // Aca agrego
												"_id", BCON_INT32(arrayBloques[contador]),
												"}",
												"}" );
		// REALIZO EL UPDATE
		bson_error_t error;
		mongoc_collection_update(collection, MONGOC_UPDATE_NONE, query, update, NULL, &error);
		bson_destroy(update);

		contador++;
	}
	mongoc_collection_destroy(collection);
	mongoc_client_destroy(client);
	bson_destroy(query);


}


void leerNodosDeMongo()
{
	mongoc_init();
	mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017/");
	mongoc_collection_t *coleccionNodo = mongoc_client_get_collection(client, "MDFS", "nodos");
	bson_t *queryTodosLosElementos = bson_new(); // como no especifico nada, me va a devolver todos los registros
	mongoc_cursor_t *cursor = mongoc_collection_find (coleccionNodo, MONGOC_QUERY_NONE, 0, 0, 0, queryTodosLosElementos, NULL, NULL);
	t_nodo *nodoPersistido;


	const bson_t *nodo;
	while (mongoc_cursor_next(cursor, &nodo))
	{
		int contadorBloques = 0;
		bson_iter_t iterador;

		int bloquesUsados;
		int bloquesDisponibles;

		bson_iter_init(&iterador, nodo);
		bson_iter_find(&iterador, "_id");
		char *nombre = strdup(bson_iter_value(&iterador)->value.v_utf8.str);
		loguear(LOG_LEVEL_DEBUG,"Nombre Nodo %s",nombre);

		bson_iter_init(&iterador, nodo);
		bson_iter_find(&iterador, "bloquesDisponibles");
		bloquesDisponibles = bson_iter_value(&iterador)->value.v_int64;
		loguear(LOG_LEVEL_DEBUG,"Cantidad Bytes Bloques %lu", bloquesDisponibles);

		bson_iter_init(&iterador, nodo);
		bson_iter_find(&iterador, "bloquesUsados");
		bloquesUsados = bson_iter_value(&iterador)->value.v_int32;
		loguear(LOG_LEVEL_DEBUG,"Numero de Bloque en Nodo %d",bloquesUsados);

		nodoPersistido = newNodoPersistido(nombre,bloquesDisponibles,bloquesUsados);

		bson_iter_init(&iterador, nodo);
		bson_iter_find(&iterador, "arrayBloques");
		bson_iter_t arrayBloques;

		bson_iter_recurse(&iterador, &arrayBloques);
		while (bson_iter_next(&arrayBloques))
		{
			bson_iter_t iteradorBloques;
			bson_iter_recurse(&arrayBloques, &iteradorBloques);
			bson_iter_find(&iteradorBloques, "_id");
			int posicionOcupada = bson_iter_value(&iteradorBloques)->value.v_int32;
			loguear(LOG_LEVEL_DEBUG,"Posicion Ocupada: %d",posicionOcupada);
			nodoPersistido->bloques[contadorBloques] = posicionOcupada;
			contadorBloques++;

		}
		reservarListaNodosConectados();
		list_add(listaNodosConectados,nodoPersistido);
		devolverListaNodosConectados();
	}
}


void borrarDatosNodos()
{
	mongoc_init();
	mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017/");
	mongoc_collection_t *coleccionNodo = mongoc_client_get_collection(client,"MDFS","nodos");

	/* CREO UN QUERY PARA buscar los documentos que quiero eliminar*/
	bson_t *query = bson_new(); // como no especifico nada, me va a devolver todos los registros

	/* REALIZO EL DELETE */
	bson_error_t error;
	mongoc_collection_remove(coleccionNodo,MONGOC_DELETE_NONE,query,NULL,&error);
	bson_destroy(query);
	mongoc_collection_destroy(coleccionNodo);
	mongoc_client_destroy(client);

}





////////////////////////////////////////////////////////////////////////////
/////////////////////////////PERSISTIR ELEMENTOS///////////////////////////
///////////////////////////////////////////////////////////////////////////



void inicializarMongoParaElementos(t_elementoFS *elemento)
{
	/* INICIALIZO */
	mongoc_init();

	/* ME CONECTO */
	mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017/");

	/* DIGO QUE BASE Y SOBRE QUE COLECCION VOY A TRABAJAR */
	mongoc_collection_t *collection = mongoc_client_get_collection(client, "MDFS", "elementosFS");

	agregarElementoAMongo(elemento,-1,0,collection);

	mongoc_collection_destroy(collection);
	mongoc_client_destroy(client);
}


static int agregarElementoAMongo(t_elementoFS *elemento, int idPadre,int idPropio, mongoc_collection_t *collection)
{
	int contadorCopias = 0;
	int id = idPropio;

	/* ACA CARGO LOS DATOS DEL DOCUMENTO QUE VOY A INSERTAR */
	bson_t *documento = BCON_NEW(
			"_id",BCON_INT32(idPropio),
			"nombre", BCON_UTF8(elemento->nombre),  /* Los strings se guardan con UTF8 */
			"tipo", BCON_INT32(elemento->tipo),
			"cantidadBloques",BCON_INT32(elemento->cantidadBloques),
			"idPadre",BCON_INT32(idPadre)
			);

	/* ACA ESTOY AGREGANDO UNA LISTA VACIA DE COPIAS */
	bson_t listaDeCopias;
	bson_append_array_begin(documento, "listaCopias", strlen("listaCopias"), &listaDeCopias);
	bson_append_array_end(documento, &listaDeCopias);


	/* ACA INSERTO EL DOCUMENTO EN LA BASE */
	bson_error_t error;
	mongoc_collection_insert(collection, MONGOC_INSERT_NONE, documento, NULL, &error);

	bson_destroy(documento);

	if(esArchivo(elemento))
	{
		while(contadorCopias < list_size(elemento->listaCopias))
		{
			t_bloque *bloque = list_get(elemento->listaCopias,contadorCopias);
			agregarCopia(bloque,idPropio);
			contadorCopias++;
		}
	}

	if(esDirectorio(elemento))
	{
		int contador = 0;
		while(contador < list_size(elemento->listaDirectorios))
		{
			id++;
			t_elementoFS *elementoHijo = list_get(elemento->listaDirectorios, contador);
			id = agregarElementoAMongo(elementoHijo,idPropio,id,collection);
			contador++;
		}
	}
	return id;
}




void inicializarDirectorios(t_list *listaElementosMongo)
{
	int contadorActual = 0;
	int contadorResto = 0;

	while(contadorActual < list_size(listaElementosMongo))
	{
		t_elemento_mongo *elementoActual = list_get(listaElementosMongo,contadorActual);
		t_elementoFS *elementoFSActual = elementoActual->elemento;
		contadorResto = 0;

		while(contadorResto < list_size(listaElementosMongo))
		{
			t_elemento_mongo *elementoResto = list_get(listaElementosMongo,contadorResto);
			t_elementoFS *elementoFSResto = elementoResto->elemento;
			if(elementoResto->idPadre == elementoActual->id)
			{
				list_add(elementoFSActual->listaDirectorios,elementoFSResto);
			}

			contadorResto++;
		}

		if (strcmp(elementoFSActual->nombre, "/") == 0 )
		{
			root = newElementoFS("/",DIRECTORIO);
			root->listaDirectorios = elementoFSActual->listaDirectorios;
		}
		contadorActual++;
	}
	limpiarTodoMongo(listaElementosMongo);
}

static void agregarCopia(t_bloque *bloque, int idPropio)
{
	mongoc_init();
	mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017/");
	mongoc_collection_t *collection = mongoc_client_get_collection(client, "MDFS", "elementosFS");


	bson_t *query = BCON_NEW ("_id", BCON_INT32(idPropio));

	// CREO EL UPDATE QUE VOY A REALIZAR
    bson_t *update = BCON_NEW ("$push", "{",  // "$push" se usa para insertar en una lista
    								"listaCopias", "{",  // Aca agrego en la lista de autos un nuevo auto
												"_id", BCON_UTF8(bloque->nombreNodo),
												"cantidadBytesBloque", BCON_INT64(bloque->cantidadBytesBloque),
												"numeroDeBloqueEnNodo",BCON_INT32(bloque->numeroDeBloqueEnNodo),
												"numeroDeBloqueEnArchivo",BCON_INT32(bloque->numeroDeBloqueDelArchivo),"}",
							   "}" );

    // REALIZO EL UPDATE
    bson_error_t error;
    mongoc_collection_update (collection, MONGOC_UPDATE_NONE, query, update, NULL, &error);
	bson_destroy(update);

	bson_destroy(query);
	mongoc_collection_destroy(collection);
	mongoc_client_destroy(client);
}


void borrarDatosElementos()
{
	mongoc_init();
	mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017/");
	mongoc_collection_t *collection = mongoc_client_get_collection(client, "MDFS", "elementosFS");

	/* CREO UN QUERY PARA buscar los documentos que quiero eliminar*/
	bson_t *query = bson_new(); // como no especifico nada, me va a devolver todos los registros

	/* REALIZO EL DELETE */
    bson_error_t error;
	mongoc_collection_remove(collection, MONGOC_DELETE_NONE, query, NULL, &error);

	bson_destroy(query);
	mongoc_collection_destroy(collection);

	mongoc_client_destroy(client);
}

void limpiarTodoMongo(t_list *listaElementosMongo)
{
	int contador = 0;

	while(contador < list_size(listaElementosMongo))
	{
		t_elemento_mongo *elementoMongo = list_get(listaElementosMongo,contador);
		list_remove(listaElementosMongo,contador);
		free(elementoMongo);
	}

	list_destroy(listaElementosMongo);


}

void recuperarDatosPersistidos()
{
	t_list *listaElementosMongo = list_create();
	mongoc_init();
	mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017/");
	mongoc_collection_t *collection = mongoc_client_get_collection(client, "MDFS", "elementosFS");
	bson_t *queryTodosLosElementos = bson_new(); // como no especifico nada, me va a devolver todos los registros
	mongoc_cursor_t *cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, queryTodosLosElementos, NULL, NULL);

	const bson_t *elemento;
	while (mongoc_cursor_next(cursor, &elemento))
	{
		t_elemento_mongo *elementoMongo = malloc(sizeof(t_elemento_mongo));
		bson_iter_t iterador;

		bson_iter_init(&iterador, elemento);
		bson_iter_find(&iterador, "nombre");
		char *nombre = bson_iter_value(&iterador)->value.v_utf8.str;
		loguear(LOG_LEVEL_DEBUG,"Nombre: %s",nombre);

		bson_iter_init(&iterador, elemento);
		bson_iter_find(&iterador, "tipo");
		t_elemento tipo = (t_elemento)bson_iter_value(&iterador)->value.v_int32;
		loguear(LOG_LEVEL_DEBUG,"Tipo: %d",tipo);

		bson_iter_init(&iterador, elemento);
		bson_iter_find(&iterador, "cantidadBloques");
		int cantidadBloques = bson_iter_value(&iterador)->value.v_int32;
		loguear(LOG_LEVEL_DEBUG,"CantidadBloques: %d",cantidadBloques);

		bson_iter_init(&iterador, elemento);
		bson_iter_find(&iterador, "idPadre");
		int idPadre = bson_iter_value(&iterador)->value.v_int32;
		loguear(LOG_LEVEL_DEBUG,"id Padre: %d",idPadre);

		bson_iter_init(&iterador, elemento);
		bson_iter_find(&iterador, "_id");
		int id = bson_iter_value(&iterador)->value.v_int32;
		loguear(LOG_LEVEL_DEBUG,"id Propio: %d",id);

		t_elementoFS *elemento = newElementoFS(nombre,tipo);
		elemento->cantidadBloques = cantidadBloques;


		bson_iter_find(&iterador, "listaCopias");
		bson_iter_t iteradorLista;
		bson_iter_recurse(&iterador, &iteradorLista);
		while (bson_iter_next(&iteradorLista))
		{
			bson_iter_t iteradorCopia;

			bson_iter_recurse(&iteradorLista, &iteradorCopia);
			bson_iter_find(&iteradorCopia, "_id");
			char *nombreNodo = strdup(bson_iter_value(&iteradorCopia)->value.v_utf8.str);
			loguear(LOG_LEVEL_DEBUG,"Nombre Nodo: %s",nombreNodo);

			bson_iter_recurse(&iteradorLista, &iteradorCopia);
			bson_iter_find(&iteradorCopia, "numeroDeBloqueEnNodo");
			int numeroDeBloqueEnNodo = bson_iter_value(&iteradorCopia)->value.v_int32;
			loguear(LOG_LEVEL_DEBUG,"Numero de bloque en Nodo: %d",numeroDeBloqueEnNodo);

			bson_iter_recurse(&iteradorLista, &iteradorCopia);
			bson_iter_find(&iteradorCopia, "numeroDeBloqueEnArchivo");
			int numeroDeBloqueEnArchivo = bson_iter_value(&iteradorCopia)->value.v_int32;
			loguear(LOG_LEVEL_DEBUG,"Numero de bloque en Archivo: %d",numeroDeBloqueEnArchivo);

			bson_iter_recurse(&iteradorLista, &iteradorCopia);
			bson_iter_find(&iteradorCopia, "cantidadBytesBloque");
			off_t cantidadBytesBloque = bson_iter_value(&iteradorCopia)->value.v_int64;
			loguear(LOG_LEVEL_DEBUG,"Numero de bytes: %lu",cantidadBytesBloque);


			t_bloque *bloque = newBloque(nombreNodo,numeroDeBloqueEnNodo,numeroDeBloqueEnArchivo,cantidadBytesBloque);
			list_add(elemento->listaCopias,bloque);
		}
		elementoMongo->elemento = elemento;
		elementoMongo->id = id;
		elementoMongo->idPadre = idPadre;
		list_add(listaElementosMongo,elementoMongo);
	}


	inicializarDirectorios(listaElementosMongo);

	bson_destroy(queryTodosLosElementos);
	mongoc_collection_destroy(collection);
	mongoc_client_destroy(client);

}


int hayDatosEnMongo()
{
	bson_error_t error;
	mongoc_init();
	mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017/");
	mongoc_collection_t *collection = mongoc_client_get_collection(client, "MDFS", "elementosFS");
	bson_t *queryTodosLosElementos = bson_new(); // como no especifico nada, me va a devolver todos los registros
	bson_init(queryTodosLosElementos);
	int64_t cantidadElementos = mongoc_collection_count(collection,MONGOC_QUERY_NONE,queryTodosLosElementos,0,0,NULL,&error);

	return cantidadElementos;
}


