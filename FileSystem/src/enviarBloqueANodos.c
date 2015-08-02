/*
 * enviarBloqueANodos.c
 *
 *  Created on: 21/6/2015
 *      Author: utnso
 */

#include "enviarBloqueANodos.h"
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
#include "enviarBloqueANodos.h"
#include "atenderConexiones.h"
#include "estadoFileSystem.h"
#include "logueo.h"

#define CONEXION_CONSOLA_FILESYSTEM "F"

static enviarDatosNodo *obtenerDatosBloqueEnviar(char *bloque,t_elementoFS *archivoNuevo,int contadorBloques, t_nodo *nodo);
static void *enviarCopiaANodo(void *argEnviarDatosNodo);

int enviarBloqueANodos(char *bloque,t_elementoFS *archivoNuevo,int contadorBloques)
{
	enviarDatosNodo *enviarDatos1,*enviarDatos2,*enviarDatos3;
	t_nodo *nodo1 = seleccionarNodoMenosCargado(listaNodosConectados,1);
	t_nodo *nodo2 = seleccionarNodoMenosCargado(listaNodosConectados,2);
	t_nodo *nodo3 = seleccionarNodoMenosCargado(listaNodosConectados,3);

	if ((nodo1 == NULL) || (nodo2 == NULL) || (nodo3 == NULL))
		return 0;


	pthread_t hiloCopiaNodo1;
	enviarDatos1 = obtenerDatosBloqueEnviar(bloque,archivoNuevo,contadorBloques,nodo1);
	pthread_create(&hiloCopiaNodo1,NULL,enviarCopiaANodo,enviarDatos1);

	pthread_t hiloCopiaNodo2;
	enviarDatos2 = obtenerDatosBloqueEnviar(bloque,archivoNuevo,contadorBloques,nodo2);
	pthread_create(&hiloCopiaNodo2,NULL,enviarCopiaANodo,enviarDatos2);

	pthread_t hiloCopiaNodo3;
	enviarDatos3 = obtenerDatosBloqueEnviar(bloque,archivoNuevo,contadorBloques,nodo3);
	pthread_create(&hiloCopiaNodo3,NULL,enviarCopiaANodo,enviarDatos3);

	int respuestaHiloNodo1;
	int respuestaHiloNodo2;
	int respuestaHiloNodo3;
	pthread_join(hiloCopiaNodo1, (void *)&respuestaHiloNodo1);
	pthread_join(hiloCopiaNodo2, (void *)&respuestaHiloNodo2);
	pthread_join(hiloCopiaNodo3, (void *)&respuestaHiloNodo3);
	free(enviarDatos1);
	free(enviarDatos2);
	free(enviarDatos3);

	if (respuestaHiloNodo1 == 0 || respuestaHiloNodo1 == 0 || respuestaHiloNodo3 == 0)
		return 0;



	return 1;
}
static void *enviarCopiaANodo(void *argEnviarDatosNodo)
{
	enviarDatosNodo *enviarDatos = argEnviarDatosNodo;
	char *bloque = enviarDatos->bloque;
	int contadorBloques = enviarDatos->contadorBloques;
	t_elementoFS *archivoNuevo = enviarDatos->archivoNuevo;
	t_nodo *nodo = enviarDatos->nodo;
	t_bloque *bloqueCopia;

	bloqueCopia = newBloque(nodo->nombre,posicionLibreEnNodo(nodo->bloques),contadorBloques,strlen(bloque));

	if (!grabarBloques(nodo,bloque,bloqueCopia))
		return 0;

	if (!estaConectado(nodo->conexionNodoServidor))
		return 0;

	list_add(archivoNuevo->listaCopias,bloqueCopia);

	return (void *)1;

}

static enviarDatosNodo *obtenerDatosBloqueEnviar(char *bloque,t_elementoFS *archivoNuevo,int contadorBloques, t_nodo *nodo)
{
	enviarDatosNodo *datosEnviar = malloc(sizeof(enviarDatosNodo));
	datosEnviar->archivoNuevo = archivoNuevo;
	datosEnviar->bloque = bloque;
	datosEnviar->nodo = nodo;
	datosEnviar->contadorBloques = contadorBloques;

	return datosEnviar;
}
