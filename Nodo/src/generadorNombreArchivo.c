#include "generadorNombreArchivo.h"
#include "configNodo.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

char *obtenerNombreArchivoResultadoTemporal(int tipo)
{
	// Uso la hora del sistema en segundos y nanosegundos
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    char *pathTemporal = malloc(1024 * 3 * sizeof(char));

    pid_t miPID = getpid();
    pthread_t miTID = pthread_self();

    if (tipo == TIPO_EJECUTABLE)
    	sprintf(pathTemporal, "%sejecutable.%d.%d.%lu.%lu.tmp", obtenerPathEspacioTemporal(), miPID, (unsigned int)miTID, spec.tv_sec, spec.tv_nsec);
    else if (tipo == TIPO_INTERMEDIO)
    	sprintf(pathTemporal, "%sintermedio.%d.%d.%lu.%lu.tmp", obtenerPathEspacioTemporal(), miPID, (unsigned int)miTID, spec.tv_sec, spec.tv_nsec);

    return pathTemporal;
}
