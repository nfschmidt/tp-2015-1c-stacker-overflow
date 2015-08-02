#include "ArchivoApareado.h"
#include "ejecucionDeRutina.h"
#include "mapReduce.h"
#include "ArchivoBloque.h"


int ejecutarRutinaDeMap(const char *pathDeRutina, BloqueDatos *bloque, Archivo *archivoDeSalida)
{
	Archivo *archivoDeEntrada = (Archivo*) newArchivoBloque(bloque);

	return ejecutarRutina(pathDeRutina, archivoDeEntrada, archivoDeSalida);
}

int ejecutarRutinaDeReduce(const char *pathDeRutinaReduce, t_list *listaArchivos, Archivo *archivoDeSalida)
{
	Archivo *archivoApareado = (Archivo *) newArchivoApareado(listaArchivos);

	int resultado = ejecutarRutina(pathDeRutinaReduce, archivoApareado, archivoDeSalida);

	cerrarArchivo(archivoApareado);

	return resultado;
}
