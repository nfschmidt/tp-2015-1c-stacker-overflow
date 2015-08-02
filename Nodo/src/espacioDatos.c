#define __USE_LARGEFILE64
#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <err.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "espacioDatos.h"
#include "configNodo.h"
#include "bloqueDatos.h"
#include <errno.h>
#include "logueo.h"
#include <sys/stat.h>





off_t calcularEspacioArchivo(char *ruta)
{
	struct stat buffer;
	stat(ruta,&buffer);
	return 	buffer.st_size;
}

void obtenerArchivoMemoria(char *ruta, EspacioDatos *esp)
{
	char *archMemoria;

	FILE *archivo = fopen(ruta, "r+");

	if (archivo == NULL)
	{
		int codigoError = errno;
		fprintf(stderr, "ERROR: fopen fallo.\nCodigo de error: %d\nError: %s", codigoError, strerror(codigoError));
		exit(1);
	}

    esp->Fd = fileno(archivo);
    archMemoria = (char*)mmap(NULL, esp->Size, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, esp->Fd, 0);
	if (archMemoria == MAP_FAILED)
	{
		int codigoError = errno;
		fprintf(stderr, "Error: mmap fallo.\nCodigo de codigo de Error: %d\nError: %s", codigoError, strerror(codigoError));
		exit(1);
	}

	esp->archivoMemoria = archMemoria;
}

void cerrarEspacioDatos(EspacioDatos *esp)
{
	munmap(esp->archivoMemoria,esp->Size);
	close(esp->Fd);
}

EspacioDatos *newEspacioDatos(char *ruta)
{
	EspacioDatos *nEspacioDatos = malloc(sizeof(EspacioDatos));

	nEspacioDatos->Size = calcularEspacioArchivo(ruta);

	obtenerArchivoMemoria(ruta,nEspacioDatos);

	return nEspacioDatos;
}

int cantidadBloquesEspacio(EspacioDatos *esp)
{
	int cantBloques = esp->Size / tamanioBloqueFijo;


	return cantBloques;
}

void setBloque(EspacioDatos *esp,char *bloque, int numeroBloque)
{
	off_t posEscritura = tamanioBloqueFijo * numeroBloque;
	off_t finEscritura = posEscritura + strlen(bloque);
	off_t finBloque = tamanioBloqueFijo * (numeroBloque + 1);
	finBloque--;

	loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Se grabo datos en el bloque %d", numeroBloque);

	int i = 0;
	while (posEscritura < finEscritura)
	{
		esp->archivoMemoria[posEscritura] = bloque[i];
		i++;
		posEscritura++;
	}

	if (finEscritura < finBloque)
	{
		while (posEscritura < finBloque)
		{
			esp->archivoMemoria[posEscritura] = '\0';
			posEscritura++;
		}
	}

}
