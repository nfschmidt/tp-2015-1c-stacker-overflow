#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#include "ejecucionDeRutina.h"

#include "procesos.h"
#include "Archivo.h"
#include "ArchivoLocal.h"
#include "ArchivoPipe.h"

#include "generadorNombreArchivo.h"

static int ejecutarRutinaYGuardarSalida(const char *pathDeRutina, Archivo *archivoDeEntrada, Archivo *archivoDeSalida);
static void ordenarArchivo(Archivo *archivoDeEntrada, Archivo *archivoDeSalida);


int ejecutarRutina(const char *pathDeRutina, Archivo *archivoDeEntrada, Archivo *archivoDeSalida)
{
	char *pathArchivoTemporal = obtenerNombreArchivoResultadoTemporal(TIPO_INTERMEDIO);
	int resultado;

	// Ejecuta Rutina
	Archivo *archivoTemporal = (Archivo *) newArchivoLocal(pathArchivoTemporal, ESCRITURA);
	int resultadoRutina = ejecutarRutinaYGuardarSalida(pathDeRutina, archivoDeEntrada, archivoTemporal);
	cerrarArchivo(archivoTemporal);

	resultado = resultadoRutina;

	if(resultadoRutina == RUTINA_OK)
	{
		// Ordena resultado
		archivoTemporal = (Archivo *) newArchivoLocal(pathArchivoTemporal, LECTURA);
		ordenarArchivo(archivoTemporal, archivoDeSalida);
		cerrarArchivo(archivoTemporal);
	}

	remove(pathArchivoTemporal);
	free(pathArchivoTemporal);

	return resultado;
}


static int ejecutarRutinaYGuardarSalida(const char *pathDeRutina, Archivo *archivoDeEntrada, Archivo *archivoDeSalida)
{
	Archivo *pipeDesdeEntradaARutina = (Archivo *) newArchivoPipe();

	/*
	 * Ignoro la signal de pipe que recibo si se me muere el hijo por algun problema
	 * Me voy a enterar si hubo un problema por el resultado que obtengo cuando espero al hijo
	 * Si no la atrapo me explota el padre tambien.
	 */
	signal(SIGPIPE, SIG_IGN);

	pid_t pid = crearProcesoHijoNoBloqueante(pathDeRutina, pipeDesdeEntradaARutina, archivoDeSalida);

	int resultadoDeCopia = copiarContenidoDeArchivo(archivoDeEntrada, pipeDesdeEntradaARutina);
	cerrarArchivo(pipeDesdeEntradaARutina);

	int resultadoRutina = esperarProcesoHijo(pid);

	if (resultadoRutina != PROCESO_OK)
		return RUTINA_ERROR_EJECUCION;

	return resultadoDeCopia == COPIA_OK ? RUTINA_OK : RUTINA_ERROR_LECTURA_DATOS;
}


static void ordenarArchivo(Archivo *archivoDeEntrada, Archivo *archivoDeSalida)
{
	setenv("LC_ALL", "C", 1);
	crearProcesoHijoBloqueante("sort", archivoDeEntrada, archivoDeSalida);
}
