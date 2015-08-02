#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>

void agregarTestsDeArchivoLocal(CU_pSuite suite);
void agregarTestsDeArchivoApareado(CU_pSuite suite);
void agregarTestsDeEspacioDeDatos(CU_pSuite suite);

void ejecutarPruebasDeWorkspace();


int main(int argc, char *argv[]) {
	if (argc > 1)
		ejecutarPruebasDeWorkspace(argc, argv);
	else
	{
		CU_initialize_registry();

		CU_pSuite suiteArchivoLocal = CU_add_suite("Suite de tests de TAD ArchivoLocal", NULL, NULL);
		agregarTestsDeArchivoLocal(suiteArchivoLocal);

		CU_pSuite suiteArchivoApareado = CU_add_suite("Suite de tests de TAD ArchivoApareado", NULL, NULL);
		agregarTestsDeArchivoApareado(suiteArchivoApareado);


		CU_pSuite suiteEspacioDatos = CU_add_suite("Suite de tests de TAD EspacioDatos", NULL, NULL);
		agregarTestsDeEspacioDeDatos(suiteEspacioDatos);


		CU_basic_set_mode(CU_BRM_VERBOSE);
		CU_basic_run_tests();

		CU_cleanup_registry();

		return CU_get_error();
	}

	return 0;
}
