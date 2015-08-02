#include "protocoloFileSystem.h"

#include "logueo.h"
#include "nodo.h"
#include <stdlib.h>
#include <string.h>

#define MENSAJE_SOY_MARTA 'M'
#define MENSAJE_FS_OPERATIVO 'S'
#define MENSAJE_FS_NO_OPERATIVO 'N'

#define MENSAJE_ESTADO_ARCHIVO 'E'
#define MENSAJE_BLOQUES_ARCHIVO 'B'
#define MENSAJE_DETALLE_BLOQUE 'D'

#define MENSAJE_ARCHIVO_NO_DISPONIBLE 'N'
#define MENSAJE_ARCHIVO_SI_DISPONIBLE 'S'

#define MENSAJE_HAY_OTRO_BLOQUE 'O'
#define MENSAJE_NO_HAY_OTRO_BLOQUE 'P'


static SocketConecta *_conexionFS;

static void enviarPathArchivo(char *pathMDFS);


void establecerConexionConFileSystem(char *ipFileSystem, char *puertoFileSystem)
{
	loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Iniciando conexion con FileSystem en %s:%s", ipFileSystem, puertoFileSystem);

	_conexionFS = newSocketConecta(ipFileSystem, puertoFileSystem);
	conectarAServidor(_conexionFS);

	if(estaConectado(_conexionFS))
	{
		char soyMarta = MENSAJE_SOY_MARTA;

		enviarMensaje(_conexionFS, &soyMarta, sizeof(char));

		char estaOperativo;
		recibirMensaje(_conexionFS, &estaOperativo,sizeof(char));

		if (estaOperativo == MENSAJE_FS_OPERATIVO) {
			loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"FileSystem Operativo");
		}
		else
		{
			loguearYMostrarEnPantalla(LOG_LEVEL_INFO,"FileSystem NO operativo. Se termino la conexion con el filesystem");
			loguearYMostrarEnPantalla(LOG_LEVEL_INFO,"Se termina el proceso");
			exit(1);
		}
	}
	else
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO,"No se pudo conectar al FileSystem. Se termina el proceso");
		exit(1);
	}
}


int consultarArchivoDisponible(char *pathMDFS)
{
	char tipoPedido = MENSAJE_ESTADO_ARCHIVO;
	enviarMensaje(_conexionFS, &tipoPedido, 1);

	enviarPathArchivo(pathMDFS);

	char respuesta;
	recibirMensaje(_conexionFS, &respuesta, 1);

	return respuesta == MENSAJE_ARCHIVO_SI_DISPONIBLE;
}

int consultarCantidadBloquesArchivo(char *pathMDFS)
{
	char tipoPedido = MENSAJE_BLOQUES_ARCHIVO;
	enviarMensaje(_conexionFS, &tipoPedido, 1);

	enviarPathArchivo(pathMDFS);

	int cantidadDeBloques;
	recibirMensaje(_conexionFS, &cantidadDeBloques, sizeof(int));

	return cantidadDeBloques;
}

t_list *consultarDetallesBloqueArchivo(char *pathMDFS, int bloque)
{
	char tipoPedido = MENSAJE_DETALLE_BLOQUE;
	enviarMensaje(_conexionFS, &tipoPedido, 1);

	enviarPathArchivo(pathMDFS);

	enviarMensaje(_conexionFS, &bloque, sizeof(int));


	t_list *listaDeNodosConBloque = list_create();

	char hayOtroBloque;
	recibirMensaje(_conexionFS, &hayOtroBloque, 1);
	while (hayOtroBloque == MENSAJE_HAY_OTRO_BLOQUE)
	{
		int longitudMensaje;

		recibirMensaje(_conexionFS, &longitudMensaje, sizeof(int));
		char *nombreNodo = malloc(longitudMensaje + 1);
		recibirMensaje(_conexionFS, nombreNodo, longitudMensaje);
		nombreNodo[longitudMensaje] = '\0';

		recibirMensaje(_conexionFS, &longitudMensaje, sizeof(int));
		char *ip = malloc(longitudMensaje + 1);
		recibirMensaje(_conexionFS, ip, longitudMensaje);
		ip[longitudMensaje] = '\0';

		recibirMensaje(_conexionFS, &longitudMensaje, sizeof(int));
		char *puerto = malloc(longitudMensaje + 1);
		recibirMensaje(_conexionFS, puerto, longitudMensaje);
		puerto[longitudMensaje] = '\0';

		int bloqueEnNodo;
		recibirMensaje(_conexionFS, &bloqueEnNodo, sizeof(int));

		t_nodo *nodo = newNodo(nombreNodo, ip, puerto);
		nodoSetBloque(nodo, bloqueEnNodo);
		list_add(listaDeNodosConBloque, nodo);

		recibirMensaje(_conexionFS, &hayOtroBloque, 1);
	}

	return listaDeNodosConBloque;
}

static void enviarPathArchivo(char *pathMDFS)
{
	int longitudPath = strlen(pathMDFS) + 1;
	enviarMensaje(_conexionFS, &longitudPath, sizeof(int));

	enviarMensaje(_conexionFS, pathMDFS, longitudPath);
}
