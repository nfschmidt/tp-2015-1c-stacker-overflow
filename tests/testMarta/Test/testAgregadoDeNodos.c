#include <CUnit/Basic.h>
#include <commons/collections/list.h>
#include "cargaNodos.h"
#include "nodo.h"


void listaNodosVacia_seAgrega1Nodo_tamanioListaEs1()
{
	t_cargaNodos *cargaDeNodos = newCargaNodos();
	t_nodo *nodo = newNodo("nodo1","192.168.1.101","5000");
	cargaNodosAgregarNodo(cargaDeNodos,nodo);

	t_list *listaDeNodos = cargaNodosObtenerListaNodos(cargaDeNodos);
	CU_ASSERT_EQUAL(list_size(listaDeNodos),1);
}

void listaNodosCon1Nodo_seAgregaMismoNodo_tamanioListaEs1()
{
	t_cargaNodos *cargaDeNodos = newCargaNodos();
	t_nodo *nodo = newNodo("nodo1","192.168.1.101","5000");
	cargaNodosAgregarNodo(cargaDeNodos,nodo);

	t_nodo *nodo2 = newNodo("nodo1","192.168.1.101","5000");
	cargaNodosAgregarNodo(cargaDeNodos,nodo2);

	t_list *listaDeNodos = cargaNodosObtenerListaNodos(cargaDeNodos);
	CU_ASSERT_EQUAL(list_size(listaDeNodos),1);
}

void listaNodosCon1Nodo_seAgregaNodoDistinto_tamanioListaEs2()
{
	t_cargaNodos *cargaDeNodos = newCargaNodos();
	t_nodo *nodo = newNodo("nodo1","192.168.1.101","5000");
	cargaNodosAgregarNodo(cargaDeNodos, nodo);

	t_nodo *nodo2 = newNodo("nodo2","192.168.1.101","5000");
	cargaNodosAgregarNodo(cargaDeNodos, nodo2);

	t_list *listaDeNodos = cargaNodosObtenerListaNodos(cargaDeNodos);
	CU_ASSERT_EQUAL(list_size(listaDeNodos), 2);
}

void cargaNodosSinNodos_pidoNodoParaMapConListaDe2Nodos_LaListaDeNodosDeCargaNodosTieneLos2Nodos()
{
	setCargaMaxima(10);
	setCargaDeMap(1);
	setCargaDeReduce(3);

	t_cargaNodos *cargaNodos = newCargaNodos();

	t_nodo *nodo1 = newNodo("nodo1", "129.168.1.101", "5001");
	t_nodo *nodo2 = newNodo("nodo2", "129.168.1.102", "5001");

	t_list *listaNodosParaMap = list_create();
	list_add(listaNodosParaMap, nodo1);
	list_add(listaNodosParaMap, nodo2);

	cargaNodosObtenerNodoDisponibleParaMap(cargaNodos, listaNodosParaMap);

	t_list *listaNodosDeCargaNodos = cargaNodosObtenerListaNodos(cargaNodos);
	CU_ASSERT_EQUAL(list_size(listaNodosDeCargaNodos), 2);
	CU_ASSERT_STRING_EQUAL(((t_nodo*) list_get(listaNodosDeCargaNodos, 0))->nombre, nodo1->nombre)
	CU_ASSERT_STRING_EQUAL(((t_nodo*) list_get(listaNodosDeCargaNodos, 1))->nombre, nodo2->nombre)
}

void cargaNodosCon1Nodo_pidoNodoParaMapConListaDe2NodosCon1Repetido_LaListaDeNodosDeCargaNodosTieneLos2NodosDistintos()
{
	setCargaMaxima(10);
	setCargaDeMap(1);
	setCargaDeReduce(3);

	t_cargaNodos *cargaNodos = newCargaNodos();
	cargaNodosAgregarNodo(cargaNodos, newNodo("nodo1", "129.168.1.101", "5001"));

	t_nodo *nodo1 = newNodo("nodo1", "129.168.1.101", "5001");
	t_nodo *nodo2 = newNodo("nodo2", "129.168.1.102", "5001");

	t_list *listaNodosParaMap = list_create();
	list_add(listaNodosParaMap, nodo1);
	list_add(listaNodosParaMap, nodo2);

	cargaNodosObtenerNodoDisponibleParaMap(cargaNodos, listaNodosParaMap);

	t_list *listaNodosDeCargaNodos = cargaNodosObtenerListaNodos(cargaNodos);
	CU_ASSERT_EQUAL(list_size(listaNodosDeCargaNodos), 2);
	CU_ASSERT_STRING_EQUAL(((t_nodo*) list_get(listaNodosDeCargaNodos, 0))->nombre, nodo1->nombre)
	CU_ASSERT_STRING_EQUAL(((t_nodo*) list_get(listaNodosDeCargaNodos, 1))->nombre, nodo2->nombre)
}




void agregarTestAgregadoDeNodos(CU_pSuite suite)
{
	CU_ADD_TEST(suite, listaNodosVacia_seAgrega1Nodo_tamanioListaEs1);
	CU_ADD_TEST(suite, listaNodosCon1Nodo_seAgregaMismoNodo_tamanioListaEs1);
	CU_ADD_TEST(suite, listaNodosCon1Nodo_seAgregaNodoDistinto_tamanioListaEs2);
	CU_ADD_TEST(suite, cargaNodosSinNodos_pidoNodoParaMapConListaDe2Nodos_LaListaDeNodosDeCargaNodosTieneLos2Nodos);
	CU_ADD_TEST(suite, cargaNodosCon1Nodo_pidoNodoParaMapConListaDe2NodosCon1Repetido_LaListaDeNodosDeCargaNodosTieneLos2NodosDistintos);
}
