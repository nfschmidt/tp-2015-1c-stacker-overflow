#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>

void agregarTestDeMapsJob(CU_pSuite suite);
void agregarTestReduceSinCombinerJob(CU_pSuite suite);
void agregarTestReduceConCombinerJob(CU_pSuite suite);
void agregarTestAgregadoDeNodos(CU_pSuite suite);
void agregarTestObtenerNodoParaEjecucion(CU_pSuite suite);
void agregarTestJobFinalizado(CU_pSuite suite);
void agregarTestBajaNodoEnJob(CU_pSuite suite);
void agregarTestPlanificador(CU_pSuite suite);

void workspace(char *archivoConfiguracion);


int main(int argc, char *argv[]) {

	if (argc == 1)
	{
		CU_initialize_registry();

		CU_pSuite suiteTareasJob = CU_add_suite("Suite de tests de TAD Job y Tareas", NULL, NULL);
		agregarTestDeMapsJob(suiteTareasJob);

		CU_pSuite suiteReduceSinCombinerJob = CU_add_suite("Suite de tests de Reduce sin combiner", NULL, NULL);
		agregarTestReduceSinCombinerJob(suiteReduceSinCombinerJob);

		CU_pSuite suiteReduceConCombinerJob = CU_add_suite("Suite de tests de Reduce con combiner", NULL, NULL);
		agregarTestReduceConCombinerJob(suiteReduceConCombinerJob);

		CU_pSuite suiteAgregadoNodos = CU_add_suite("Suite de tests de agregado de nodos a lista", NULL, NULL);
		agregarTestAgregadoDeNodos(suiteAgregadoNodos);

		CU_pSuite suiteObtenerNodoParaEjecucion = CU_add_suite("Suite de tests de obtencion de nodos para ejecucion", NULL, NULL);
		agregarTestObtenerNodoParaEjecucion(suiteObtenerNodoParaEjecucion);

		CU_pSuite suiteJobFinalizado = CU_add_suite("Suite de tests de finalizacion de job", NULL, NULL);
		agregarTestJobFinalizado(suiteJobFinalizado);

		CU_pSuite suiteBajaNodoEnJob = CU_add_suite("Suite de tests de baja de nodo en job", NULL, NULL);
		agregarTestBajaNodoEnJob(suiteBajaNodoEnJob);

		CU_pSuite suitePlanificador = CU_add_suite("Suite de tests de planificador", NULL, NULL);
		agregarTestPlanificador(suitePlanificador);

		CU_basic_set_mode(CU_BRM_VERBOSE);
		CU_basic_run_tests();

		CU_cleanup_registry();

		return CU_get_error();
	}
	else
	{
		workspace(argv[1]);
	}

	return 0;
}
