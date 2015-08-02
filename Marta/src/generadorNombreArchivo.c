#include "generadorNombreArchivo.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

char *obtenerNombreArchivoResultadoTemporal(TipoTarea tipoTarea)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    char *pathTemporal = malloc(100);

    if (tipoTarea == TAREA_MAP)
    	sprintf(pathTemporal, "map_%lu.%lu.tmp", spec.tv_sec, spec.tv_nsec);
    else
    	sprintf(pathTemporal, "reduce_%lu.%lu.tmp", spec.tv_sec, spec.tv_nsec);

    return pathTemporal;
}
