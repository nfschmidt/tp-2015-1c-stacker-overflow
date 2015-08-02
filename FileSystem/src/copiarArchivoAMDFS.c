/*
 * copiarArchivoAMDFS.c
 *
 *  Created on: 20/6/2015
 *      Author: utnso
 */
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
#include "asignarBloque.h"
#include "copiarArchivoAMDFS.h"
#include "distribuirBloques.h"
#include "estadoFileSystem.h"
#include "enviarBloqueANodos.h"
#include "logueo.h"

static t_elementoFS *creoArchivoNuevoFS(char *ruta,char *rutaMDFS, char *nombreArchivo);
static int verSIExisteLugarYEnviar(char *ruta, char *rutaMDFS, char *nombreArchivo,int enviar);
static t_list *crearListaAuxiliarConNodosAuxiliares();
static int vaciarTodoYEliminarLista(char *bloque, t_list *listaAux,char *rutaMDFS, char *nombreArch
		,FILE *archivo, int enviar,int fallo);
static off_t tamanioArchivoLinux(char *ruta);

int copiarArchivoAMDFS(char *ruta,char *rutaMDFS, char *nombreArchivo)
{
	if (existeArchivoQueQuieroAgregar(rutaMDFS,nombreArchivo))
		return 0;

	if (verSIExisteLugarYEnviar(ruta,rutaMDFS,nombreArchivo,0))
		return verSIExisteLugarYEnviar(ruta,rutaMDFS,nombreArchivo,1);

	return 0;
}


static t_elementoFS *creoArchivoNuevoFS(char *ruta,char *rutaMDFS, char *nombreArchivo)
{
	t_elementoFS *archivoNuevo;

	///creo el archivo en la ruta del MDFS
	archivoNuevo =crearArchivo(rutaMDFS,nombreArchivo,root);

	return archivoNuevo;
}

static off_t tamanioArchivoLinux(char *ruta)
{
	struct stat buffer;
	stat(ruta,&buffer);
	return 	buffer.st_size;

}

static int verSIExisteLugarYEnviar(char *ruta, char *rutaMDFS, char *nombreArchivo,int enviar)
{
	off_t contadorArchivo = 0,contadorBloque = 0,ultimaPosBarraN = 0;
	off_t tamanioArchivo;
	long int posicionReubicarArchivo;
	int caracter,cantidadBloques = 0;
	FILE *archivo = abrirArchivoParaBloque(ruta);

	if (archivo == NULL)
		return 0;

	tamanioArchivo = tamanioArchivoLinux(ruta);
	t_elementoFS *archivoNuevo = NULL;

	if (enviar == 1)
	{
		archivoNuevo = creoArchivoNuevoFS(ruta,rutaMDFS,nombreArchivo);
		if (archivoNuevo == NULL)
			return 0;
	}
	t_list *listaAux = crearListaAuxiliarConNodosAuxiliares();
	char *bloque = malloc(TAMANIO_MAXIMO_BLOQUE + 1);

	if (enviar == 0)
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Verificando si hay espacio disponible para subir el archivo");
	else
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO,"Grabando archivo");

	while(contadorArchivo < tamanioArchivo)
	{

		contadorArchivo++;
		caracter = fgetc(archivo);

		bloque[contadorBloque] = caracter;

		if (caracter == '\n')
			ultimaPosBarraN = contadorBloque;

		if ((contadorBloque >= TAMANIO_ANTEULTIMO_BLOQUE) || (contadorArchivo == tamanioArchivo))
		{
			//aca lei 20mega

			posicionReubicarArchivo = posicionALeerReubicarArchivo(ultimaPosBarraN,contadorBloque);
			if (ultimaPosBarraN == 0)
				ultimaPosBarraN = contadorBloque;


			copiarMemoriaBloques(ultimaPosBarraN,&bloque);


			if (!verEnviarOIncrementarBloques(bloque,archivoNuevo,cantidadBloques,enviar,listaAux))
				return vaciarTodoYEliminarLista(bloque,listaAux,rutaMDFS,nombreArchivo,archivo,enviar,1);

			ultimaPosBarraN = 0;
			contadorBloque = -1;

			cantidadBloques++;
			fseek(archivo,posicionReubicarArchivo,SEEK_CUR);
			contadorArchivo += posicionReubicarArchivo;
			free(bloque);
			bloque = malloc(TAMANIO_MAXIMO_BLOQUE + 1);
		}
		contadorBloque++;
	}
	vaciarTodoYEliminarLista(bloque,listaAux,rutaMDFS,nombreArchivo,archivo,enviar,0);

	if (enviar == 1)
		archivoNuevo->cantidadBloques = cantidadBloques;
	return 1;
}

static int vaciarTodoYEliminarLista(char *bloque, t_list *listaAux,char *rutaMDFS, char *nombreArch
		,FILE *archivo, int enviar,int fallo)
{
	free(bloque);
	eliminarLista(listaAux);
	fclose(archivo);
	if (enviar == 1 && fallo == 1)
	{
		char * rutaCompleta = malloc(strlen(rutaMDFS) + strlen(nombreArch) + 2);
		rutaCompleta[0] = '\0';
		strcat(rutaCompleta , rutaMDFS);
		if (strcmp(rutaMDFS,"/") != 0)
			strcat(rutaCompleta , "/");
		strcat(rutaCompleta, nombreArch);
		eliminarArchivo(nombreArch,root);
		free(rutaCompleta);
	}
	return 0;
}

static t_list *crearListaAuxiliarConNodosAuxiliares()
{
	reservarListaNodosConectados();
	t_list *listaAux = list_create();
	int contador =0;

	while(contador < list_size(listaNodosConectados))
	{
		t_nodo *nodo = list_get(listaNodosConectados,contador);
		if (estaHabilitado(nodo))
		{
			t_nodo *nodoNuevo = newNodo(nodo->ip, nodo->puerto, nodo->nombre, nodo->bloquesDisponibles,nodo->conexionNodoServidor);
			nodoNuevo->estado = nodo->estado;
			nodoNuevo->bloques = nodo->bloques;

			nodoNuevo->bloquesUsados  = nodo->bloquesUsados;
			list_add(listaAux,nodoNuevo);
		}
		contador++;
	}

	devolverListaNodosConectados();
	return listaAux;
}
