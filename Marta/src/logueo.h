#ifndef LOGUEO_H_
#define LOGUEO_H_

#include <commons/log.h>

void setPathArchivoDeLog(char *pathArchivoDeLog);

void loguear(t_log_level logLevel, char *mensaje, ...);

void loguearYMostrarEnPantalla(t_log_level logLevel, char *mensaje, ...);

#endif
