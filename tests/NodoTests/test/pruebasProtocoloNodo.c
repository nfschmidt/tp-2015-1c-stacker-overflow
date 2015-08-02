#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libsocket.h>

#include "ArchivoLocal.h"
#include "atenderConexiones.h"
#include "ArchivoRemoto.h"


#define PATH_TEMPORAL "/home/utnso/Escritorio/"
#define NOMBRE_ARCHIVO "archivoPrueba.txt"
#define PATH_ARCHIVO PATH_TEMPORAL NOMBRE_ARCHIVO


void transferirRegistrosRemotos(int argc, char *argv[])
{
	/*
	 * PAra ejecutar las pruebas:
	 * ./NodoTest servidor
	 *
	 * ./NodoTest cliente
	 */

	if (argc < 2)
	{
		fprintf(stderr, "Warning: tenes que indicar si es cliente o servidor para probar el protocolo\n");
		return;
	}


	if (strcmp(argv[1], "servidor") == 0)
	{
		Archivo *archivoLocal = (Archivo*) newArchivoLocal(PATH_ARCHIVO, ESCRITURA);
		int i = 0;
		for (i = 0; i <= 100000; i ++)
		{
			char registro[100];
			sprintf(registro, "REGISTRO %d\n", i);
			escribirRegistro(archivoLocal, registro);
		}
		cerrarArchivo(archivoLocal);

		escucharConexionesANodo("12345", NULL);
	}
	else
	{
		char *ipServer;
		if (argc == 3)
			ipServer = strdup(argv[2]);
		else
			ipServer = strdup("127.0.0.1");

		SocketConecta *conexion = newSocketConecta(ipServer, "12345");
		conectarAServidor(conexion);

		Archivo *archivoRemoto = (Archivo*) newArchivoRemoto(NOMBRE_ARCHIVO, conexion);

		char *registro = obtenerRegistro(archivoRemoto);

		while (registro != NULL && ! huboError(registro))
		{
			printf("Registro recibido: %s", registro);

			free(registro);
			registro = obtenerRegistro(archivoRemoto);
		}

		if (registro == NULL)
		{
			printf("Recibi el ultimo regisro\n");
			cerrarArchivo(archivoRemoto);
		}
		else
			printf("Se desconecto el server");

		cerrarSocketConecta(conexion);
		printf("Cerre la conexion\n"); fflush(stdout);
	}
}


#undef PATH_TEMPORAL
#undef NOMBRE_ARCHIVO
#undef PATH_ARCHIVO
