#include "ArchivoBloque.h"
#include "registroDatos.h"
#include <stdlib.h>


static char *obtenerRegistroArchivoBloque(Archivo *archivo);



ArchivoBloque *newArchivoBloque(BloqueDatos *bloque)
{
	ArchivoBloque *archivo = malloc(sizeof(ArchivoBloque));

	archivo->bloque = bloque;
	archivo->vtable.obtenerRegistro = obtenerRegistroArchivoBloque;
	archivo->primerRegistroEstaLeido = 0;

	return archivo;
}




static char *obtenerRegistroArchivoBloque(Archivo *archivo)
{
	ArchivoBloque *archivoBloque = (ArchivoBloque *)archivo;
	char *registro = getRegistro(archivoBloque->bloque);
	return registro;
}
