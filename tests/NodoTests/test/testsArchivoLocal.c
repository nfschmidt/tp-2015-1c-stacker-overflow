#include "ArchivoLocal.h"
#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>


#define PATH_ARCHIVO_LECTURA "test/resources/lectura.txt"
#define PATH_ARCHIVO_ESCRITURA "test/resources/escritura.txt"
#define REGISTRO1 "Este es el registro 1\n"
#define REGISTRO2 "Este es el registro 2\n"
#define REGISTRO_INCOMPLETO "Este no es un registro porque no termina con salto de linea"
#define NUEVO_REGISTRO_CON_SALTO_DE_LINEA "Este es el nuevo registro\n"
#define NUEVO_REGISTRO_SIN_SALTO_DE_LINEA "Este es el nuevo registro"


Archivo *archivo = 0;
char *buffer = 0;


static void inicializar()
{
	FILE *archivoALlenar = fopen(PATH_ARCHIVO_LECTURA, "w");
	fprintf(archivoALlenar, "%s", REGISTRO1);
	fprintf(archivoALlenar, "%s", REGISTRO2);
	fprintf(archivoALlenar, "%s", REGISTRO_INCOMPLETO);
	fclose(archivoALlenar);

	archivo = (Archivo *) newArchivoLocal(PATH_ARCHIVO_LECTURA, LECTURA);
}

static void limpiar()
{
	if(buffer != 0)
	{
		free(buffer);
		buffer = 0;
	}
}


void debePoderObtenerUnRegistro()
{
	inicializar();

	buffer = obtenerRegistro(archivo);
	CU_ASSERT_STRING_EQUAL(buffer, REGISTRO1);

	limpiar();
}

void debePoderObtenerElSiguienteRegistro()
{
	inicializar();

	buffer = obtenerRegistro(archivo);
	free(buffer);
	buffer = obtenerRegistro(archivo);

	CU_ASSERT_STRING_EQUAL(buffer, REGISTRO2);

	limpiar();
}

void devuelveNuloSiNoHayMasRegistros()
{
	inicializar();

	buffer = obtenerRegistro(archivo);
	free(buffer);
	buffer = obtenerRegistro(archivo);
	free(buffer);
	buffer = obtenerRegistro(archivo);

	CU_ASSERT_PTR_EQUAL(buffer, NULL);

	limpiar();
}

void puedeLeerUltimoRegistroSiNoHayMasContenidoLuego()
{
	FILE *archivoALlenar = fopen(PATH_ARCHIVO_LECTURA, "w");
	fprintf(archivoALlenar, "%s", REGISTRO1);
	fclose(archivoALlenar);

	inicializar();

	buffer = obtenerRegistro(archivo);

	CU_ASSERT_STRING_EQUAL(buffer, REGISTRO1);

	limpiar();
}

void puedeEscribirRegistro()
{
	Archivo *archivoEscritura = (Archivo *) newArchivoLocal(PATH_ARCHIVO_ESCRITURA, ESCRITURA);

	escribirRegistro(archivoEscritura, NUEVO_REGISTRO_CON_SALTO_DE_LINEA);
	cerrarArchivo(archivoEscritura);

	Archivo *archivoLectura = (Archivo *) newArchivoLocal(PATH_ARCHIVO_ESCRITURA, LECTURA);
	buffer = obtenerRegistro(archivoLectura);

	CU_ASSERT_STRING_EQUAL(buffer, NUEVO_REGISTRO_CON_SALTO_DE_LINEA);
}

void alEscribirAgregaSaltoDeLineaSiNoLoTiene()
{
	Archivo *archivoEscritura = (Archivo*) newArchivoLocal(PATH_ARCHIVO_ESCRITURA, ESCRITURA);

	escribirRegistro(archivoEscritura, NUEVO_REGISTRO_SIN_SALTO_DE_LINEA);
	cerrarArchivo(archivoEscritura);

	Archivo *archivoLectura = (Archivo*) newArchivoLocal(PATH_ARCHIVO_ESCRITURA, LECTURA);
	buffer = obtenerRegistro(archivoLectura);

	CU_ASSERT_STRING_EQUAL(buffer, NUEVO_REGISTRO_CON_SALTO_DE_LINEA);
}

void alEscribirNoAgregaSaltoDeLineaSiYaLoTiene()
{
	Archivo *archivoEscritura = (Archivo*) newArchivoLocal(PATH_ARCHIVO_ESCRITURA, ESCRITURA);

	escribirRegistro(archivoEscritura, NUEVO_REGISTRO_CON_SALTO_DE_LINEA);
	cerrarArchivo(archivoEscritura);

	Archivo *archivoLectura = (Archivo*) newArchivoLocal(PATH_ARCHIVO_ESCRITURA, LECTURA);
	buffer = obtenerRegistro(archivoLectura);
	free(buffer);
	buffer = obtenerRegistro(archivoLectura);

	CU_ASSERT_PTR_EQUAL(buffer, NULL);
}


void agregarTestsDeArchivoLocal(CU_pSuite suite)
{
	CU_ADD_TEST(suite, debePoderObtenerUnRegistro);
	CU_ADD_TEST(suite, debePoderObtenerElSiguienteRegistro);
	CU_ADD_TEST(suite, devuelveNuloSiNoHayMasRegistros);
	CU_ADD_TEST(suite, puedeLeerUltimoRegistroSiNoHayMasContenidoLuego);
	CU_ADD_TEST(suite, puedeEscribirRegistro);
	CU_ADD_TEST(suite, alEscribirAgregaSaltoDeLineaSiNoLoTiene);
	CU_ADD_TEST(suite, alEscribirNoAgregaSaltoDeLineaSiYaLoTiene);
}
