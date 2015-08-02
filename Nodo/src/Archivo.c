#include "Archivo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SIZE_BUFFER_REGISTROS (8 * 1024)

static int resultadoDeCopia(char *ultimoRegistro);

void establecerComoStdin(Archivo *archivo)
{
	archivo->establecerComoStdin(archivo);
}


void cerrarArchivo(Archivo *archivo)
{
	archivo->cerrarArchivo(archivo);
}


void escribirRegistro(Archivo *archivo, const char *registro)
{
	archivo->escribirRegistro(archivo, registro);
}


void establecerComoStdout(Archivo *archivo)
{
	archivo->establecerComoStdout(archivo);
}


char *obtenerRegistro(Archivo *archivo)
{
	return archivo->obtenerRegistro(archivo);
}


int copiarContenidoDeArchivo(Archivo *origen, Archivo *destino)
{
	char *registroActual = obtenerRegistro(origen);

	while(registroActual != NULL && !huboError(registroActual))
	{
		long long bytesEscritos = 0;
		char bufferRegistros[SIZE_BUFFER_REGISTROS + 1];
		bufferRegistros[0] = '\0';

		while (bytesEscritos < SIZE_BUFFER_REGISTROS)
		{
			strcat(bufferRegistros, registroActual);
			bytesEscritos += strlen(registroActual);
			free(registroActual);

			registroActual = obtenerRegistro(origen);
			if (registroActual == NULL || huboError(registroActual) || bytesEscritos + strlen(registroActual) > SIZE_BUFFER_REGISTROS)
				break;
		}

		if (!huboError(registroActual))
			escribirRegistro(destino, bufferRegistros);
	}

	return resultadoDeCopia(registroActual);
}


int huboError(char *registro)
{
	return registro != NULL && registro[strlen(registro) -1] != '\n';
}

static int resultadoDeCopia(char *ultimoRegistro)
{
	if (ultimoRegistro == NULL)
		return COPIA_OK;

	return ERROR_COPIA;
}
