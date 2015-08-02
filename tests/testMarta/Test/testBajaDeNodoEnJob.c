#include <CUnit/Basic.h>
#include "job.h"
#include "cargaNodos.h"

void jobCon3MapsEnEjecucion_SeBajaUnNodo_laTareaMapDeEseNodoQuedaNoEjecutada()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",3);

	t_tarea *map1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map1, newNodo("nodo1", "192.168.1.101", "5001"), "archivo1");
	t_tarea *map2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map2, newNodo("nodo2", "192.168.1.102", "5001"), "archivo2");
	t_tarea *map3 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map3, newNodo("nodo3", "192.168.1.102", "5001"), "archivo3");

	jobDarDeBajaNodo(job, "nodo3");

	CU_ASSERT_TRUE(tareaEstaNoEjecutada(map3));
}

void cargaNodosConNodo1Nodo2Nodo3_SeBajaUnNodo_elNodoNoSeEncuentraEnCargaNodos()
{
	t_cargaNodos *cargaNodos = newCargaNodos();
	cargaNodosAgregarNodo(cargaNodos, newNodo("nodo1", "192.168.1.101", "5001"));
	cargaNodosAgregarNodo(cargaNodos, newNodo("nodo2", "192.168.1.102", "5001"));
	cargaNodosAgregarNodo(cargaNodos, newNodo("nodo3", "192.168.1.103", "5001"));

	cargaNodosDarDeBajaNodo(cargaNodos, "nodo2");

	t_list *listaNodos = cargaNodosObtenerListaNodos(cargaNodos);

	t_nodo *nodo1 = list_get(listaNodos, 0);
	t_nodo *nodo3 = list_get(listaNodos, 1);

	CU_ASSERT_EQUAL(list_size(listaNodos), 2);
	CU_ASSERT_STRING_EQUAL(nodo1->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodo3->nombre, "nodo3");
}




void agregarTestBajaNodoEnJob(CU_pSuite suite)
{
	CU_ADD_TEST(suite, jobCon3MapsEnEjecucion_SeBajaUnNodo_laTareaMapDeEseNodoQuedaNoEjecutada);
	CU_ADD_TEST(suite, cargaNodosConNodo1Nodo2Nodo3_SeBajaUnNodo_elNodoNoSeEncuentraEnCargaNodos);
}
