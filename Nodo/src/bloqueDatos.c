/*
 * bloqueDatos.c
 *
 *  Created on: 7/5/2015
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include "bloqueDatos.h"
#include "espacioDatos.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <err.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "logueo.h"

BloqueDatos  *getBloque(EspacioDatos *esp, long numBloq)
{
	BloqueDatos *bloqueDatos = malloc(sizeof(BloqueDatos));
	bloqueDatos->numeroBloque = numBloq;
	bloqueDatos->registroPos = 0;

	long InicioBloque = calcularInicioBloque(numBloq);
	long TamanioBloque = calcularTamanioBloque(numBloq,esp->Size);

	bloqueDatos->textoBloque = esp->archivoMemoria + InicioBloque;

	bloqueDatos->tamanioBloque = TamanioBloque;

	return bloqueDatos;
}

long calcularInicioBloque(long numBloq)
{

	long Inicio;

	Inicio = tamanioBloqueFijo * numBloq;

	return Inicio;
}

long calcularTamanioBloque(long numBloq,long tamanioArchivo)
{

	long TamanioBloque;
	numBloq++;
	TamanioBloque = (tamanioBloqueFijo * numBloq);
	TamanioBloque--;
	numBloq--;

	if (TamanioBloque > tamanioArchivo)
		return tamanioArchivo - numBloq * tamanioBloqueFijo;

	return TamanioBloque - numBloq * tamanioBloqueFijo;
}
