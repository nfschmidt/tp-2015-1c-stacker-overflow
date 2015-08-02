/*
 * cargaDeRutinas.c
 *
 *  Created on: 14/7/2015
 *      Author: utnso
 */
#include "cargaDeRutinas.h"


static int cargaDeMap = CARGA_MAP_DEFAULT;
static int cargaDeReduce = CARGA_REDUCE_DEFAULT;
static int cargaMaxima = CARGA_MAXIMA_DEFAULT;

void setCargaDeMap(int carga)
{
	cargaDeMap = carga;
}

int getCargaMap()
{
	return cargaDeMap;
}

void setCargaDeReduce(int carga)
{
	cargaDeReduce = carga;
}


int getCargaReduce()
{
	return cargaDeReduce;
}


void setCargaMaxima(int carga)
{
	cargaMaxima = carga;
}


int getCargaMaxima()
{
	return cargaMaxima;
}


