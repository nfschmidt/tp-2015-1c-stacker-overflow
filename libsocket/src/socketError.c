#include "socketError.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void manejarErrorSocket(const char *mensajeDeError)
{
	fprintf(stderr, "%s\n", mensajeDeError);
	exit(1);
}


void manejarWarningSocket(const char *mensaje, int codigoDeError)
{
	/*
	fprintf(stderr, "%s\n", mensaje);
	fprintf(stderr, "Detalle: %s\n", strerror(codigoDeError));
	fprintf(stderr, "Codigo de error: %d\n", codigoDeError);
	*/
}
