#include "estructuraFS.h"
#include  <stdlib.h>
#include  <string.h>
#include  <stdio.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "nodo.h"
#include "enviarBloqueANodos.h"
#include "estadoFileSystem.h"
#include "logueo.h"
#include "asignarBloque.h"
#include "conexionMongo.h"

#define ERROR_FILESYSTEM -1
#define OPERACION_OK_FILESYSTEM 1
#define QUIERO_BLOQUE "Q"
#define SOY_CONSOLA "F"


static off_t recibirYVerificarConexion(SocketConecta *conexion, void *buffer, off_t longitudBuffer);
static void marcarPosicionComoLibreEnArray(int *bloques,int posicionALiberar);
static void verSiAgregarBloqueALaLista(t_list *listaAux, t_bloque *bloque);
static bool compararNumeroBloquesArchivo(void *bloque1arg, void *bloque2arg);
static void limpiarListaNodosConectados();
static void eliminarArchivoDelDirecPadre(t_elementoFS *hijoElemento,char *ruta);

t_elementoFS *obtenerSubDirectorio(char *nombreDirectorio, t_elementoFS *directorioActual);
t_elementoFS *buscarElementoConRutaParseada(char **rutaParseada, t_elementoFS *elementoFS);

t_elementoFS *newElementoFS(char *nombre, t_elemento tipo)
{

	t_elementoFS *elementoFS = malloc(sizeof(t_elementoFS));
	elementoFS->nombre = strdup(nombre);
	if(tipo == DIRECTORIO)
		{
		elementoFS->tipo = tipo;
		elementoFS->listaDirectorios = list_create();
		elementoFS->cantidadBloques = -1;
		}else
			{
			if(tipo == ARCHIVO)
				{
				elementoFS->tipo = tipo;
				elementoFS->cantidadBloques = -1;
				elementoFS->listaCopias = list_create();
				elementoFS->listaDirectorios = list_create();
				}
			}
				return elementoFS;
}

void inicializarDirectoriosFS()
{
	if(hayDatosEnMongo())
	{
		recuperarDatosPersistidos();
		leerNodosDeMongo();
	}
	else
		root = newElementoFS("/",DIRECTORIO);
}


void imprimirArbol(t_elementoFS *elementoFS, int nivel)
{	int x =0;
	for(x = 0; x < nivel; x++ )
		printf("	");

	if (nivel != 0)
		printf("/");


	if (list_size(elementoFS->listaDirectorios) == 0 && esDirectorio(elementoFS) && nivel != 0)
		printf("%s/\n",elementoFS->nombre);
	else
		printf("%s\n",elementoFS->nombre);
	int contador = 0;
	while(contador < list_size(elementoFS->listaDirectorios))
	{
		t_elementoFS *directorioAux = list_get(elementoFS->listaDirectorios,contador);
		imprimirArbol(directorioAux, nivel+1);
		contador++;
	}

}

int agregarElementoASubDirectorio(t_elementoFS *elementoFS,t_elementoFS *nuevoElemento)
{
	int contador = 0;
	int contadorNombre = 0;
	if(esDirectorio(elementoFS))

		while(contador < list_size(elementoFS->listaDirectorios))
		{
			t_elementoFS *directorioAux = list_get(elementoFS->listaDirectorios,contador);
			if(tienenMismoNombre(directorioAux, nuevoElemento) == 1)
				contadorNombre++;

			contador++;
		}
	if(contadorNombre == 0)
		{
			list_add(elementoFS->listaDirectorios,nuevoElemento);
			return OPERACION_OK_FILESYSTEM;
		}else
		{
			return ERROR_FILESYSTEM;
		}
	return OPERACION_OK_FILESYSTEM;
}

int tienenMismoNombre(t_elementoFS *elementoFS, t_elementoFS *otroElementoFS)
{
	if(strcmp(elementoFS->nombre, otroElementoFS->nombre) == 0)
		return OPERACION_OK_FILESYSTEM;
	else
		return ERROR_FILESYSTEM;
}

int esDirectorio(t_elementoFS *elementoFS)
{
	  return elementoFS->tipo == DIRECTORIO;
}


int esArchivo(t_elementoFS *elementoFS)
{
	return elementoFS->tipo == ARCHIVO;
}


t_elementoFS *buscarElemento(char *ruta,t_elementoFS *elementoFS)
{
		if((strcmp(ruta,"/") == 0))
		{
			return root;
		}else
		{
		char **rutaParseada = string_split(ruta,"/");
		return buscarElementoConRutaParseada(rutaParseada,elementoFS);
		}
}

t_elementoFS *buscarElementoConRutaParseada(char **rutaParseada, t_elementoFS *elementoFS)
{
	t_elementoFS *directorio = elementoFS;
	int i = 0;
	while(rutaParseada[i] != NULL)
		{
			directorio = obtenerSubDirectorio(rutaParseada[i],directorio);
			i++;
			if(directorio == NULL)
				return NULL;
		}
		return directorio;
}

t_elementoFS *obtenerSubDirectorio(char *nombreDirectorio, t_elementoFS *directorioActual)
{	int contador = 0;
	while(contador < list_size(directorioActual->listaDirectorios))
	{
		t_elementoFS *directorioAux = list_get(directorioActual->listaDirectorios,contador);
		if(strcmp(nombreDirectorio,directorioAux->nombre) == 0)
			return directorioAux;
		contador++;
	}
	return NULL;
}


int crearDirectorio(char *ruta, char *nombre,t_elementoFS *elementoFS)
{
	t_elementoFS *archivo = buscarElemento(ruta,elementoFS);
	if (archivo == NULL)
		return ERROR_FILESYSTEM;
	return agregarElementoASubDirectorio(archivo,newElementoFS(nombre,DIRECTORIO));
}


t_elementoFS *crearArchivo(char *ruta, char *nombre,t_elementoFS *elementoFS)
{
	t_elementoFS *archivoPadre = buscarElemento(ruta,elementoFS);
	if (archivoPadre == NULL)
		return NULL;

	t_elementoFS *archivo;
	archivo = newElementoFS(nombre,ARCHIVO);
	agregarElementoASubDirectorio(archivoPadre,archivo);

	return archivo;
}

int existeArchivoQueQuieroAgregar(char *rutaMDFS, char *nombreArchivo)
{
	char * rutaCompleta = malloc(strlen(rutaMDFS) + strlen(nombreArchivo) + 2);
	rutaCompleta[0] = '\0';
	strcat(rutaCompleta , rutaMDFS);
	if (strcmp(rutaMDFS,"/") != 0)
		strcat(rutaCompleta , "/");
	strcat(rutaCompleta, nombreArchivo);
	int existeArch = buscarElemento(rutaCompleta,root) != NULL;
	free(rutaCompleta);
	return existeArch;
}


int renombrarDirectorio(char *ruta, char *nuevoNombre,t_elementoFS *elementoFS)
{
	t_elementoFS *archivo = buscarElemento(ruta,elementoFS);
	if (archivo == NULL)
		return ERROR_FILESYSTEM;
	if(archivo->tipo == DIRECTORIO)
		archivo->nombre = strdup(nuevoNombre);
	else
			return ERROR_FILESYSTEM;

	return OPERACION_OK_FILESYSTEM;
}

int renombrarArchivo(char *ruta, char *nuevoNombre,t_elementoFS *elementoFS)
{
	t_elementoFS *archivo = buscarElemento(ruta,elementoFS);
	if (archivo == NULL)
		return ERROR_FILESYSTEM;
	if(archivo->tipo == ARCHIVO)
		archivo->nombre = strdup(nuevoNombre);
	else
		return ERROR_FILESYSTEM;

	return OPERACION_OK_FILESYSTEM;
}

int moverElemento(char *rutaOrigen, char *rutaDestino, t_elementoFS *elementoFS)
{
	t_elementoFS *archivoDestino = buscarElemento(rutaDestino,elementoFS);
	if (archivoDestino == NULL)
		return ERROR_FILESYSTEM;
	t_elementoFS *elementoAEliminar = elementoARemover(rutaOrigen,elementoFS);
	t_elementoFS *elementoDestino = archivoDestino;
	agregarElementoASubDirectorio(elementoDestino,elementoAEliminar);
	return 1;
}

int moverDirectorio(char *rutaOrigen, char *rutaDestino, t_elementoFS *elementoFS)
{
	t_elementoFS *elementoOrigen = buscarElemento(rutaOrigen,elementoFS);
	if (elementoOrigen == NULL)
		return ERROR_FILESYSTEM;
	if(elementoOrigen->tipo == DIRECTORIO)
		{
			return moverElemento(rutaOrigen,rutaDestino,elementoFS);

		}
	else{
		return ERROR_FILESYSTEM;
		}

	return OPERACION_OK_FILESYSTEM;

}

int moverArchivo(char *rutaOrigen, char *rutaDestino, t_elementoFS *elementoFS)
{
	t_elementoFS *elementoOrigen = buscarElemento(rutaOrigen,elementoFS);
	if (elementoOrigen == NULL)
		return ERROR_FILESYSTEM;
	if(elementoOrigen->tipo == ARCHIVO)
		{
		return moverElemento(rutaOrigen,rutaDestino,elementoFS);
		}
	else{
		return ERROR_FILESYSTEM;
		}
	return OPERACION_OK_FILESYSTEM;

}

int buscarIndiceElemento(t_elementoFS *elementoPadre, t_elementoFS *elementoFS)
{
	int i = 0;
	while(i < list_size(elementoPadre->listaDirectorios))
	{
		t_elementoFS *directorioAux = list_get(elementoPadre->listaDirectorios,i);
		if(strcmp(directorioAux->nombre,elementoFS->nombre) == 0)
			return i;

		i++;

	}
		return ERROR_FILESYSTEM;

}


int eliminarArchivo(char *ruta,t_elementoFS *elementoFS)
{
	t_bloque *bloque;
	t_elementoFS *archivo = buscarElemento(ruta,elementoFS);

	int contador = 0;
	if (archivo == NULL)
		return ERROR_FILESYSTEM;
	if(esArchivo(archivo))
	{
		t_elementoFS *padre = buscarPadre(ruta,root);
		while(contador < list_size(padre->listaDirectorios))
		{
			t_elementoFS *archivoAEliminar = list_get(padre->listaDirectorios,contador);
			if(archivo == archivoAEliminar)
				list_remove(padre->listaDirectorios,contador);

			contador++;
		}


		while(list_size(archivo->listaCopias) > 0)
		{
			bloque = list_get(archivo->listaCopias, 0);
			liberarPosicionEnNodo(bloque->nombreNodo,bloque->numeroDeBloqueEnNodo);
			free(bloque);
			list_remove(archivo->listaCopias, 0);
		}

		list_destroy(archivo->listaCopias);
		free(archivo);
		return 1;

	}else
		return 0;
}


int eliminarDirectorio(char *ruta,t_elementoFS *elementoFS)
{
	t_elementoFS *directorio = buscarElemento(ruta,elementoFS);


	if(directorio != NULL)
	{
		if(directorio->tipo == DIRECTORIO)
		{
			if(!(strcmp(directorio->nombre,"/") == 0))
			{
				eliminarArchivoDelDirecPadre(directorio,ruta);
				formatear(directorio);
			}else
			{
				printf("No es posible eliminar el directorio root\n");fflush(stdout);
				return ERROR_FILESYSTEM;
			}
		}else
		{
			return ERROR_FILESYSTEM;
		}

	}else
		return ERROR_FILESYSTEM;

	return OPERACION_OK_FILESYSTEM;
}

static void eliminarArchivoDelDirecPadre(t_elementoFS *hijoElemento,char *ruta)
{
	t_elementoFS *padre = buscarPadre(ruta,root);

	int indice = 0;
	int remove = 0;

	while (indice < list_size(padre->listaDirectorios))
	{

		t_elementoFS *hermanos = list_get(padre->listaDirectorios,indice);
		if (strcmp(hermanos->nombre,hijoElemento->nombre) == 0)
		{
			list_remove(padre->listaDirectorios,indice);
			remove = 1;
			break;
		}
		indice ++;
	}
	if (remove == 0)
		exit(1);
}

void dividirRuta(char *ruta)
{	int i = 0;
	char **rutaParseada = string_split(ruta,"/");
	while( rutaParseada[i] != NULL)
	{
		printf("%s ",rutaParseada[i]);
		i++;
	}
}

void formatearMDFS(t_elementoFS *elementoFS)
{
	formatear(elementoFS);
	limpiarListaNodosConectados();
	inicializarDirectoriosFS();

}


int formatear(t_elementoFS *elementoFS)
{

	int contador = 0;
	while(contador < list_size(elementoFS->listaDirectorios))
	{
		t_elementoFS *directorioAux = list_get(elementoFS->listaDirectorios,contador);
		formatear(directorioAux);
		contador++;
	}
	if(elementoFS->tipo == ARCHIVO)
			liberarLista(elementoFS->listaCopias);

	free(elementoFS);
	return OPERACION_OK_FILESYSTEM;
}


void liberarLista(t_list *listaBloques)
{
	t_bloque *bloque;
	while(0 != list_size(listaBloques))
	{
		bloque = list_get(listaBloques,0);
		liberarPosicionEnNodo(bloque->nombreNodo,bloque->numeroDeBloqueEnNodo);
		list_remove(listaBloques,0);
		free(bloque);
	}
	list_destroy(listaBloques);
}



t_elementoFS *elementoARemover(char *ruta,t_elementoFS *elementoFS)
{
	t_elementoFS *directorio = buscarElemento(ruta,elementoFS);
	t_elementoFS *padre = buscarPadre(ruta,elementoFS);
	int indice = buscarIndiceElemento(padre,directorio);
	t_elementoFS *elementoRemovido = list_remove(padre->listaDirectorios,indice);

	return elementoRemovido;
}

t_elementoFS *buscarPadre(char *ruta,t_elementoFS *elementoFS)
{
		int i = 0;
		char **rutaParseada = string_split(ruta,"/");
		while(rutaParseada[i] != NULL)
			i++;

		free(rutaParseada[i-1]);
		rutaParseada[i-1] = NULL;
		return buscarElementoConRutaParseada(rutaParseada,elementoFS);

}
/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////FUNCIONES CON BLOQUES////////////////////////////////////


/////////////////////////////////////// NEW BLOQUE//////////////////////////////////////////


t_bloque *newBloque(char *nombreNodo, int numeroDeBloqueNodo, int numeroDeBloqueArchivo, off_t cantBytesBloque)
{
	t_bloque *bloque = malloc(sizeof(t_bloque));

	bloque->nombreNodo = nombreNodo;
	bloque->numeroDeBloqueEnNodo = numeroDeBloqueNodo;
	bloque->numeroDeBloqueDelArchivo = numeroDeBloqueArchivo;
	bloque->cantidadBytesBloque = cantBytesBloque;

	return bloque;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////BORRAR BLOQUE DE UN ARCHIVO////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


int borrarBloqueDeUnArchivo(char *rutaMDFS, char *nombreNodo, int bloqueABorrar)
{
	int contador = 0;
	t_elementoFS *archivo;
	t_bloque *bloque;

	archivo = buscarElemento(rutaMDFS,root);

	if (archivo == NULL)
		return ERROR_FILESYSTEM;

	if(esArchivo(archivo))
	{
		while(contador < list_size(archivo->listaCopias))
		{
			bloque = list_get(archivo->listaCopias,contador);
			if((strcmp(nombreNodo,bloque->nombreNodo) == 0) && (bloque->numeroDeBloqueDelArchivo == bloqueABorrar))
			{
				liberarPosicionEnNodo(bloque->nombreNodo,bloque->numeroDeBloqueEnNodo);
				list_remove(archivo->listaCopias,contador);
				free(bloque);
				return 1;
			}
			contador++;
		}
	}
	return 0;
}

void liberarPosicionEnNodo(char *nombre,int numeroDeBloque)
{
	int contador = 0;
	t_nodo *nodo;
	reservarListaNodosConectados();
	while(contador < list_size(listaNodosConectados))
	{
		nodo = list_get(listaNodosConectados,contador);
		if(tieneNombre(nodo,nombre))
		{
			marcarPosicionComoLibreEnArray(nodo->bloques,numeroDeBloque);
			nodo->bloquesUsados--;
		}
		contador++;
	}
	devolverListaNodosConectados();
}


int copiarBloqueDeUnArchivo(char *rutaArchivoMDFS, char *nombreNodoDestino, int numeroDeBloqueACopiar)
{
	t_elementoFS *archivo = buscarElemento(rutaArchivoMDFS,root);
	t_bloque *bloqueOriginal;
	t_nodo *nodoDestino,*nodoOrigen;
	int contador = 0;
	char *texto;

	if (archivo == NULL)
		return ERROR_FILESYSTEM;

	if(estaHabilitadoNodo(nombreNodoDestino,listaNodosConectados))
	{
		nodoDestino = buscarNodoHabilitado(nombreNodoDestino);
		if (nodoDestino->bloquesDisponibles <= 0)
			return 0;
	}
	else
		return ERROR_FILESYSTEM;

	if(esArchivo(archivo))
	{
		while(contador < list_size(archivo->listaCopias))
		{
			bloqueOriginal = list_get(archivo->listaCopias,contador);
			if(bloqueOriginal->numeroDeBloqueDelArchivo == numeroDeBloqueACopiar)
			{

				if(estaHabilitadoNodo(bloqueOriginal->nombreNodo,listaNodosConectados))
				{
					//buscar nodo a grabar, validar q este habilitado
					nodoOrigen = buscarNodoHabilitado(bloqueOriginal->nombreNodo);
					if (nodoOrigen == NULL)
						return 0;
					//llamar a ver bloque que me devuelva el contenido del bloque y eso pasarlo
					texto = pedirBloqueANodo(nodoOrigen,bloqueOriginal->numeroDeBloqueEnNodo,bloqueOriginal->cantidadBytesBloque);
					if (texto == NULL)
						return 0;

					t_bloque *nuevoBloque = newBloque(nombreNodoDestino,posicionLibreEnNodo(nodoDestino->bloques),numeroDeBloqueACopiar,strlen(texto));
					list_add(archivo->listaCopias,nuevoBloque);
					grabarBloques(nodoDestino,texto,nuevoBloque);
					free(texto);

					return 1;
				}
			}
			contador++;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////VER BLOQUE DE UN ARCHIVO////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int verBloqueDelArchivo(char *rutaArchivo, int bloque)
{
	int contador = 0;
	char *texto;
	t_elementoFS *archivo;
	t_bloque *bloqueArchivo;
	t_nodo *nodo;

	archivo = buscarElemento(rutaArchivo,root);

	if (archivo == NULL)
		return ERROR_FILESYSTEM;

	if(esArchivo(archivo))
	{
		printf("cant size %d\n",list_size(archivo->listaCopias));fflush(stdout);
		while(contador < list_size(archivo->listaCopias))
		{
			bloqueArchivo = list_get(archivo->listaCopias,contador);
			printf("Entre al While\n");fflush(stdout);
			if(((bloqueArchivo->numeroDeBloqueDelArchivo) == bloque) && estaHabilitadoNodo(bloqueArchivo->nombreNodo,listaNodosConectados))
			{
				nodo = buscarNodoHabilitado(bloqueArchivo->nombreNodo);
				if (nodo != NULL)
				{
					texto = pedirBloqueANodo(nodo,bloqueArchivo->numeroDeBloqueEnNodo,bloqueArchivo->cantidadBytesBloque);
					if (texto == NULL)
						return 0;

					grabarArchivoParaFuncionVerBloque(texto);
					return 1;
				}
			}

			contador++;
		}

		return 0;
	}else
		return 0;


}

////////////////////////////////////FIN VER BOQUE DE UN ARCHIVO////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////COPIAR ARCHIVO DE MDFS A LINUX////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////


int copiarArchivoDeMDFSALiniux(char *rutaMDFS,char *rutaLinux)
{

	if(estaDisponibleArchivo(rutaMDFS))
		return pedirArchivoANodos(rutaMDFS,rutaLinux);
	else
		return 0;

}


int pedirArchivoANodos(char *rutaMDFS,char *rutaArchivoLinux)
{
	t_elementoFS *archivo = buscarElemento(rutaMDFS,root);
	if (archivo == NULL)
		return ERROR_FILESYSTEM;

	char *texto;
	int contadorBloques = 0;
	int contador = 0;
	t_bloque *bloque;
	t_list *listaAux = list_create();
	t_nodo *nodo;


	if(esArchivo(archivo))
	{
		while(contador < list_size(archivo->listaCopias))
		{
			bloque = list_get(archivo->listaCopias, contador);
			verSiAgregarBloqueALaLista(listaAux,bloque);


			contador++;
		}
		list_sort(listaAux,compararNumeroBloquesArchivo);

		if(list_size(listaAux) == archivo->cantidadBloques)
		{
			while(contadorBloques < list_size(listaAux))
			{
				bloque = list_get(listaAux,contadorBloques);
				nodo = buscarNodoHabilitado(bloque->nombreNodo);

				if (nodo == NULL)
					return 0;

				loguear(LOG_LEVEL_INFO,"Se solicito al nodo %s el bloque %d del archivo que esta en el bloque %d del nodo",nodo->nombre,bloque->numeroDeBloqueDelArchivo,bloque->numeroDeBloqueEnNodo);
				texto = pedirBloqueANodo(nodo,bloque->numeroDeBloqueEnNodo,bloque->cantidadBytesBloque);
				loguear(LOG_LEVEL_INFO,"Se obtuvo del nodo %s el bloque %d del archivo que esta en el bloque %d del nodo",nodo->nombre,bloque->numeroDeBloqueDelArchivo,bloque->numeroDeBloqueEnNodo);
				if (texto == NULL)
					return 0;

				grabarBloqueEnArchivo(texto,rutaArchivoLinux);

				contadorBloques++;
			}
			list_destroy(listaAux);
			return 1;
		}
	}
	list_destroy(listaAux);
	return 0;
}

static bool compararNumeroBloquesArchivo(void *bloque1arg, void *bloque2arg)
{
	t_bloque *bloque1 = (t_bloque*) bloque1arg;
	t_bloque *bloque2 = (t_bloque*) bloque2arg;
	int numeroBloque1;
	int numeroBloque2;

	numeroBloque1 = bloque1->numeroDeBloqueDelArchivo;
	numeroBloque2 = bloque2->numeroDeBloqueDelArchivo;

	return numeroBloque1 <= numeroBloque2;
}

static void verSiAgregarBloqueALaLista(t_list *listaAux, t_bloque *bloque)
{
	if(0 == list_size(listaAux))
	{
		if(estaHabilitadoNodo(bloque->nombreNodo,listaNodosConectados))
			list_add(listaAux,bloque);
	}else
	{
		if(existeBloqueEnLista(listaAux,bloque) == 0)
		{
			if(estaHabilitadoNodo(bloque->nombreNodo,listaNodosConectados))
				list_add(listaAux,bloque);
		}
	}
}


void grabarBloqueEnArchivo(char *cadena,char *rutaLinux)
{
	int contador = 0;
	FILE *archivo = fopen(rutaLinux,"a");
	while(cadena[contador] != '\0')
	{
		////ver de grabar siempre en la ultima pisicion del archivo
		fputc(cadena[contador],archivo);
		contador++;
	}
	fclose(archivo);
}

void grabarArchivoParaFuncionVerBloque(char *cadena)
{

	printf("Entre a GrabarArchivoParaFuncion\n");fflush(stdout);
	char *rutaLinux = strdup("/tmp/verBloqueDelArchivo.txt");
	int contador = 0;
	FILE *archivo = fopen(rutaLinux,"w");
	while(cadena[contador] != '\0')
	{
		////ver de grabar siempre en la ultima pisicion del archivo
		fputc(cadena[contador],archivo);
		contador++;
	}
	fclose(archivo);

	printf("Cerre el archivo a grabar\n"); fflush(stdout);
	free(rutaLinux);
}

////////////////////////////////////FIN COPIAR ARCHIVO DEL MDFS A LINUX////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////SOLICITAR MD5 DE ARCHIVO DEL MDFS////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////



int solicitarMD5Archivo(char *rutaArchivoMDFS)
{
	t_elementoFS *archivo = buscarElemento(rutaArchivoMDFS,root);
	if (archivo == NULL)
		return ERROR_FILESYSTEM;

	if(esArchivo(archivo))
	{
		if(copiarArchivoDeMDFSALiniux(rutaArchivoMDFS,"/tmp/archivoTemporalParaMD5deMDFS.txt") == 1)
		{
			system("md5sum /tmp/archivoTemporalParaMD5deMDFS.txt | awk '{print $1}' > /tmp/md5sum.out");
			FILE *archivoResultadoMD5 = fopen("/tmp/md5sum.out", "r");
			char resultado[1024];
			fgets(resultado, 1023, archivoResultadoMD5);
			fclose(archivoResultadoMD5);
			loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "MD5 de %s: %s", rutaArchivoMDFS, resultado);
			remove("/tmp/archivoTemporalParaMD5deMDFS.txt");
			remove("/tmp/md5sum.out");
			return 1;
		}else
		{
			remove("/tmp/archivoTemporalParaMD5deMDFS.txt");
			return 0;
		}
	}else
		return 0;

}

////////////////////////////////////FIN SOLICITAR MD5 DEL ARCHIVO DE MFDS////////////////////////////////////






//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////LA USAN VARIOS////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

char *pedirBloqueANodo(t_nodo *nodo, int numeroDeBloqueEnNodo,off_t cantidadBytesBloque)
{
	char *textoAux;
	char *texto;
	long tamanioBloque;
	off_t bytesRecibidos;
	off_t posicionBloque = 0;

	reservarConexionNodo(nodo);
	if(estaConectado(nodo->conexionNodoServidor))
		enviarMensaje(nodo->conexionNodoServidor,QUIERO_BLOQUE,sizeof(char));

	if(estaConectado(nodo->conexionNodoServidor))
		enviarMensaje(nodo->conexionNodoServidor,&numeroDeBloqueEnNodo,sizeof(int));

	if(estaConectado(nodo->conexionNodoServidor))
		enviarMensaje(nodo->conexionNodoServidor,&cantidadBytesBloque,sizeof(off_t));

	recibirYVerificarConexion(nodo->conexionNodoServidor,&tamanioBloque,sizeof(long));

	texto = malloc(tamanioBloque + 1);
	textoAux = malloc(tamanioBloque + 1);

	bytesRecibidos = recibirYVerificarConexion(nodo->conexionNodoServidor,textoAux,tamanioBloque);
	while(bytesRecibidos > 0)
	{
		memcpy(texto + posicionBloque,textoAux,bytesRecibidos);
		posicionBloque +=bytesRecibidos;

		if (posicionBloque < tamanioBloque)
			bytesRecibidos = recibirYVerificarConexion(nodo->conexionNodoServidor,textoAux,tamanioBloque);
		else
			bytesRecibidos = 0;
	}

	texto[tamanioBloque]= '\0';
	devolverConexionNodo(nodo);
	free(textoAux);

	if (cantidadBytesBloque != strlen(texto))
		return NULL;
	return texto;
}


int estaDisponibleArchivo(char *rutaMDFS)
{
	t_elementoFS *archivo = buscarElemento(rutaMDFS,root);
	if(archivo == NULL)
		return 0;
	int contador = 0;
	t_bloque *bloque;
	t_list *listaAux = list_create();

	if(esArchivo(archivo))
	{
		while(contador < list_size(archivo->listaCopias))
		{
			bloque = list_get(archivo->listaCopias, contador);

			if(0 == list_size(listaAux))
			{
				if(estaHabilitadoNodo(bloque->nombreNodo,listaNodosConectados))
					list_add(listaAux,bloque);

			}else
			{
				if(existeBloqueEnLista(listaAux,bloque) == 0)
				{
					if(estaHabilitadoNodo(bloque->nombreNodo,listaNodosConectados))
						list_add(listaAux,bloque);
				}
			}
			contador++;
		}

		if(list_size(listaAux) == archivo->cantidadBloques)
			return 1;
	}
	return 0;
}




int existeBloqueEnLista(t_list *lista, t_bloque *bloque)
{
	int contador = 0;
	t_bloque *bloqueLista;
	while(contador < list_size(lista))
	{
		bloqueLista = list_get(lista,contador);

		if(bloqueLista->numeroDeBloqueDelArchivo == bloque->numeroDeBloqueDelArchivo)
			return 1;

		contador++;
	}
	return 0;
}


static off_t recibirYVerificarConexion(SocketConecta *conexion, void *buffer, off_t longitudBuffer)
{
	if (!estaConectado(conexion))
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "Se perdio la conexion con el nodo");

	ssize_t bytesRecibidos = recibirMensaje(conexion, buffer, longitudBuffer);

	if (!estaConectado(conexion))
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "Se perdio la conexion con el nodo");


	return bytesRecibidos;
}

static void marcarPosicionComoLibreEnArray(int *bloques,int posicionALiberar)
{
	bloques[posicionALiberar] = 0;

}

static void limpiarListaNodosConectados()
{
	int contador = 0;
	t_nodo *nodo;
	reservarListaNodosConectados();
	while (contador < list_size(listaNodosConectados))
	{
		nodo = list_get(listaNodosConectados,contador);
		nodo->bloquesUsados = 0;
		ponerArrayBloquesEn0(nodo->bloques, nodo->bloquesDisponibles);
		contador ++;
	}
	devolverListaNodosConectados();
}

void imprimirEstadoNodo(char *nombreNodo)
{
	reservarListaNodosConectados();
	t_nodo *nodo;
	int contador = 0,encontroNodo = 0;
	while(contador < list_size(listaNodosConectados))
	{
		nodo = list_get(listaNodosConectados,contador);
		if (tieneNombre(nodo,nombreNodo))
		{
			encontroNodo = 1;
			break;
		}
		contador ++;
	}
	if(encontroNodo == 1)
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Informacion del Nodo : %s",nodo->nombre);
		if (nodo->estado == 1 || nodo->estado == 0)
		{
			loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"IP: %s",nodo->ip);
			loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Puerto: %s",nodo->puerto);
		}
		if (nodo->estado == 1)
			loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Estado: Habilitado");
		else if(nodo->estado == 0)
			loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "Estado: Deshabilitado");
		else
			loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG, "Estado: Desconectado");

		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Cantidad de bloques %d",nodo->bloquesDisponibles);
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Cantidad de bloques usados %d",nodo->bloquesUsados);
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Cantidad de bloques disponibles para usar %d",nodo->bloquesDisponibles - nodo->bloquesUsados);
		contador  = 0;
		while (contador < nodo->bloquesDisponibles)
		{
			if (nodo->bloques[contador] == 1)
				loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Estado del bloque %d : Ocupado",contador);
			else
				loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Estado del bloque %d : Libre",contador);
			contador++;
		}
	}
	else
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Nodo inexistente");
	devolverListaNodosConectados();
}

void imprimirEstadoArchivo(char *rutaMDFS)
{
	t_elementoFS *elemento = buscarElemento(rutaMDFS,root);
	if (elemento == NULL)
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"El archivo no Existe");
		return;
	}

	if (!esArchivo(elemento))
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"La ruta indicada no pertenece a un archivo");
		return;
	}

	loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Informacion del Archivo : %s",elemento->nombre);
	loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"Cantidad de bloques de archivo: %d",elemento->cantidadBloques);

	int contador = 0, cantidadCopias = list_size(elemento->listaCopias);

	while(contador < cantidadCopias)
	{
		t_bloque *bloque = list_get(elemento->listaCopias,contador);
		loguearYMostrarEnPantalla(LOG_LEVEL_DEBUG,"El nodo %s contiene en su bloque %d el bloque %d del archivo",bloque->nombreNodo,bloque->numeroDeBloqueEnNodo,bloque->numeroDeBloqueDelArchivo);
		contador++;

	}
}

void imprimirEstadoArchivoLog(char *rutaMDFS)
{
	t_elementoFS *elemento = buscarElemento(rutaMDFS,root);
	if (elemento == NULL)
	{
		loguear(LOG_LEVEL_DEBUG,"El archivo no Existe");
		return;
	}

	if (!esArchivo(elemento))
	{
		loguear(LOG_LEVEL_DEBUG,"La ruta indicada no pertenece a un archivo");
		return;
	}

	loguear(LOG_LEVEL_DEBUG,"Informacion del Archivo : %s",elemento->nombre);
	loguear(LOG_LEVEL_DEBUG,"Cantidad de bloques de archivo: %d",elemento->cantidadBloques);

	int contador = 0, cantidadCopias = list_size(elemento->listaCopias);

	while(contador < cantidadCopias)
	{
		t_bloque *bloque = list_get(elemento->listaCopias,contador);
		loguear(LOG_LEVEL_DEBUG,"El nodo %s contiene en su bloque %d el bloque %d del archivo",bloque->nombreNodo,bloque->numeroDeBloqueEnNodo,bloque->numeroDeBloqueDelArchivo);
		contador++;

	}
}
