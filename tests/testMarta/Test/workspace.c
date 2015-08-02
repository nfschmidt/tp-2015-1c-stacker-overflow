#include "protocoloFileSystem.h"
#include "configMarta.h"
#include "nodo.h"
#include <stdio.h>
#include "logueo.h"

static void testArchivoDisponible(char *path);
static void testCantidadBloques(char *path);
static void testDetalleBloques(char *path, int bloque);


void workspace(char *archivoConfiguracion)
{
	t_configuracionMarta *config = levantarArchivoConfiguracion(archivoConfiguracion);
	setPathArchivoDeLog(config->pathArchivoLog);

	establecerConexionConFileSystem(config->ipFileSystem, config->puertoFileSystem);

	testArchivoDisponible("/sarasa");
	testCantidadBloques("/sarasa");
	testArchivoDisponible("/holy.txt");
	testCantidadBloques("/holy.txt");
	testDetalleBloques("/holy.txt", 0);
	testDetalleBloques("/holy.txt", 1);
	testDetalleBloques("/holy.txt", 2);
	testDetalleBloques("/sarasa.txt", 1);


	testArchivoDisponible("/mr");
	testCantidadBloques("/mr");
	testDetalleBloques("/mr", 0);
}

static void testArchivoDisponible(char *path)
{
	printf("Pregunto si existe %s\n", path); fflush(stdout);

	int existeArchivo = consultarArchivoDisponible(path);
	if (existeArchivo)
	{
		printf("Existe %s\n", path);
	}else{
		printf("No existe %s\n", path);
	}
}

static void testCantidadBloques(char *path)
{
	printf("Pregunto bloques %s\n", path); fflush(stdout);

	int cantidadDeBloques = consultarCantidadBloquesArchivo(path); fflush(stdout);
	printf("Cantidad de blques de %s: %d\n", path, cantidadDeBloques); fflush(stdout);
}

static void testDetalleBloques(char *path, int bloque)
{
	printf("Pregunto detalle de bloque %d de archivo %s\n", bloque, path); fflush(stdout);

	t_list *nodosConBloques = consultarDetallesBloqueArchivo(path, bloque);

	int contador = 0;
	while(contador < list_size(nodosConBloques))
	{
		t_nodo *nodo = list_get(nodosConBloques, contador);
		printf("\tArchivo %s\tNodo: %s\tIP: %s\tPuerto: %s\tBloque en nodo: %d\n",
				path,
				nodo->nombre,
				nodo->ip,
				nodo->puerto,
				nodo->bloque); fflush(stdout);

		contador ++;
	}

	printf("Fin de pedido de bloque\n");
}
