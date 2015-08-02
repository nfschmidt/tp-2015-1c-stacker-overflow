#ifndef EJECUCIONDERUTINA_H_
#define EJECUCIONDERUTINA_H_

#include "Archivo.h"

#define RUTINA_OK 0
#define RUTINA_ERROR_LECTURA_DATOS 1
#define RUTINA_ERROR_EJECUCION 2

int ejecutarRutina(const char *pathDeRutina, Archivo *archivoDeEntrada, Archivo *archivoDeSalida);


#endif /* EJECUCIONDERUTINA_H_ */
