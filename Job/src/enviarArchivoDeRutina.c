#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "configJob.h"
#include "archivosConfiguracion.h"
#include "logs.h"
#include <libsocket.h>
#include <commons/collections/list.h>
#include <string.h>
#define TAMLIN 400

void enviarArchivoANodo(SocketConecta *conectarANodo,char *pathRutina)
{

	FILE *archivo;

	archivo=fopen(pathRutina,"r");

	if(archivo==NULL)
	{
		printf("No se pudo abrir el archivo de rutina: %s \n",pathRutina);
		exit(1);
	}

	//Recorro el archivo mandando de a poco
	uint64_t bytesLeidosEnTotal = 0;
	char *codigoTotal;
	while(!feof(archivo))
	{
		char *codigo = malloc(TAMLIN);
		size_t tamanioleido = fread(codigo,1,TAMLIN,archivo);
		codigoTotal = realloc(codigoTotal, bytesLeidosEnTotal + tamanioleido);
		memcpy(codigoTotal + bytesLeidosEnTotal, codigo, tamanioleido);
		bytesLeidosEnTotal += tamanioleido;
		free(codigo);
	}

	enviarTodo(conectarANodo, codigoTotal, bytesLeidosEnTotal);
	free(codigoTotal);

	logCabeceraEnvia("Contenido de rutina");
	fclose(archivo);
}


