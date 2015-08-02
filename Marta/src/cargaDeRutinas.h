/*
 * cargaDeRutinas.h
 *
 *  Created on: 14/7/2015
 *      Author: utnso
 */

#ifndef CARGADERUTINAS_H_
#define CARGADERUTINAS_H_

#define CARGA_MAP_DEFAULT 1
#define CARGA_REDUCE_DEFAULT 3
#define CARGA_MAXIMA_DEFAULT 10


void setCargaDeMap(int carga);

int getCargaMap();


void setCargaDeReduce(int carga);

int getCargaReduce();


void setCargaMaxima(int carga);

int getCargaMaxima();


#endif /* CARGADERUTINAS_H_ */
