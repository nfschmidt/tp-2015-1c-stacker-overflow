#ifndef SELECTOR_H_
#define SELECTOR_H_

#include "SocketConecta.h"
#include "SocketEscucha.h"

#include <commons/collections/list.h>
#include <sys/select.h>

typedef struct
{
	SocketEscucha *listener;
	SocketConecta *nuevaConexion;
	t_list *conexiones;
	fd_set fdSet;
} Selector;



Selector *newSelector(SocketEscucha *listener);

void esperarEventos(Selector *selector);

int hayNuevaConexion(Selector *selector);

int hayMensajesEntrantes(Selector *selector);

int hayConexionesCaidas(Selector *selector);

SocketConecta *obtenerNuevaConexion(Selector *selector);

t_list *obtenerConexionesActivas(Selector *selector);

t_list *obtenerConexionesConMensajesEntrantes(Selector *selector);

t_list *obtenerConexionesCaidas(Selector *selector);

#endif
