#include <libsocket.h>
#include <stdio.h>
#include <stdlib.h>
#include "estructuraFS.h"
#include "nodo.h"
#include <string.h>
#include "estadoFileSystem.h"
#include "configFileSystem.h"
#include "inicializarFS.h"
#include "atenderANodo.h"
#include "atenderConexiones.h"
#include "logueo.h"
#include <pthread.h>


#define DAME_INFORMACION 'D'
#define CANTIDAD_BLOQUES 'C'
#define NOMBRE_EXISTENTE "E"
#define SIGUIENTE_REGISTRO 'S'
#define NODO_INEXISTENTE "X"
#define BLOQUEAR_ATENDER_NODO 'T'

static void *avisarNodoExisteYTerminarConexion(SocketConecta *conexion, char *puertoNodo, char *nombreNodo, char *ipNodo);
static t_nodo *obtenerNodoReConectado(int posEnListaConectado, SocketConecta *nuevaConexion, char *ipNodo, char *puertoNodo);
static off_t recibirYVerificarConexion(SocketConecta *conexion, void *buffer, off_t longitudBuffer);
static void *avisarNodoInexisteYTerminarConexion(SocketConecta *conexion,char *puertoNodo, char *nombreNodo, char *ipNodo);
static void terminarEjecucionDeTarea();
static void liberarPuertoYNombreNodo(char *nombre, char *puerto, char *ipNodo);

t_nodo *crearNodo(SocketConecta *conexion)
{
	char *nombreNodo, *puertoNodo;
	size_t tamanioNombre = 0,tamanioPuerto = 0;
	int cantidadBloquesDisponibles = 0;
	int existeNombre = 0;
	int nodoNuevo;
	t_nodo *nodo;

	recibirYVerificarConexion(conexion,&cantidadBloquesDisponibles,sizeof(int));

	recibirYVerificarConexion(conexion, &nodoNuevo,sizeof(int));

	recibirYVerificarConexion(conexion,&tamanioNombre,sizeof(size_t));

	nombreNodo = malloc(tamanioNombre +1);
	recibirYVerificarConexion(conexion,nombreNodo,tamanioNombre);

	nombreNodo[tamanioNombre] = '\0';

	existeNombre = validarExistenciaNombre(nombreNodo);

	recibirYVerificarConexion(conexion,&tamanioPuerto,sizeof(size_t));

	puertoNodo = malloc(tamanioPuerto +1);
	recibirYVerificarConexion(conexion,puertoNodo,tamanioPuerto);
	puertoNodo[tamanioPuerto] = '\0';

	char *ipNodo = obtenerIPRemoto(conexion);
	if(existeNombre < 0  && nodoNuevo)
		nodo = agregarNodoConectado(ipNodo,puertoNodo,nombreNodo,cantidadBloquesDisponibles,conexion);
	else if((existeNombre >= 0) && nodoNuevo)
		return avisarNodoExisteYTerminarConexion(conexion,puertoNodo,nombreNodo,ipNodo);
	else if(existeNombre >= 0 && !nodoNuevo)
		nodo = obtenerNodoReConectado(existeNombre, conexion, ipNodo,puertoNodo);
	else
		return avisarNodoInexisteYTerminarConexion(conexion,puertoNodo,nombreNodo,ipNodo);

	liberarPuertoYNombreNodo(nombreNodo,puertoNodo,ipNodo);

	return nodo;
}

static void liberarPuertoYNombreNodo(char *nombre, char *puerto,char *ipNodo)
{
	free(nombre);
	free(puerto);
	free(ipNodo);

}

static void *avisarNodoExisteYTerminarConexion(SocketConecta *conexion, char *puertoNodo, char *nombreNodo, char *ipNodo)
{
	free(puertoNodo);
	free(nombreNodo);
	free(ipNodo);
	enviarMensaje(conexion,NOMBRE_EXISTENTE,sizeof(char));
	loguear(LOG_LEVEL_INFO,"Un nodo intento conectarse pero ya existe un nombre con ese nodo y se lo desconecto");
	deleteSocketConecta(conexion);
	return NULL;
}

static void *avisarNodoInexisteYTerminarConexion(SocketConecta *conexion, char *puertoNodo, char *nombreNodo, char *ipNodo)
{
	free(puertoNodo);
	free(nombreNodo);
	free(ipNodo);
	enviarMensaje(conexion,NODO_INEXISTENTE,sizeof(char));
	loguear(LOG_LEVEL_INFO,"Un nodo intento conectarse como existente, pero no existe y se lo desconecto");
	deleteSocketConecta(conexion);
	return NULL;
}


void *atenderANodo(void *argConexion)
{
	SocketConecta *conexion = argConexion;


	while(estaConectado(conexion))
	{

		char nombrePaquete;
		t_nodo *nodo;
		recibirMensaje(conexion, &nombrePaquete,1);
		if(estaConectado(conexion))
		{
			if (nombrePaquete == CANTIDAD_BLOQUES)
				nodo = crearNodo(conexion);
			else if (nombrePaquete == BLOQUEAR_ATENDER_NODO){
				reservarConexionNodo(nodo);
				devolverConexionNodo(nodo);
			}
		}else
		{
			deshabilitarNodoPorDesconexion(nodo);
			loguear(LOG_LEVEL_INFO,"Se desconecto el nodo %s\n",nodo->nombre);
			cerrarSocketConecta(conexion);

		}
	}
	return 0;
}


int validarExistenciaNombre(char *nombreNodo)
{
	reservarListaNodosConectados();
	int contador = 0;
	t_nodo *nodo;
	while(contador < list_size(listaNodosConectados))
	{
		nodo = list_get(listaNodosConectados,contador);

		if(strcmp(nodo->nombre, nombreNodo) == 0)
			{
				devolverListaNodosConectados();
				return contador;
			}else
			{
			 contador++;
			}
	}
	devolverListaNodosConectados();
	return -1;
}

static t_nodo *obtenerNodoReConectado(int posEnListaConectado,SocketConecta *nuevaConexion, char *ipNodo, char *puertoNodo)
{
	t_nodo *nodo;
	reservarListaNodosConectados();
	nodo = list_get(listaNodosConectados,posEnListaConectado);
	nodo->conexionNodoServidor = nuevaConexion;
	nodo->ip = strdup(ipNodo);
	nodo->puerto = strdup(puertoNodo);
	pthread_mutex_init(&(nodo->mutex_conexionNodo),NULL);
	devolverListaNodosConectados();
	loguear(LOG_LEVEL_INFO, "Se reconecto el nodo %s",nodo->nombre);
	deshabilitarNodo(nodo);
	return nodo;
}

static off_t recibirYVerificarConexion(SocketConecta *conexion, void *buffer, off_t longitudBuffer)
{
	if (!estaConectado(conexion))
	{
		loguear(LOG_LEVEL_DEBUG, "Se perdio la conexion con el nodo");
		terminarEjecucionDeTarea();
	}

	ssize_t bytesRecibidos = recibirMensaje(conexion, buffer, longitudBuffer);

	if (!estaConectado(conexion))
	{
		loguear(LOG_LEVEL_DEBUG, "Se perdio la conexion con el nodo");
		terminarEjecucionDeTarea();
	}

	return bytesRecibidos;
}

static void terminarEjecucionDeTarea()
{
	pthread_cancel(pthread_self());
}
