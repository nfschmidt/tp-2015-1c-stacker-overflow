#include "subidaArchivoMDFS.h"
#include "logueo.h"

#include <commons/string.h>
#include <libsocket.h>
#include "configNodo.h"
#include <string.h>
#include <stdlib.h>

#define MENSAJE_PEDIDO_SUBIDA_ARCHIVO 'A'
#define MENSAJE_SUBIDA_OK 'O'
#define MENSAJE_SUBIDA_FAIL 'F'

#define SIZE_BLOQUE_DE_ENVIO (1024*32)

int conectarAFilesystemYSubirArchivo(char *nombreArchivo, char *pathArchivoEnMDFS)
{
	ConfiguracionNodo *config = obtenerConfiguracionNodo();

	SocketConecta *conexion = newSocketConecta(config->ip_fs, config->puerto_fs);
	conectarAServidor(conexion);

	if (! estaConectado(conexion))
		return SUBIDA_FALLO;

	char pedidoSubidaArchivo = MENSAJE_PEDIDO_SUBIDA_ARCHIVO;
	enviarMensaje(conexion, &pedidoSubidaArchivo, 1);


	int contador = strlen(pathArchivoEnMDFS);
	while(pathArchivoEnMDFS[contador] != '/')
		contador--;

	char *pathDirectorio;
	if (contador == 0)
		pathDirectorio = "/";
	else
		pathDirectorio = string_substring_until(pathArchivoEnMDFS, contador);
	char *nombre = string_substring_from(pathArchivoEnMDFS, contador + 1);

	int longitudMensaje = strlen(pathDirectorio);
	enviarMensaje(conexion, &longitudMensaje, sizeof(int));
	enviarMensaje(conexion, pathDirectorio, longitudMensaje);

	longitudMensaje = strlen(nombre);
	enviarMensaje(conexion, &longitudMensaje, sizeof(int));
	enviarMensaje(conexion, nombre, longitudMensaje);


	char *pathEspacioTemporal = obtenerPathEspacioTemporal();
	char *rutaArchivo = malloc(strlen(pathEspacioTemporal) + strlen(nombreArchivo) + 2);
	strcpy(rutaArchivo, pathEspacioTemporal);
	strcat(rutaArchivo, "/");
	strcat(rutaArchivo, nombreArchivo);

	loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Subiendo archivo %s al MDFS en %s", rutaArchivo, pathArchivoEnMDFS);
	FILE *contenidoArchivo = fopen(rutaArchivo, "r");

	char buffer[SIZE_BLOQUE_DE_ENVIO + 1];
	size_t bytesLeidos = fread(buffer, 1, SIZE_BLOQUE_DE_ENVIO, contenidoArchivo);
	while(bytesLeidos > 0)
	{
		enviarTodo(conexion, buffer, bytesLeidos);
		bytesLeidos = fread(buffer, 1, SIZE_BLOQUE_DE_ENVIO, contenidoArchivo);

		char siguiente;
		recibirMensaje(conexion, &siguiente, 1);
	}

	int cero = 0;
	enviarMensaje(conexion, &cero, sizeof(int));

	char resultadoSubida;
	recibirMensaje(conexion, &resultadoSubida, 1);

	if (resultadoSubida == MENSAJE_SUBIDA_OK)
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Subida de archivo %s al MDFS en %s finalizada", nombreArchivo, pathArchivoEnMDFS);
		return SUBIDA_OK;
	}

	loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Subida de archivo %s al MDFS en %s fallida", nombreArchivo, pathArchivoEnMDFS);
	return SUBIDA_FALLO;
}
