/*
 * recibirArchivoFinalNodo.c-
 *
 *  Created on: 16/7/2015
 *      Author: utnso
 */
#include <libsocket.h>
#include "copiarArchivoAMDFS.h"
#include <stdlib.h>
#include "generadorNombreArchivo.h"
#include <stdio.h>
#include "logueo.h"
#include "conexionMongo.h"

#define RUTA_ARCHIVO_LINUX "/tmp/reciboArchivoNodo.txt"

#define OPERACION_OK 'O'
#define OPERACION_FAIL 'F'




static char *recibirContenidoDelArchivo(SocketConecta *conexion);

void *atenderANodoParaArchivo(void *argConexion)
{

	loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Empezo a grabar el archivo final en el MDFS");
	SocketConecta *conexion = argConexion;

	int longitudPath;
	if(estaConectado(conexion))
		recibirMensaje(conexion,&longitudPath,sizeof(int));

	char *path = malloc(longitudPath + 1);
	if(estaConectado(conexion))
		recibirMensaje(conexion,path,longitudPath);
	path[longitudPath] = '\0';


	int longitudNombreArchivo;
	if(estaConectado(conexion))
		recibirMensaje(conexion,&longitudNombreArchivo,sizeof(int));


	char *nombreArchivo = malloc(longitudNombreArchivo + 1);
	if(estaConectado(conexion))
		recibirMensaje(conexion,nombreArchivo,longitudNombreArchivo);

	nombreArchivo[longitudNombreArchivo] = '\0';


	char *rutaArchivoLinux = recibirContenidoDelArchivo(conexion);
	if(rutaArchivoLinux == NULL)
		return 0;

	if(copiarArchivoAMDFS(rutaArchivoLinux,path,nombreArchivo))
	{

		char operacionOk = OPERACION_OK;
		if(estaConectado(conexion))
			enviarMensaje(conexion,&operacionOk,1);

		borrarDatosElementos();
		inicializarMongoParaElementos(root);
		borrarDatosNodos();
		persistirNodos();
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Archivo subido al MDFS correctamente");
	}else
	{
		char operacionFail = OPERACION_FAIL;
		if(estaConectado(conexion))
			enviarMensaje(conexion,&operacionFail,1);
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"El archivo no se pudo subir al MDFS");
	}

	return 0;
}

static char *recibirContenidoDelArchivo(SocketConecta *conexion)
{

	char *nombreArchivoEnLinux = obtenerNombreArchivoResultadoTemporal();

	FILE *archivo = fopen(nombreArchivoEnLinux, "w");
	uint64_t bytesRecibidos;
	char *buffer;

	do
	{	if(estaConectado(conexion))
		{
			bytesRecibidos = recibirTodo(conexion,(void**) &buffer);
			if(bytesRecibidos == 0)
				break;

			fwrite(buffer,bytesRecibidos,1,archivo);
			free(buffer);

			char x;
			if(estaConectado(conexion))
				enviarMensaje(conexion,&x,1);

		}else
		{
			loguear(LOG_LEVEL_DEBUG,"Se desconecto el Nodo");
			return NULL;
		}
	}while(bytesRecibidos != 0);


	fclose(archivo);
	return nombreArchivoEnLinux;
}

