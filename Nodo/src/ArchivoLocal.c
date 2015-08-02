#include "ArchivoLocal.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>


static char *obtenerRegistroArchivoLocal(Archivo *archivo);
static void escribirRegistroArchivoLocal(Archivo *archivo, const char *registro);
static void cerrarArchivoLocal(Archivo *archivo);
static void establecerComoStdinArchivoLocal(Archivo *archivo);
static void establecerComoStdoutArchivoLocal(Archivo *archivo);


const Archivo archivoLocalVtable =
{
	.obtenerRegistro = obtenerRegistroArchivoLocal,
	.escribirRegistro = escribirRegistroArchivoLocal,
	.cerrarArchivo = cerrarArchivoLocal,
	.establecerComoStdin = establecerComoStdinArchivoLocal,
	.establecerComoStdout = establecerComoStdoutArchivoLocal
};


ArchivoLocal *newArchivoLocal(const char *path, ModoApertura modo)
{
	ArchivoLocal *archivo = malloc(sizeof(ArchivoLocal));
	archivo->vtable = archivoLocalVtable;

	switch(modo)
	{
	case LECTURA:
		archivo->fileHandler = fopen(path, "rb");
		break;
	case ESCRITURA:
		archivo->fileHandler = fopen(path, "wb");
		break;
	}

	if (archivo->fileHandler == NULL)
	{
		free(archivo);
		archivo = NULL;
	}

	return archivo;
}

#define LONGITUD_BUFFER 1024
static char *obtenerRegistroArchivoLocal(Archivo *archivo)
{
	ArchivoLocal *archivoLocal = (ArchivoLocal *) archivo;

	off_t espacioDisponible = 0;
	off_t caracteresLeidos = 0;

	char *registro = NULL;
	char *resultadoDeLectura;

	do
	{
		espacioDisponible += LONGITUD_BUFFER;
		registro = realloc(registro, espacioDisponible);

		resultadoDeLectura = fgets(registro + caracteresLeidos, LONGITUD_BUFFER, archivoLocal->fileHandler);

		caracteresLeidos += strlen(registro + caracteresLeidos);
	} while(registro[caracteresLeidos - 1] != '\n' && resultadoDeLectura != NULL);

	if (registro[caracteresLeidos - 1] != '\n' || resultadoDeLectura == NULL)
	{
		free(registro);
		return NULL;
	}

	return registro;
}
#undef LONGITUD_BUFFER


static void escribirRegistroArchivoLocal(Archivo *archivo, const char *registro)
{
	ArchivoLocal *archivoLocal = (ArchivoLocal *) archivo;
	fprintf(archivoLocal->fileHandler, "%s", registro);

	size_t longitudRegistro = strlen(registro);
	if(registro[longitudRegistro - 1] != '\n')
		fprintf(archivoLocal->fileHandler, "\n");
}


static void cerrarArchivoLocal(Archivo *archivo)
{
	ArchivoLocal *archivoLocal = (ArchivoLocal *) archivo;
	fclose(archivoLocal->fileHandler);
	free(archivoLocal);
}

static void establecerComoStdinArchivoLocal(Archivo *archivo)
{
	ArchivoLocal *archivoLocal = (ArchivoLocal *) archivo;
	dup2(fileno(archivoLocal->fileHandler), fileno(stdin));
}


static void establecerComoStdoutArchivoLocal(Archivo *archivo)
{
	ArchivoLocal *archivoLocal = (ArchivoLocal *) archivo;
	dup2(fileno(archivoLocal->fileHandler), fileno(stdout));
}

size_t obtenerTamanioSiguienteRegistro(ArchivoLocal *archivoLocal)
{
	char *registro = obtenerRegistroArchivoLocal((Archivo *) archivoLocal);
	if (registro == NULL)
		return 0;

	size_t tamanio = strlen(registro);

	fseek(archivoLocal->fileHandler, -tamanio, SEEK_CUR);

	free(registro);

	return tamanio;
}
