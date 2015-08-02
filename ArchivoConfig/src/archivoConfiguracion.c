#include "archivosConfiguracion.h"
#include <commons/config.h>
#include <string.h>
#include <stdlib.h>

static int *obtenerParametroEntero(t_config *conf, char *parametro);
static char *obtenerParametroString(t_config *conf, char *parametro);
static char **obtenerParametroLista(t_config *conf, char *parametro);
static double *obtenerParametroDouble(t_config *conf, char *parametro);
static long *obtenerParametroLong(t_config *conf, char *parametro);


void *obtenerParametroConfiguracion(t_config *conf, char *parametro, tipoDeDato tipo)
{
	bool existeParametro = config_has_property(conf,parametro);

	if(existeParametro)
	{
		switch(tipo)
		{
		case ENTERO: return obtenerParametroEntero(conf, parametro);

		case STRING: return obtenerParametroString(conf, parametro);

		case LISTA: return obtenerParametroLista(conf, parametro);

		case LONG: return obtenerParametroLong(conf, parametro);

		case DOUBLE: return obtenerParametroDouble(conf, parametro);
		}
	}
	return NULL;
}


static int *obtenerParametroEntero(t_config *conf, char *parametro)
{
	int *valorParametro = malloc(sizeof(int));
	*((int*) valorParametro) = config_get_int_value(conf, parametro);
	return valorParametro;
}

static char *obtenerParametroString(t_config *conf, char *parametro)
{
	char *valorParametro = config_get_string_value(conf, parametro);
	return valorParametro;
}

static char **obtenerParametroLista(t_config *conf, char *parametro)
{
	char **valorParametro = config_get_array_value(conf, parametro);

	return valorParametro;
}

static long *obtenerParametroLong(t_config *conf, char *parametro)
{
	long *valorParametro = malloc(sizeof(long));
	*((long*) valorParametro) = config_get_long_value(conf, parametro);
	return valorParametro;
}

static double *obtenerParametroDouble(t_config *conf, char *parametro)
{
	double *valorParametro = malloc(sizeof(double));
	*((double*) valorParametro) = config_get_double_value(conf, parametro);
	return valorParametro;
}
