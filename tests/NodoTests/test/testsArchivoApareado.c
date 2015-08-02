#include <CUnit/Basic.h>
#include "ArchivoLocal.h"
#include "ArchivoApareado.h"

#include <commons/collections/list.h>
#include <stdlib.h>

#define PATH_ARCHIVO_1 "test/resources/apareo1.txt"
#define PATH_ARCHIVO_2 "test/resources/apareo2.txt"

#define ARCHIVO1_REGISTRO0_0 "0\n"
#define ARCHIVO1_REGISTRO1_2 "2\n"
#define ARCHIVO1_REGISTRO2_5 "5\n"
#define ARCHIVO1_REGISTRO3_6 "6\n"
#define ARCHIVO1_REGISTRO4_7 "7\n"
#define ARCHIVO2_REGISTRO0_1 "1\n"
#define ARCHIVO2_REGISTRO1_3 "3\n"
#define ARCHIVO2_REGISTRO2_4 "4\n"


t_list *listaArchivos;
Archivo *archivoApareado;

static void inicializar()
{
	listaArchivos = list_create();


	// Crear archivo apareado
	Archivo *archivoLocal1 = (Archivo *) newArchivoLocal(PATH_ARCHIVO_1, ESCRITURA);
	escribirRegistro(archivoLocal1, ARCHIVO1_REGISTRO0_0);
	escribirRegistro(archivoLocal1, ARCHIVO1_REGISTRO1_2);
	escribirRegistro(archivoLocal1, ARCHIVO1_REGISTRO2_5);
	escribirRegistro(archivoLocal1, ARCHIVO1_REGISTRO3_6);
	escribirRegistro(archivoLocal1, ARCHIVO1_REGISTRO4_7);
	cerrarArchivo(archivoLocal1);

	Archivo *archivoLocal2 = (Archivo *) newArchivoLocal(PATH_ARCHIVO_2, ESCRITURA);
	escribirRegistro(archivoLocal2, ARCHIVO2_REGISTRO0_1);
	escribirRegistro(archivoLocal2, ARCHIVO2_REGISTRO1_3);
	escribirRegistro(archivoLocal2, ARCHIVO2_REGISTRO2_4);
	cerrarArchivo(archivoLocal2);

	archivoLocal1 = (Archivo *) newArchivoLocal(PATH_ARCHIVO_1, LECTURA);
	archivoLocal2 = (Archivo *) newArchivoLocal(PATH_ARCHIVO_2, LECTURA);

	list_add(listaArchivos, archivoLocal1);
	list_add(listaArchivos, archivoLocal2);
	archivoApareado = (Archivo *) newArchivoApareado(listaArchivos);

	// Fin de crear archivo apareado


}

void cerrarUnArchivo(void *archivo)
{
	cerrarArchivo((Archivo *)archivo);
}

static void limpiar()
{
	list_destroy_and_destroy_elements(listaArchivos, cerrarUnArchivo);
}



void siSoloHayUnArchivoSeLeeElContenidoDeEseArchivo()
{
	inicializar();
	Archivo *archivo = (Archivo *) newArchivoLocal(PATH_ARCHIVO_1, LECTURA);
	t_list *listaDe1Archivo = list_create();
	list_add(listaDe1Archivo, archivo);

	Archivo *unArchivoApareado = (Archivo *) newArchivoApareado(listaDe1Archivo);

	char *registro0 = obtenerRegistro(unArchivoApareado);
	char *registro1 = obtenerRegistro(unArchivoApareado);
	char *registro2 = obtenerRegistro(unArchivoApareado);

	CU_ASSERT_STRING_EQUAL(registro0, ARCHIVO1_REGISTRO0_0);
	CU_ASSERT_STRING_EQUAL(registro1, ARCHIVO1_REGISTRO1_2);
	CU_ASSERT_STRING_EQUAL(registro2, ARCHIVO1_REGISTRO2_5);
}

void obtieneLosRegistrosApareandoDeManeraCreciente()
{
	inicializar();

	char *registro0 = obtenerRegistro(archivoApareado);
	char *registro1 = obtenerRegistro(archivoApareado);
	char *registro2 = obtenerRegistro(archivoApareado);
	char *registro3 = obtenerRegistro(archivoApareado);
	char *registro4 = obtenerRegistro(archivoApareado);
	char *registro5 = obtenerRegistro(archivoApareado);

	CU_ASSERT_STRING_EQUAL(registro0, ARCHIVO1_REGISTRO0_0);
	CU_ASSERT_STRING_EQUAL(registro1, ARCHIVO2_REGISTRO0_1);
	CU_ASSERT_STRING_EQUAL(registro2, ARCHIVO1_REGISTRO1_2);
	CU_ASSERT_STRING_EQUAL(registro3, ARCHIVO2_REGISTRO1_3);
	CU_ASSERT_STRING_EQUAL(registro4, ARCHIVO2_REGISTRO2_4);
	CU_ASSERT_STRING_EQUAL(registro5, ARCHIVO1_REGISTRO2_5);

	limpiar();
}

void devuelveNULLSiNoHayMasRegistrosEnNingunArchivo()
{
	inicializar();

	int leidas;
	for(leidas = 0; leidas < 8; leidas ++)
	{
		char *registro = obtenerRegistro(archivoApareado);
		free(registro);
	}

	char *registroQueDeberiaSerNulo = obtenerRegistro(archivoApareado);

	CU_ASSERT_PTR_EQUAL(registroQueDeberiaSerNulo, NULL);

	limpiar();
}

void devuelveNULLSiSeLeeMuchasVecesAnqueNoHayaMasRegistrosEnNingunArchivo()
{
	inicializar();

	int leidas;
	for(leidas = 0; leidas < 6; leidas ++)
	{
		char *registro = obtenerRegistro(archivoApareado);
		free(registro);
	}

	char *registroQueDeberiaSerNulo = obtenerRegistro(archivoApareado);
	registroQueDeberiaSerNulo = obtenerRegistro(archivoApareado);
	registroQueDeberiaSerNulo = obtenerRegistro(archivoApareado);
	registroQueDeberiaSerNulo = obtenerRegistro(archivoApareado);

	CU_ASSERT_PTR_EQUAL(registroQueDeberiaSerNulo, NULL);

	limpiar();
}


void agregarTestsDeArchivoApareado(CU_pSuite suite)
{
	CU_ADD_TEST(suite, siSoloHayUnArchivoSeLeeElContenidoDeEseArchivo);
	CU_ADD_TEST(suite, obtieneLosRegistrosApareandoDeManeraCreciente);
	CU_ADD_TEST(suite, devuelveNULLSiNoHayMasRegistrosEnNingunArchivo);
	CU_ADD_TEST(suite, devuelveNULLSiSeLeeMuchasVecesAnqueNoHayaMasRegistrosEnNingunArchivo);
}
