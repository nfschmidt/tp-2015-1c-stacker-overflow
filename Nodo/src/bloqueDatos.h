/*
 * bloqueDatos.h
 *
 *  Created on: 7/5/2015
 *      Author: utnso
 */

#ifndef BLOQUEDATOS_H_
#define BLOQUEDATOS_H_
#include "espacioDatos.h"

#define tamanioBloqueFijo 20971520

typedef struct{
	char *textoBloque;
	long numeroBloque;
	long registroPos;
	long tamanioBloque;
}BloqueDatos;

BloqueDatos  *getBloque(EspacioDatos *esp, long numBloq);
long calcularInicioBloque(long numBloq);
long calcularTamanioBloque(long numBloq,long tamanioArchivo);
#endif /* BLOQUEDATOS_H_ */
