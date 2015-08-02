/*
 * registroDatos.c

 *
 *  Created on: 7/5/2015
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include "registroDatos.h"
#include "bloqueDatos.h"
#include <string.h>
#include "logueo.h"

#define SIZE_BLOQUE 	(20*1024*1024)

char *getRegistro(BloqueDatos *bloque)
{
	long posicionInicio = bloque->registroPos;

	if (posicionInicio == SIZE_BLOQUE || bloque->textoBloque[posicionInicio] == '\0')
		return NULL;

	long posicionFinal = posicionInicio;
	while (posicionFinal < SIZE_BLOQUE && bloque->textoBloque[posicionFinal] != '\n')
		posicionFinal ++;

	if (posicionFinal == SIZE_BLOQUE)
	{
		bloque->registroPos = posicionFinal;
		return NULL;
	}

	long cantidadCaracteresDeRegistro = posicionFinal - posicionInicio + 1;

	char *registro = malloc(cantidadCaracteresDeRegistro + 1);
	memcpy(registro, bloque->textoBloque + posicionInicio, cantidadCaracteresDeRegistro);
	registro[cantidadCaracteresDeRegistro] = '\0';

	bloque->registroPos += cantidadCaracteresDeRegistro;

	return registro;
}
