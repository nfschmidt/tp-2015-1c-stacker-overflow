#ifndef ATENDERAJOB_H_
#define ATENDERAJOB_H_

#include <commons/collections/list.h>

typedef struct
{
	char *pathRutina;
	char *tipoRutina;
	int bloque;
	t_list *listaArchivosRemotos;
	char *nombreArchivoResultado;
	int resultado;
} Tarea;

void *ejecutarTareaDeJob(void *conexionArg);
void *ejecutarUltimaTareaDeJob(void *conexionArg);
#endif
