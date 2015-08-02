/*
 * planificador.h
 *
 *  Created on: 21/7/2015
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "job.h"
#include "nodo.h"
#include "tarea.h"

extern t_list *(*obtenerDetalleDeBloques)(char *pathMDFS, int bloque);
extern int (*notificarTareaAJob)(t_job *job, t_tarea *tarea);
extern void (*notificarFinalizacionJob)(t_job *job);
extern int (*recibirResultadoDeSubidaDeArchivoAMDFS)(t_job *job);

void planificadorInicializar();

void planificadorAgregarNodo(t_nodo *nodo);

void planificadorAgregarJob(t_job *job);

t_tarea *planificadorObtenerSiguienteTareaYComunicarlaAJob();

t_tarea *planificadorObtenerTareaDeId(uint32_t idTarea);

void planificadorMarcarTareaFinalizada(uint32_t idTarea);

void planificadorMarcarTareaFallida(uint32_t idTarea);

void planificadorMarcarTareaFallidaPorBajaDeNodo(uint32_t idTarea);

t_job *planificadorObtenerJobDeTarea(uint32_t idTarea);

uint32_t planificadorDarDeBajaJobPorDesconexion(SocketConecta *conexion);

#endif /* PLANIFICADOR_H_ */
