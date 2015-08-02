#include "generadorNombreArchivo.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

char *obtenerNombreArchivoResultadoTemporal()
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    char *pathTemporal = malloc(1024 * 3 * sizeof(char));

	sprintf(pathTemporal, "/tmp/temporal.%lu.%lu.tmp", spec.tv_sec, spec.tv_nsec);

    return pathTemporal;
}
