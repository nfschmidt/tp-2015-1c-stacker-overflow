#ifndef CONSOLA_SRC_CONSOLA_H_
#define CONSOLA_SRC_CONSOLA_H_

#include <commons/collections/list.h>

#define ARGUMENTOS_VARIABLES -1

typedef void (*operacionConsola)(char **argumentos);

typedef struct
{
	t_list *listaComandos;
	char *prompt;
} Consola;



Consola *newConsola(const char *prompt);

void agregarComando(Consola *consola, const char *stringComando, operacionConsola operacion, int cantidadArgumentos);

void ejecutarConsola(Consola *consola);



#endif /* CONSOLA_SRC_CONSOLA_H_ */
