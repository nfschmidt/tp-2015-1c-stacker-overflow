#include "consola.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commons/string.h>

typedef struct
{
	char *nombreComando;
	operacionConsola operacion;
	int cantidadArgumentos;
} Comando;


static void leerInput(Consola *consola, char *buffer_input);
static void interpretarYEjecutar(Consola *consola, char *stringComando);
static Comando *obtenerComando(Consola *consola, char *stringComando);
static void ejecutarOperacionDeComando(Comando *comando, char *argumentos[]);
static int cantidadDeArgumentosEsValida(Comando *comando, char *argumentos[]);
static int obtenerCantidadArgumentos(char **argumentos);
static void mostrarMensajeComandoInvalido(char *stringComando);
static void mostrarMensajeComandoCantidadArgumentosInvalidos(Comando *comando);


Consola *newConsola(const char *prompt)
{
	Consola *consola = malloc(sizeof(Consola));
	consola->listaComandos = list_create();
	consola->prompt = strdup(prompt);

	return consola;
}


void agregarComando(Consola *consola, const char *stringComando, operacionConsola operacion, int cantidadArgumentos)
{
	Comando *comando = malloc(sizeof(Comando));
	comando->nombreComando = strdup(stringComando);
	comando->operacion = operacion;
	comando->cantidadArgumentos = cantidadArgumentos;

	list_add(consola->listaComandos, comando);
}


void ejecutarConsola(Consola *consola)
{
	char buffer_input[1024];
	while(1)
	{
		leerInput(consola, buffer_input);
		interpretarYEjecutar(consola, buffer_input);
	}
}


static void leerInput(Consola *consola, char *buffer_input)
{
	printf("%s", consola->prompt);
	scanf(" %[^\n]", buffer_input);
}


static void interpretarYEjecutar(Consola *consola, char *input)
{
	char **palabrasIngresadas = string_split(input, " ");
	char *stringComando = palabrasIngresadas[0];
	char **argumentos = palabrasIngresadas + 1;

	Comando *comando = obtenerComando(consola, stringComando);

	if (comando)
		ejecutarOperacionDeComando(comando, argumentos);
	else
		mostrarMensajeComandoInvalido(stringComando);
}


static Comando *obtenerComando(Consola *consola, char *stringComando)
{
	t_list *listaComandos = consola->listaComandos;
	int cantidadComandos = list_size(listaComandos);

	int i;
	for(i = 0; i < cantidadComandos; i ++)
	{
		Comando *comando = list_get(listaComandos, i);
		if(strcmp(comando->nombreComando, stringComando) == 0)
			return comando;
	}

	return NULL;
}


static void mostrarMensajeComandoInvalido(char *stringComandoInvalido)
{
	printf("Error: el comando \"%s\" no es válido.\n", stringComandoInvalido);
}


static void ejecutarOperacionDeComando(Comando *comando, char **argumentos)
{
	if (cantidadDeArgumentosEsValida(comando, argumentos))
		comando->operacion(argumentos);
	else
		mostrarMensajeComandoCantidadArgumentosInvalidos(comando);
}


static int cantidadDeArgumentosEsValida(Comando *comando, char *argumentos[])
{
	if (comando->cantidadArgumentos == ARGUMENTOS_VARIABLES)
		return 1;

	return obtenerCantidadArgumentos(argumentos) == comando->cantidadArgumentos;
}


static int obtenerCantidadArgumentos(char **argumentos)
{
	int i = 0;

	while (argumentos[i] != NULL)
		i ++;

	return i;
}


static void mostrarMensajeComandoCantidadArgumentosInvalidos(Comando *comando)
{
	printf("Error: el comando \"%s\" necesita %d argumento", comando->nombreComando,
			comando->cantidadArgumentos);

	if(comando->cantidadArgumentos != 1)
		printf("s");

	printf(".\n");
}
