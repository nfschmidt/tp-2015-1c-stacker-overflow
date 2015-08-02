#ifndef PROCESOS_H_
#define PROCESOS_H_

#include "Archivo.h"
#include <stdlib.h>

#define PROCESO_OK 0

pid_t crearProcesoHijoNoBloqueante(const char *path, Archivo *nuevoStdin, Archivo *nuevoStdout);

int crearProcesoHijoBloqueante(const char *path, Archivo *nuevoStdin, Archivo *nuevoStdout);

int esperarProcesoHijo(pid_t pid);


#endif
