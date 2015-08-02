/*
 * estadoFileSystem.h
 *
 *  Created on: 6/5/2015
 *      Author: utnso
 */

#ifndef ESTADOFILESYSTEM_H_
#define ESTADOFILESYSTEM_H_
#include <commons/collections/list.h>
#include "configFileSystem.h"
#include "libsocket.h"
#include "nodo.h"

extern t_list *listaNodosConectados;

void devolverListaNodosConectados();
void reservarListaNodosConectados();
void inicializarFileSystem(t_fileSystem *FS);
void actualizarEstadoFS(char *nombre,int estado,int habilitado);
void validarNodosOperativos();
int esEstadoOperativo();
void desconectarNodo(char *nombre);
t_nodo *agregarNodoConectado(char *ip, char *puerto, char *nombre, int bloquesDisponibles,SocketConecta *conexion);
int deshabilitarNodoConectado(char *nombre);
int habilitarNodoConectado(char *nombre);
int posicionElemento(char *nombre);
int actualizarNodoReconectado(char *nombre, SocketConecta *conexion, char *puerto);

#endif /* ESTADOFILESYSTEM_H_ */
