#ifndef ESTRUCTURAFS_H_
#define ESTRUCTURAFS_H_
#include <commons/collections/list.h>
#include "libsocket.h"
#include "nodo.h"


typedef enum{ARCHIVO,DIRECTORIO}t_elemento;

typedef struct {
	char *nombre;
	t_elemento tipo;
	t_list *listaDirectorios;
	int cantidadBloques;
	t_list *listaCopias;
}t_elementoFS;

typedef struct {
	char *nombreNodo;   	 			//nombre unico del nodo
	int numeroDeBloqueEnNodo;			//me dice donde esta guardado el BLOQUE(numeroDeBloqueDelArchivo) en el NODO
	int numeroDeBloqueDelArchivo;	    //numero de bloque del archivo
	off_t cantidadBytesBloque;			//numero de btes que tiene el bloque(no me quedo otra por el tema de la basura y el getregistor lee hastael \n y no \0
}t_bloque;


t_elementoFS *root;

t_elementoFS *buscarPadre(char *ruta,t_elementoFS *elementoFS);
t_elementoFS *newElementoFS(char *nombre, t_elemento tipo);
t_elementoFS *buscarElemento(char *ruta,t_elementoFS *elementoFS);
t_elementoFS *elementoARemover(char *ruta, t_elementoFS *elementoFS);
t_elementoFS *crearArchivo(char *ruta, char *nombre,t_elementoFS *elementoFS);
t_bloque *newBloque(char *nombreNodo, int numeroDeBloqueNodo, int numeroDeBloqueArchivo, off_t cantBytesBloque);
int esDirectorio(t_elementoFS *elementoFS);
int esArchivo(t_elementoFS *elementoFS);
int agregarElementoASubDirectorio(t_elementoFS *elementoFS,t_elementoFS *nuevoElemento);
int crearDirectorio(char *ruta, char *nombre,t_elementoFS *elementoFS);
int renombrarDirectorio(char *ruta, char* nombre,t_elementoFS *elementoFS);
int renombrarArchivo(char *ruta, char* nuevoNombre,t_elementoFS *elementoFS);
int moverDirectorio(char *rutaOrigen, char *rutaDestino, t_elementoFS *elementoFS);
int moverArchivo(char *rutaOrigen, char *rutaDestino, t_elementoFS *elementoFS);
int buscarIndiceElemento(t_elementoFS *elementoPadre, t_elementoFS *elementoFS);
int eliminarArchivo(char *ruta,t_elementoFS *elementoFS);
int tienenMismoNombre(t_elementoFS *elementoFS, t_elementoFS *otroElementoFS);
int eliminarDirectorio(char *ruta,t_elementoFS *elementoFS);
int existeArchivoQueQuieroAgregar(char *rutaMDFS, char *nombreArchivo);
void formatearMDFS(t_elementoFS *elementoFS);
int formatear(t_elementoFS *elementoFS);
void imprimirArbol(t_elementoFS *elementoFS, int nivel);
void dividirRuta(char *ruta);
void inicializarDirectoriosFS();
int moverElemento(char *rutaOrigen, char *rutaDestino, t_elementoFS *elementoFS);
void liberarLista(t_list *listaBloques);
void liberarPosicionEnNodo(char *nombre,int numeroDeBloque);
int estaDisponibleArchivo(char *rutaMDFS);
int existeBloqueEnLista(t_list *lista, t_bloque *bloque);
int copiarBloqueDeUnArchivo(char *rutaArchivoMDFS, char *nombreNodo, int numeroDeBloqueACopiar);
int verBloqueDelArchivo(char *rutaArchivo, int bloque);
char *pedirBloqueANodo(t_nodo *nodo, int numeroDeBloqueEnNodo,off_t cantidadBytesBloque);
int pedirArchivoANodos(char *rutaMDFS,char *rutaLinux);
int copiarArchivoDeMDFSALiniux(char *rutaMDFS,char *rutaLinux);
void grabarBloqueEnArchivo(char *cadena,char *rutaLinux);
int solicitarMD5Archivo(char *rutaArchivo);
int estadoMongo();
void imprimirEstadoNodo(char *nombreNodo);
void imprimirEstadoArchivo(char *rutaMDFS);
void imprimirEstadoArchivoLog(char *rutaMDFS);
void grabarArchivoParaFuncionVerBloque(char *cadena);
int borrarBloqueDeUnArchivo(char *ruta, char *nombreNodo, int bloque);

#endif /* ESTRUCTURAFS_H_ */
