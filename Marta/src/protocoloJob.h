#ifndef PROTOCOLOJOB_H_
#define PROTOCOLOJOB_H_

#include "job.h"
#include <libsocket.h>

t_job *protocoloJobObtenerDatosJob(SocketConecta *conexion);

int protocoloJobNotificarTarea(t_job *job, t_tarea *tarea);

int protocoloJobObtenerResultadoDeSubidaDeArchivo(t_job *job);

void protocoloJobNotificarFin(t_job *job);

#endif /* PROTOCOLOJOB_H_ */
