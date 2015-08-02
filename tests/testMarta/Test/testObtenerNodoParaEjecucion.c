#include <CUnit/Basic.h>
#include "cargaNodos.h"
#include "nodo.h"
#include "tarea.h"
#include "archivoRemoto.h"


void cargaNodosCon2NodosLibres_pidoNodoDisponible_obtengoPrimerNodo()
{
	t_cargaNodos *cargaNodos = newCargaNodos();

	t_nodo *nodo1 = newNodo("nodo1", "129.168.1.101", "5001");
	t_nodo *nodo2 = newNodo("nodo2", "129.168.1.102", "5002");

	cargaNodosAgregarNodo(cargaNodos, nodo1);
	cargaNodosAgregarNodo(cargaNodos, nodo2);

	t_list *listaNodosParaMap = list_create();
	list_add(listaNodosParaMap, newNodo("nodo1", "129.168.1.101", "5001"));
	list_add(listaNodosParaMap, newNodo("nodo2", "129.168.1.101", "5001"));

	t_nodo *nodoDisponible = cargaNodosObtenerNodoDisponibleParaMap(cargaNodos, listaNodosParaMap);

	CU_ASSERT_STRING_EQUAL(nodoDisponible->nombre, nodo1->nombre);
}

void cargaNodosCon1NodoCargadoYOtroLibre_pidoNodoDisponible_obtengoNodoLibre()
{
	t_cargaNodos *cargaNodos = newCargaNodos();

	t_nodo *nodo1 = newNodo("nodo1", "129.168.1.101", "5001");
	t_nodo *nodo2 = newNodo("nodo2", "129.168.1.102", "5002");

	cargaNodosAgregarNodo(cargaNodos, nodo1);
	cargaNodosAgregarNodo(cargaNodos, nodo2);

	nodoAsignarCargaMap(nodo1);

	t_list *listaNodosParaMap = list_create();
	list_add(listaNodosParaMap, newNodo("nodo1", "129.168.1.101", "5001"));
	list_add(listaNodosParaMap, newNodo("nodo2", "129.168.1.101", "5001"));

	t_nodo *nodoDisponible = cargaNodosObtenerNodoDisponibleParaMap(cargaNodos, listaNodosParaMap);

	CU_ASSERT_STRING_EQUAL(nodoDisponible->nombre, nodo2->nombre);
}

void unNodoCargadoAlMaximo_pidoNodoDisponibleParaMap_noHayNodosDisponibles()
{
	setCargaMaxima(1);
	setCargaDeMap(1);

	t_cargaNodos *cargaNodos = newCargaNodos();

	t_nodo *nodo1 = newNodo("nodo1", "129.168.1.101", "5001");

	cargaNodosAgregarNodo(cargaNodos, nodo1);

	nodoAsignarCargaMap(nodo1);

	t_list *listaNodosParaMap = list_create();
	list_add(listaNodosParaMap, newNodo("nodo1", "129.168.1.101", "5001"));

	t_nodo *nodoDisponible = cargaNodosObtenerNodoDisponibleParaMap(cargaNodos, listaNodosParaMap);

	CU_ASSERT_PTR_EQUAL(nodoDisponible,NULL);

}

void cargaMaxima3CargaReduce3CargaEnNodo1_pidoNodoDisponibleParaReduce_noHayNodosDisponibles()
{
	setCargaMaxima(3);
	setCargaDeMap(1);
	setCargaDeReduce(3);

	t_cargaNodos *cargaNodos = newCargaNodos();
	t_nodo *nodo1 = newNodo("nodo1", "129.168.1.101", "5001");
	cargaNodosAgregarNodo(cargaNodos, nodo1);
	nodoAsignarCargaMap(nodo1);

	t_list *archivosAReducir = list_create();
	list_add(archivosAReducir, newArchivoRemoto(nodo1, "temporal1"));
	t_tarea *tareaReduce = newTareaReduce(archivosAReducir, SIN_COMBINER);
	t_nodo *nodoDisponible = cargaNodosObtenerNodoDisponibleParaReduce(cargaNodos, tareaReduce);

	CU_ASSERT_PTR_EQUAL(nodoDisponible,NULL);
}

void nodo1Carga1YNodo2Carga3ConMasArchivosParaReduceSinCombiner_pidoNodoParaReduce_obtengoNodo2()
{
	setCargaMaxima(10);
	setCargaDeMap(1);
	setCargaDeReduce(3);

	t_cargaNodos *cargaNodos = newCargaNodos();
	t_nodo *nodo1 = newNodo("nodo1", "129.168.1.101", "5001");
	t_nodo *nodo2 = newNodo("nodo2", "129.168.1.101", "5001");
	cargaNodosAgregarNodo(cargaNodos, nodo1);
	cargaNodosAgregarNodo(cargaNodos, nodo2);
	nodoAsignarCargaMap(nodo1);
	nodoAsignarCargaReduce(nodo2);

	t_list *archivosAReducir = list_create();
	list_add(archivosAReducir, newArchivoRemoto(nodo1, "temporal1"));
	list_add(archivosAReducir, newArchivoRemoto(nodo2, "temporal2"));
	list_add(archivosAReducir, newArchivoRemoto(nodo2, "temporal3"));
	t_tarea *tareaReduce = newTareaReduce(archivosAReducir, SIN_COMBINER);

	t_nodo *nodoDisponible = cargaNodosObtenerNodoDisponibleParaReduce(cargaNodos, tareaReduce);

	CU_ASSERT_STRING_EQUAL(nodoDisponible->nombre, nodo2->nombre);
}

void nodo1Carga1YNodo2Carga2Nodo3Carga3_pidoNodoParaMapEnBloqueDeNodo3_obtengoNodo3()
{
	setCargaMaxima(10);
	setCargaDeMap(1);
	setCargaDeReduce(3);

	t_cargaNodos *cargaNodos = newCargaNodos();
	t_nodo *nodo1 = newNodo("nodo1", "129.168.1.101", "5001");
	t_nodo *nodo2 = newNodo("nodo2", "129.168.1.102", "5001");
	t_nodo *nodo3 = newNodo("nodo3", "129.168.1.103", "5001");
	cargaNodosAgregarNodo(cargaNodos, nodo1);
	cargaNodosAgregarNodo(cargaNodos, nodo2);
	cargaNodosAgregarNodo(cargaNodos, nodo3);

	nodoAsignarCargaMap(nodo1);
	nodoAsignarCargaMap(nodo2);
	nodoAsignarCargaMap(nodo2);
	nodoAsignarCargaMap(nodo3);
	nodoAsignarCargaMap(nodo3);
	nodoAsignarCargaMap(nodo3);

	t_list *listaNodosParaMap = list_create();
	list_add(listaNodosParaMap, newNodo("nodo3", "129.168.1.103", "5001"));

	t_nodo *nodoDisponible = cargaNodosObtenerNodoDisponibleParaMap(cargaNodos, listaNodosParaMap);

	CU_ASSERT_STRING_EQUAL(nodoDisponible->nombre, nodo3->nombre);
}

void unNodoConCargaMaximayLiberoCargaDeMap_pidoNodoParaMap_obtengoMismoNodo()
{
	setCargaMaxima(2);
	setCargaDeMap(1);

	t_cargaNodos *cargaNodos = newCargaNodos();
	t_nodo *nodo1 = newNodo("nodo1", "129.168.1.101", "5001");
	cargaNodosAgregarNodo(cargaNodos, nodo1);

	nodoAsignarCargaMap(nodo1);
	nodoAsignarCargaMap(nodo1);
	nodoLiberarCargaMap(nodo1);

	t_list *listaNodosParaMap = list_create();
	list_add(listaNodosParaMap, newNodo("nodo1", "129.168.1.101", "5001"));

	t_nodo *nodoDisponible = cargaNodosObtenerNodoDisponibleParaMap(cargaNodos, listaNodosParaMap);

	CU_ASSERT_STRING_EQUAL(nodoDisponible->nombre, nodo1->nombre);
}


void agregarTestObtenerNodoParaEjecucion(CU_pSuite suite)
{
	CU_ADD_TEST(suite, cargaNodosCon2NodosLibres_pidoNodoDisponible_obtengoPrimerNodo);
	CU_ADD_TEST(suite, cargaNodosCon1NodoCargadoYOtroLibre_pidoNodoDisponible_obtengoNodoLibre);
	CU_ADD_TEST(suite, unNodoCargadoAlMaximo_pidoNodoDisponibleParaMap_noHayNodosDisponibles);
	CU_ADD_TEST(suite, cargaMaxima3CargaReduce3CargaEnNodo1_pidoNodoDisponibleParaReduce_noHayNodosDisponibles);
	CU_ADD_TEST(suite, nodo1Carga1YNodo2Carga3ConMasArchivosParaReduceSinCombiner_pidoNodoParaReduce_obtengoNodo2);
	CU_ADD_TEST(suite, nodo1Carga1YNodo2Carga2Nodo3Carga3_pidoNodoParaMapEnBloqueDeNodo3_obtengoNodo3);
	CU_ADD_TEST(suite, unNodoConCargaMaximayLiberoCargaDeMap_pidoNodoParaMap_obtengoMismoNodo);
}
