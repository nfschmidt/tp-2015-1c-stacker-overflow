/*
 * Job.h
 *
 *  Created on: 10/6/2015
 *      Author: utnso
 */

#ifndef JOB_H_
#define JOB_H_

void recibirYChequearConexion(SocketConecta *socket,void * mensaje, int tam);
pthread_mutex_t semaforoMarta;

#endif /* JOB_H_ */
