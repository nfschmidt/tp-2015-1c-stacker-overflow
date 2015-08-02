#include  "nodo.h"
#include "string.h"
#include "responderANodo.h"
#include <stdio.h>
#include <stdlib.h>
#include "estructuraFS.h"
#include "libsocket.h"
#include "atenderConexiones.h"
#include "enviarBloqueANodos.h"
#include "logueo.h"

#define TAMANIO_MAXIMO 1024
#define GRABAR_TEXTO "G"
#define TAMANIO_BLOQUE	"B"

static void marcarPosicionComoOcupadaEnArray(int *bloques, int posicionAOcupar);

int grabarBloques(t_nodo *nodo, char *bloque,t_bloque *bloqueCopia)
{
	reservarConexionNodo(nodo);
	loguear(LOG_LEVEL_INFO,"Solicito al nodo %s grabar el bloque %d  del archivo en su bloque %d",nodo->nombre,bloqueCopia->numeroDeBloqueDelArchivo,bloqueCopia->numeroDeBloqueEnNodo);
	if (!enviarTamanioBloque(nodo->conexionNodoServidor,bloque))
	{
		devolverConexionNodo(nodo);
		return 0;
	}
	if (!enviarBloque(nodo,bloque,bloqueCopia,nodo->conexionNodoServidor))
	{
		devolverConexionNodo(nodo);
		return 0;
	}
	loguear(LOG_LEVEL_INFO,"Se grabo en el nodo %s el bloque %d  del archivo en su bloque %d",nodo->nombre,bloqueCopia->numeroDeBloqueDelArchivo,bloqueCopia->numeroDeBloqueEnNodo);
	devolverConexionNodo(nodo);
	return 1;

}

int enviarTamanioBloque(SocketConecta *conexion,char *bloque)
{

	off_t tamanioBloque = tamanioDelBloque(bloque);
	char *mensajeNodo = malloc(sizeof(char) + sizeof(off_t));
	memcpy(mensajeNodo,TAMANIO_BLOQUE,1);
	memcpy(mensajeNodo + 1,&tamanioBloque,sizeof(off_t));
	if(estaConectado(conexion))
	{
		enviarMensaje(conexion,mensajeNodo,sizeof(char) + sizeof(off_t));
		free(mensajeNodo);
		return 1;
	}
	else
	{
		free(mensajeNodo);
		return 0;
	}
}

int enviarBloque(t_nodo *nodo, char *bloque, t_bloque *copiaBloque,SocketConecta *conexion)
{
	off_t bytesEnviados = 0;
	off_t bytesPorEnviar;
	off_t retorno;
	char respuestaNodo;
	off_t tamanio = tamanioDelBloque(bloque);
	marcarPosicionComoOcupadaEnArray(nodo->bloques,posicionLibreEnNodo(nodo->bloques));
	if(estaConectado(conexion))
		enviarMensaje(conexion,GRABAR_TEXTO,1);
	if (estaConectado(conexion))
		enviarMensaje(conexion,&copiaBloque->numeroDeBloqueEnNodo,sizeof(int));
	bytesPorEnviar = tamanio;
	while(bytesEnviados < bytesPorEnviar)
	{
		if(estaConectado(conexion))
		{
			retorno = enviarMensaje(conexion,(char*)(bloque + bytesEnviados),(bytesPorEnviar - bytesEnviados));

			if(retorno <= 0)
				break;
			bytesEnviados += retorno;
		}else
			return 0;
	}

	recibirMensaje(conexion,&respuestaNodo,sizeof(char));
	if (!estaConectado(conexion))
		return 0;
	nodo->bloquesUsados++;
	return 1;
}


off_t tamanioDelBloque(char *bloque)
{
	off_t tamanio = strlen(bloque);
	return tamanio;
}

static void marcarPosicionComoOcupadaEnArray(int *bloques, int posicionAOcupar)
{
	bloques[posicionAOcupar] = 1;
}
