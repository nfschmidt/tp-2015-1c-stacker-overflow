#include "atenderAJob.h"
#include "generadorNombreArchivo.h"
#include "ArchivoRemoto.h"
#include "ArchivoLocal.h"
#include "atenderConexiones.h"
#include "mapReduce.h"
#include "configNodo.h"
#include "ejecucionDeRutina.h"
#include "subidaArchivoMDFS.h"

#include "espacioDatos.h"
#include "bloqueDatos.h"
#include "logueo.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <commons/string.h>
#include <commons/collections/list.h>

#include <libsocket.h>


#define SEPARADOR_ARCHIVOS "|"
#define SEPARADOR_CAMPOS_ARCHIVO ":"

static Tarea *ejecutarTareaYNotificarResultado(SocketConecta *conexion, EspacioDatos *espacioDeDatos);
static Tarea *obtenerTarea(SocketConecta *conexion);
static char *recibirArchivoRutina(SocketConecta *conexion);
static char *recibirTipoRutina(SocketConecta *conexion);
static ssize_t recibirYVerificarConexion(SocketConecta *conexion, void *buffer, size_t longitudBuffer);
static void terminarEjecucionDeTarea();
static int ejecutarTarea(Tarea *tarea, EspacioDatos *espacioDeDatos);
static int esMap(Tarea *tarea);
static int recibirBloque(SocketConecta *conexion);
static t_list *recibirListaArchivosRemotos(SocketConecta *conexion);
static t_list *convertirMensajeAListaArchivosRemotos(char *mensaje);
static char *recibirNombreArchivoResultado(SocketConecta *conexion);
static Archivo *obtenerArchivoSalidaRutina(char *nombreArchivoResultado);
static void notificarResultado(Tarea *tarea, SocketConecta *conexion, int resultadoDeTarea);
static char *obtenerPathDeArchivoEnEspacioTemporal(char *nombreArchivo);
static void deleteTarea(Tarea *tarea);
static int esMiIpYPuerto(char *ip, char *puerto);

static void subirArchivoResultadoAMDFS(SocketConecta *conexionConJob, char *nombreArchivo);

void *ejecutarTareaDeJob(void *argEspacioYConexion)
{
	EspacioYConexion *espacioYCoexion = argEspacioYConexion;
	SocketConecta *conexion = espacioYCoexion->conexion;
	EspacioDatos *espacioDeDatos = espacioYCoexion->espacioDeDatos;
	free(espacioYCoexion);

	Tarea *tarea = ejecutarTareaYNotificarResultado(conexion, espacioDeDatos);

	deleteSocketConecta(conexion);

	deleteTarea(tarea);

	return 0;
}

void *ejecutarUltimaTareaDeJob(void *argEspacioYConexion)
{
	EspacioYConexion *espacioYCoexion = argEspacioYConexion;
	SocketConecta *conexion = espacioYCoexion->conexion;
	EspacioDatos *espacioDeDatos = espacioYCoexion->espacioDeDatos;
	free(espacioYCoexion);

	Tarea *tarea = ejecutarTareaYNotificarResultado(conexion, espacioDeDatos);

	if (tarea->resultado == RUTINA_OK)
		subirArchivoResultadoAMDFS(conexion, tarea->nombreArchivoResultado);

	deleteSocketConecta(conexion);

	deleteTarea(tarea);

	return 0;
}

static Tarea *ejecutarTareaYNotificarResultado(SocketConecta *conexion, EspacioDatos *espacioDeDatos)
{
	char *ipRemoto = obtenerIPRemoto(conexion);
	loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Se conecto un job. IP: %s", ipRemoto);
	free(ipRemoto);

	Tarea *tarea = obtenerTarea(conexion);

	tarea->resultado = ejecutarTarea(tarea, espacioDeDatos);

	notificarResultado(tarea, conexion, tarea->resultado);

	return tarea;
}

static void subirArchivoResultadoAMDFS(SocketConecta *conexionConJob, char *nombreArchivo)
{
	int sizePathArchivoEnMDFS;
	recibirMensaje(conexionConJob, &sizePathArchivoEnMDFS, sizeof(int));

	char *pathArchivoEnMDFS = malloc(sizePathArchivoEnMDFS + 1);
	recibirMensaje(conexionConJob, pathArchivoEnMDFS, sizePathArchivoEnMDFS);
	pathArchivoEnMDFS[sizePathArchivoEnMDFS] = '\0';

	int resultadoDeSubida = conectarAFilesystemYSubirArchivo(nombreArchivo, pathArchivoEnMDFS);

	enviarMensaje(conexionConJob, &resultadoDeSubida, sizeof(int));
}

static Tarea *obtenerTarea(SocketConecta *conexion)
{
	Tarea *tarea = malloc(sizeof(Tarea));

	tarea->pathRutina = recibirArchivoRutina(conexion);
	tarea->tipoRutina = recibirTipoRutina(conexion);

	if (esMap(tarea))
	{
		tarea->bloque = recibirBloque(conexion);
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "Se recibio tipo rutina: %s", "Map");
	}
	else
	{
		tarea->listaArchivosRemotos = recibirListaArchivosRemotos(conexion);
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "Se recibio tipo rutina: %s", "Reduce");
	}

	tarea->nombreArchivoResultado = recibirNombreArchivoResultado(conexion);

	return tarea;
}

static char *recibirArchivoRutina(SocketConecta *conexion)
{
	char *pathDeRutina = obtenerNombreArchivoResultadoTemporal(TIPO_EJECUTABLE);
	FILE *archivoRutina = fopen(pathDeRutina, "w");

	char *buffer;
	uint64_t bytesRecibidos = recibirTodo(conexion, (void **)&buffer);
	fwrite(buffer, 1, bytesRecibidos, archivoRutina);

	free(buffer);
	fflush(archivoRutina);
	fclose(archivoRutina);

	int permisosDeEjecucion = strtol("0700", 0, 8);
	chmod(pathDeRutina, permisosDeEjecucion);

	loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "Se recibio una rutina. Guardada en: %s", pathDeRutina);

	return pathDeRutina;
}

static ssize_t recibirYVerificarConexion(SocketConecta *conexion, void *buffer, size_t longitudBuffer)
{
	if (!estaConectado(conexion))
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "Se perdio la conexion con el job");
		terminarEjecucionDeTarea();
	}

	ssize_t bytesRecibidos = recibirMensaje(conexion, buffer, longitudBuffer);

	if (!estaConectado(conexion))
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "Se perdio la conexion con el job");
		terminarEjecucionDeTarea();
	}

	return bytesRecibidos;
}

static void terminarEjecucionDeTarea()
{
	pthread_cancel(pthread_self());
}

static int ejecutarTarea(Tarea *tarea, EspacioDatos *espacioDeDatos)
{
	int resultado;
	Archivo *archivoSalidaRutina = obtenerArchivoSalidaRutina(tarea->nombreArchivoResultado);

	if (esMap(tarea))
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Ejecutando rutina de map\n\tBloque : %d\n\tRutina: %s\n\tSalida: %s",
				tarea->bloque, tarea->pathRutina, tarea->nombreArchivoResultado);
		BloqueDatos *bloque = getBloque(espacioDeDatos, tarea->bloque);
		resultado = ejecutarRutinaDeMap(tarea->pathRutina, bloque, archivoSalidaRutina);
		free(bloque);
	}
	else
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Ejecutando rutina de reduce. Salida: %s", tarea->nombreArchivoResultado);
		resultado = ejecutarRutinaDeReduce(tarea->pathRutina, tarea->listaArchivosRemotos, archivoSalidaRutina);
	}

	cerrarArchivo(archivoSalidaRutina);

	return resultado;
}

static char *recibirTipoRutina(SocketConecta *conexion)
{
	char *buffer = calloc(7, sizeof(char));
	int sizeMensaje;

	recibirYVerificarConexion(conexion, &sizeMensaje, sizeof(int));
	recibirYVerificarConexion(conexion, buffer, sizeMensaje);

	return buffer;
}

static int esMap(Tarea *tarea)
{
	return strcmp(tarea->tipoRutina, "map") == 0;
}

static int recibirBloque(SocketConecta *conexion)
{
	int numeroDeBloque;
	recibirYVerificarConexion(conexion, &numeroDeBloque, sizeof(int));

	return numeroDeBloque;
}

static t_list *recibirListaArchivosRemotos(SocketConecta *conexion)
{
	char *mensaje;
	recibirTodo(conexion, (void**)&mensaje);

	printf("Lista archivos remotos: %s\n", mensaje); fflush(stdout);

	t_list *listaArchivosRemotos = convertirMensajeAListaArchivosRemotos(mensaje);
	free(mensaje);

	return listaArchivosRemotos;
}

static t_list *convertirMensajeAListaArchivosRemotos(char *mensaje)
{
	t_list *archivosRemotos = list_create();
	char **datosArchivosRemotos = string_split(mensaje, SEPARADOR_ARCHIVOS);

	int i;
	for (i = 0; datosArchivosRemotos[i] != NULL; i ++)
	{
		char **datosArchivo = string_split(datosArchivosRemotos[i], SEPARADOR_CAMPOS_ARCHIVO);
		Archivo *archivoAReducir;

		if (esMiIpYPuerto(datosArchivo[0], datosArchivo[1]))
		{
			char *pathArchivo = obtenerPathDeArchivoEnEspacioTemporal(datosArchivo[2]);
			archivoAReducir = (Archivo *) newArchivoLocal(pathArchivo, LECTURA);
			free(pathArchivo);
		}
		else
		{
			SocketConecta *conexion = newSocketConecta(datosArchivo[0], datosArchivo[1]);
			conectarAServidor(conexion);
			archivoAReducir = (Archivo*) newArchivoRemoto(datosArchivo[2], conexion);
		}

		list_add(archivosRemotos, archivoAReducir);
		free(datosArchivo);
	}

	free(datosArchivosRemotos);
	return archivosRemotos;
}

static char *recibirNombreArchivoResultado(SocketConecta *conexion)
{
	int sizeNombreArchivo;

	recibirYVerificarConexion(conexion, &sizeNombreArchivo, sizeof(int));


	char *nombreArchivo = malloc((sizeNombreArchivo + 1) * sizeof(char));

	recibirYVerificarConexion(conexion, nombreArchivo, sizeNombreArchivo);
	nombreArchivo[sizeNombreArchivo] = '\0';

	return nombreArchivo;
}

static Archivo *obtenerArchivoSalidaRutina(char *nombreArchivoResultado)
{
	return (Archivo *)newArchivoLocal(
			obtenerPathDeArchivoEnEspacioTemporal(nombreArchivoResultado),
			ESCRITURA);
}

static char *obtenerPathDeArchivoEnEspacioTemporal(char *nombreArchivo)
{
	char *pathEspacioTemporal = obtenerPathEspacioTemporal();
	char *pathArchivo = malloc(strlen(pathEspacioTemporal) + strlen(nombreArchivo) + 1);

	strcpy(pathArchivo, pathEspacioTemporal);
	strcat(pathArchivo, nombreArchivo);

	return pathArchivo;
}

static void notificarResultado(Tarea *tarea, SocketConecta *conexion, int resultadoDeTarea)
{
	enviarMensaje(conexion, &resultadoDeTarea, 4);

	if (esMap(tarea))
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Ejecucion de map terminada.\n\tBloque : %d\n\tRutina: %s\n\tSalida: %s\n\tResultado: %d",
				tarea->bloque,
				tarea->pathRutina,
				tarea->nombreArchivoResultado,
				resultadoDeTarea);
	}
	else
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Ejecucion de reduce terminada.\n\tRutina: %s\n\tSalida: %s\n\tResultado: %d",
						tarea->pathRutina, tarea->nombreArchivoResultado, resultadoDeTarea);
	}
}

static int esMiIpYPuerto(char *ip, char *puerto)
{
	ConfiguracionNodo *config = obtenerConfiguracionNodo();
	if (strcmp(puerto, config->puerto) != 0)
		return 0;

	int tengoEsaIp = 0;
	struct ifaddrs *addrs, *tmp;
	getifaddrs(&addrs);
	tmp = addrs;

	while (tmp)
	{
	    if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
	    {
	        struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
	        if(strcmp(ip, inet_ntoa(pAddr->sin_addr)) == 0)
	        	tengoEsaIp = 1;
	    }

	    tmp = tmp->ifa_next;
	}

	freeifaddrs(addrs);
	return tengoEsaIp;
}

static void deleteTarea(Tarea *tarea)
{
	free(tarea->nombreArchivoResultado);
	remove(tarea->pathRutina);
	free(tarea->pathRutina);
	free(tarea->tipoRutina);
	free(tarea);
}
