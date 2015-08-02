/*
 * responderANodo.h
 *
 *  Created on: 20/5/2015
 *      Author: utnso
 */

#ifndef RESPONDERANODO_H_
#define RESPONDERANODO_H_

#include <libsocket.h>
#include "ArchivoLocal.h"

ArchivoLocal *tengoArchivo(SocketConecta *conexion);
void obtenerSiguienteConjuntosRegistrosRemotos(SocketConecta *conexion, ArchivoLocal *archivoALeer);
void terminoDePedir(SocketConecta *conexion, ArchivoLocal *archivoALeer);


#endif /* RESPONDERANODO_H_ */
