#ifndef ASIGNARBLOQUE_H_
#define ASIGNARBLOQUE_H_
#include <sys/types.h>
#include <sys/stat.h>
#include "nodo.h"

t_nodo *seleccionarNodoMenosCargado(t_list *listaNodosConectados, int posicion);
bool compararCantidadBloquesNodo(void *nodo1arg, void *nodo2arg);
void copiarArchivoALinux(char *ruta,char *rutaDestino);
void generarArchivoPesado(char *ruta);
void eliminarLista(t_list *lista);
FILE *abrirArchivoParaBloque(char *ruta);
int bloquesDisponibleNodo(t_nodo *nodo);
int posicionLibreEnNodo(int *bloques);
void mostrarEstadoDelFSEnBytesLogYPantalla();
void mostrarEstadoDelFSEnBytesSoloLog();

#endif /* ASIGNARBLOQUE_H_ */
