#include "ArchivoApareado.h"

#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>


static char *obtenerRegistroArchivoApareado(Archivo *archivo);
static void cerrarArchivoApareado(Archivo *archivo);

static void leerPrimerRegistroDeTodosLosArchivos(ArchivoApareado *archivo);
static void leerRegistroDeUltimoArchivoQueEntregoRegistro(ArchivoApareado *archivo);
static int obtenerIndiceMenorRegistro(ArchivoApareado *archivo);
static void actualizarListaRegistrosLeidos();
static void *obtenerRegistroDeArchivo(void *archivo);


ArchivoApareado *newArchivoApareado(t_list *listaArchivos)
{
	ArchivoApareado *archivo = malloc(sizeof(ArchivoApareado));

	archivo->listaArchivos = listaArchivos;
	archivo->listaRegistrosLeidos = NULL;

	archivo->vtable.obtenerRegistro = obtenerRegistroArchivoApareado;
	archivo->vtable.cerrarArchivo = cerrarArchivoApareado;


	return archivo;
}



static char *obtenerRegistroArchivoApareado(Archivo *archivo)
{
	ArchivoApareado *archivoApareado = (ArchivoApareado *) archivo;

	actualizarListaRegistrosLeidos(archivoApareado);
	archivoApareado->indiceArchivoQueEntregaRegistro = obtenerIndiceMenorRegistro(archivoApareado);

	return list_get(archivoApareado->listaRegistrosLeidos, archivoApareado->indiceArchivoQueEntregaRegistro);
}


static void actualizarListaRegistrosLeidos(ArchivoApareado *archivo)
{
	if(archivo->listaRegistrosLeidos == NULL)
		leerPrimerRegistroDeTodosLosArchivos(archivo);
	else
		leerRegistroDeUltimoArchivoQueEntregoRegistro(archivo);
}


void leerPrimerRegistroDeTodosLosArchivos(ArchivoApareado *archivo)
{
	archivo->listaRegistrosLeidos = list_map(archivo->listaArchivos, obtenerRegistroDeArchivo);
}


void leerRegistroDeUltimoArchivoQueEntregoRegistro(ArchivoApareado *archivo)
{
	list_remove(archivo->listaRegistrosLeidos, archivo->indiceArchivoQueEntregaRegistro);

	Archivo *ultimoArchivoQueEntregoRegistro = list_get(archivo->listaArchivos,
														archivo->indiceArchivoQueEntregaRegistro);

	list_add_in_index(archivo->listaRegistrosLeidos,
			archivo->indiceArchivoQueEntregaRegistro,
			obtenerRegistro(ultimoArchivoQueEntregoRegistro));
}


static int obtenerIndiceMenorRegistro(ArchivoApareado *archivo)
{
	int indiceMenorRegistro = 0;
	char *menorRegistro = list_get(archivo->listaRegistrosLeidos, 0);

	int i;
	for (i = 1; i < list_size(archivo->listaRegistrosLeidos); i ++)
	{
		char *registroActual = list_get(archivo->listaRegistrosLeidos, i);
		if (menorRegistro == NULL || (registroActual != NULL && strcmp(registroActual, menorRegistro) < 0))
		{
			menorRegistro = registroActual;
			indiceMenorRegistro = i;
		}
	}

	return indiceMenorRegistro;
}


static void *obtenerRegistroDeArchivo(void *archivo)
{
	return obtenerRegistro(((Archivo *) archivo));
}


static void cerrarArchivoApareado(Archivo *archivo)
{
	ArchivoApareado *archivoApareado = (ArchivoApareado *) archivo;

	int i;
	for (i = 0; i < list_size(archivoApareado->listaArchivos); i++)
	{
		Archivo *a = list_get(archivoApareado->listaArchivos, i);
		cerrarArchivo(a);
	}
}
