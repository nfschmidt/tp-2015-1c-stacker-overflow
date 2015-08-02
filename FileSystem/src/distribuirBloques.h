/*
 * distribuirBloques.h
 *
 *  Created on: 20/6/2015
 *      Author: utnso
 */

#ifndef DISTRIBUIRBLOQUES_H_
#define DISTRIBUIRBLOQUES_H_

#define TAMANIO_MAXIMO_BLOQUE 20971520

long int posicionALeerReubicarArchivo(off_t ultimaPosBarraN, off_t contadorBloque);
void copiarMemoriaBloques(off_t ultimaPosBarraN,char **bloque);
int verEnviarOIncrementarBloques(char *bloque,t_elementoFS *archivoNuevo, int cantidadBloques,
		int enviar,t_list *listaAux);
#endif /* DISTRIBUIRBLOQUES_H_ */
