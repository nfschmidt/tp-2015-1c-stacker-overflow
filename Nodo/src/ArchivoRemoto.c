#include "ArchivoRemoto.h"
#include "consultarNodo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define ERROR_REGISTRO_REMOTO "ERROR_REGISTRO"
#define ERROR_NO_EXISTE_ARCHIVO "ERROR_ARCHIVO"


static char *obtenerRegistroArchivoRemoto(Archivo *archivo);
static void cerrarArchivoRemoto(Archivo *archivo);

ArchivoRemoto *newArchivoRemoto(char *nombreArchivo, SocketConecta *conexion)
{
	char existeArchivoRemoto = tenesArchivo(conexion, nombreArchivo);

	ArchivoRemoto *archivo = malloc(sizeof(ArchivoRemoto));

	archivo->conexion = conexion;
	archivo->vtable.obtenerRegistro = obtenerRegistroArchivoRemoto;
	archivo->vtable.cerrarArchivo = cerrarArchivoRemoto;
	archivo->existeArchivoRemoto = (existeArchivoRemoto == 'S');
	archivo->nombreArchivo = strdup(nombreArchivo);
	archivo->conjuntoRegistros = NULL;
	archivo->posicionConjuntoRegistros = 0;

	if (existeArchivoRemoto != 'S')
		cerrarSocketConecta(conexion);

	return archivo;
}

static char *obtenerRegistroArchivoRemoto(Archivo *archivo)
{
	ArchivoRemoto *archivoRemoto = (ArchivoRemoto*) archivo;

	if (! archivoRemoto->existeArchivoRemoto)
		return strdup(ERROR_NO_EXISTE_ARCHIVO);

	if (archivoRemoto->conjuntoRegistros == NULL)
		archivoRemoto->conjuntoRegistros = getRegistroRemoto(archivoRemoto->conexion);

	if (archivoRemoto->conjuntoRegistros == NULL)
		return NULL;

	if (strcmp(archivoRemoto->conjuntoRegistros, CAYO_SERVER_C) == 0)
		return strdup(ERROR_REGISTRO_REMOTO);

	uint64_t i = 0;

	while(archivoRemoto->conjuntoRegistros[archivoRemoto->posicionConjuntoRegistros + i] != '\n')
		i++;

	char *registro = malloc(i + 2);
	memcpy(registro, archivoRemoto->conjuntoRegistros + archivoRemoto->posicionConjuntoRegistros, i + 1);
	registro[i+1] = '\0';

	archivoRemoto->posicionConjuntoRegistros += i + 1;
	if(archivoRemoto->conjuntoRegistros[archivoRemoto->posicionConjuntoRegistros] == '\0')
	{
		free(archivoRemoto->conjuntoRegistros);
		archivoRemoto->conjuntoRegistros = NULL;
		archivoRemoto->posicionConjuntoRegistros = 0;
	}

	return registro;
}

static void cerrarArchivoRemoto(Archivo *archivo)
{
	ArchivoRemoto *archivoRemoto = (ArchivoRemoto*) archivo;

	termineDePedir(archivoRemoto->conexion);

	char ultimoMensaje;
	recibirMensaje(archivoRemoto->conexion, &ultimoMensaje, 1);

	cerrarSocketConecta(archivoRemoto->conexion);
}
