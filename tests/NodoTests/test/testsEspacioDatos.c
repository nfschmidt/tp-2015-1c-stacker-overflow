#include <CUnit/Basic.h>
#include "espacioDatos.h"
#include <stdlib.h>
#include "bloqueDatos.h"
#include "registroDatos.h"


#define PATH_ESPACIO_DATOS "/tmp/espacio.bin"
#define BLOQUES_ESPACIO_DATOS "2"
#define PATH_CREADOR_ESPACIOS "../../utilidades/creadorEspacioDatos"
#define COMANDO_CREACION_ESPACIO_DATOS PATH_CREADOR_ESPACIOS " " PATH_ESPACIO_DATOS " " BLOQUES_ESPACIO_DATOS

#define CANTIDAD_REGISTROS_BLOQUE_0 958301

static int espacioFueCreado = 0;

static void inicializar()
{
	// Crear espacio de datos
	setPathArchivoDeLog("/tmp/nodo.log");
	if (! espacioFueCreado)
	{
		printf("Creando espacio de datos\n");
		system(COMANDO_CREACION_ESPACIO_DATOS);
		espacioFueCreado = 1;
	}
}


void sePuedeObtenerUnEspacioDeDatos()
{
	inicializar();
	EspacioDatos *espacio = newEspacioDatos("/tmp/espacio.bin");

	CU_ASSERT_PTR_NOT_NULL(espacio);

	free(espacio);
}


void sePuedeObtenerUnBloque()
{
	EspacioDatos *espacio = newEspacioDatos(PATH_ESPACIO_DATOS);

	BloqueDatos *bloque0 = getBloque(espacio, 0);

	CU_ASSERT_PTR_NOT_EQUAL(bloque0, NULL);

	free(bloque0);
	free(espacio);
}


void sePuedeObtenerMasDeUnBloqueDelMismoEspacioDeDatos()
{
	EspacioDatos *espacio = newEspacioDatos(PATH_ESPACIO_DATOS);

	BloqueDatos *bloque0 = getBloque(espacio, 0);
	BloqueDatos *bloque1 = getBloque(espacio, 1);

	CU_ASSERT_PTR_NOT_EQUAL(bloque0, NULL);
	CU_ASSERT_PTR_NOT_EQUAL(bloque1, NULL);
	CU_ASSERT_PTR_NOT_EQUAL(bloque0, bloque1);

	free(bloque0);
	free(bloque1);
	free(espacio);
}


void sePuedeObtenerUnRegistroDeUnBloque()
{
	EspacioDatos *espacio = newEspacioDatos(PATH_ESPACIO_DATOS);
	BloqueDatos *bloque0 = getBloque(espacio, 0);

	char *registro = getRegistro(bloque0);

	CU_ASSERT_PTR_NOT_EQUAL(registro, NULL);

	free(registro);
	free(bloque0);
	free(espacio);
}


void elContenidoDelPrimerRegistroSeLeeCorrectamente()
{
	inicializar();

	EspacioDatos *espacio = newEspacioDatos(PATH_ESPACIO_DATOS);
	BloqueDatos *bloque0 = getBloque(espacio, 0);

	char *registro = getRegistro(bloque0);

	CU_ASSERT_STRING_EQUAL(registro, "BLOQUE 0 LINEA 0\n");
	free(registro);

	free(bloque0);
	free(espacio);
}


void detectaCorrectamenteLaCantidadDeRegistrosQueTieneElBloque()
{
	inicializar();
	EspacioDatos *espacio = newEspacioDatos(PATH_ESPACIO_DATOS);
	BloqueDatos *bloque0 = getBloque(espacio, 0);

	unsigned long registrosLeidos = 0;
	char *registro;

	registro = getRegistro(bloque0);
	while (registro != NULL)
	{
		free(registro);
		registrosLeidos ++;
		registro = getRegistro(bloque0);
	}

	CU_ASSERT_EQUAL(registrosLeidos, CANTIDAD_REGISTROS_BLOQUE_0);

	printf("Registros leidos: %lu\n", registrosLeidos);

	free(bloque0);
	free(espacio);
}


void agregarTestsDeEspacioDeDatos(CU_pSuite suite)
{
	CU_ADD_TEST(suite, sePuedeObtenerUnEspacioDeDatos);

	CU_ADD_TEST(suite, sePuedeObtenerUnBloque);
	CU_ADD_TEST(suite, sePuedeObtenerMasDeUnBloqueDelMismoEspacioDeDatos);
	CU_ADD_TEST(suite, sePuedeObtenerUnRegistroDeUnBloque);
	CU_ADD_TEST(suite, elContenidoDelPrimerRegistroSeLeeCorrectamente);
	CU_ADD_TEST(suite, detectaCorrectamenteLaCantidadDeRegistrosQueTieneElBloque);
}




