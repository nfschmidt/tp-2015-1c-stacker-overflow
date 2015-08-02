#include "ArchivoLocal.h"
#include "ArchivoPipe.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define ERROR_PIPE -1

extern const Archivo archivoLocalVtable;

static char *obtenerRegistroArchivoPipe(Archivo *archivo);
static void escribirRegistroArchivoPipe(Archivo *archivo, const char *registro);
static void establecerComoStdinArchivoPipe(Archivo *archivo);
static void establecerComoStdoutArchivoPipe(Archivo *archivo);
static void cerrarArchivoPipe(Archivo *archivo);

static void activarPipeDeLectura(ArchivoPipe *archivo);
static void activarPipeDeEscritura(ArchivoPipe *archivo);
static void manejarErrorPipe(int valorErrno);


ArchivoPipe *newArchivoPipe()
{
	ArchivoPipe *archivo = malloc(sizeof(ArchivoPipe));

	archivo->base.vtable.obtenerRegistro = obtenerRegistroArchivoPipe;
	archivo->base.vtable.escribirRegistro = escribirRegistroArchivoPipe;
	archivo->base.vtable.establecerComoStdin = establecerComoStdinArchivoPipe;
	archivo->base.vtable.establecerComoStdout = establecerComoStdoutArchivoPipe;
	archivo->base.vtable.cerrarArchivo = cerrarArchivoPipe;

	archivo->base.fileHandler = NULL;

	int resultadoPipe = pipe(archivo->pipes);
	if (resultadoPipe == ERROR_PIPE)
		manejarErrorPipe(errno);

	return archivo;
}


static char *obtenerRegistroArchivoPipe(Archivo *archivo)
{
	ArchivoPipe *archivoPipe = (ArchivoPipe *) archivo;

	if(archivoPipe->base.fileHandler == NULL)
		activarPipeDeLectura(archivoPipe);

	return archivoLocalVtable.obtenerRegistro((Archivo *) archivoPipe);
}


static void activarPipeDeLectura(ArchivoPipe *archivo)
{
	close(archivo->pipes[1]);
	archivo->base.fileHandler = fdopen(archivo->pipes[0], "r");
}


void escribirRegistroArchivoPipe(Archivo *archivo, const char *registro)
{
	ArchivoPipe *archivoPipe = (ArchivoPipe *) archivo;

	if(archivoPipe->base.fileHandler == NULL)
		activarPipeDeEscritura(archivoPipe);

	archivoLocalVtable.escribirRegistro((Archivo *) archivoPipe, registro);
}


static void activarPipeDeEscritura(ArchivoPipe *archivo)
{
	close(archivo->pipes[0]);
	archivo->base.fileHandler = fdopen(archivo->pipes[1], "w");
}


static void establecerComoStdinArchivoPipe(Archivo *archivo)
{
	ArchivoPipe *archivoPipe = (ArchivoPipe *) archivo;
	if(archivoPipe->base.fileHandler == NULL)
		activarPipeDeLectura(archivoPipe);

	archivoLocalVtable.establecerComoStdin(archivo);
}


static void establecerComoStdoutArchivoPipe(Archivo *archivo)
{
	ArchivoPipe *archivoPipe = (ArchivoPipe *) archivo;
	if(archivoPipe->base.fileHandler == NULL)
		activarPipeDeEscritura(archivoPipe);

	archivoLocalVtable.establecerComoStdout(archivo);
}


static void cerrarArchivoPipe(Archivo *archivo)
{
	ArchivoPipe *archivoPipe = (ArchivoPipe *) archivo;
	if(archivoPipe->base.fileHandler == NULL)
	{
		close(archivoPipe->pipes[0]);
		close(archivoPipe->pipes[1]);
	}
	else
	{
		archivoLocalVtable.cerrarArchivo(archivo);
	}
}


static void manejarErrorPipe(int valorErrno)
{
	fprintf(stderr, "Error al ejecutar pipe(): %s\n", strerror(valorErrno));
	exit(1);
}
