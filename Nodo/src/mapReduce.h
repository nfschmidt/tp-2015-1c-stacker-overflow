#ifndef MAPREDUCE_H_
#define MAPREDUCE_H_

#include <commons/collections/list.h>
#include "Archivo.h"
#include "bloqueDatos.h"

int ejecutarRutinaDeMap(const char *pathDeRutina, BloqueDatos *bloque, Archivo *archivoDeSalida);

int ejecutarRutinaDeReduce(const char *pathDeRutina, t_list *listaArchivos, Archivo *archivoDeSalida);


#endif /* MAPREDUCE_H_ */
