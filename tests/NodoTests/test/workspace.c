#include <stdio.h>
#include <commons/string.h>
#include <string.h>
#include "subidaArchivoMDFS.h"
#include "configNodo.h"
#include "espacioDatos.h"
#include "bloqueDatos.h"
#include "ArchivoBloque.h"
#include "ArchivoLocal.h"
#include <stdlib.h>
#include <commons/collections/list.h>
#include "ArchivoApareado.h"

void ejecutarCasoDeUso(char *argv[]);


void workspace(int argc, char *argv[])
{
	t_list *listaArchivos = list_create();

	int i; for(i = 1; i < argc; i ++)
		list_add(listaArchivos, newArchivoLocal(argv[i], LECTURA));

	Archivo *archivoApareado = (Archivo*) newArchivoApareado(listaArchivos);
	Archivo *archivoSalido = (Archivo *) newArchivoLocal("/tmp/salidaApareada.txt", ESCRITURA);

	char *registro = obtenerRegistro(archivoApareado);
	while (registro != NULL)
	{
		escribirRegistro(archivoSalido, registro);
		free(registro);
		registro = obtenerRegistro(archivoApareado);
	}


	//ejecutarCasoDeUso(argv);
}




void ejecutarPruebasDeWorkspace(int argc, char *argv[])
{
	printf("********************************************\n");
	printf("*\t- Pruebas de workspace -");
	printf("\n********************************************\n\n");

	workspace(argc, argv);

	printf("\n\n********************************************\n");
	printf("*\t- Fin pruebas de workspace -");
	printf("\n********************************************\n");

}
