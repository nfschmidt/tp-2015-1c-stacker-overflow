#include <CUnit/Basic.h>
#include "job.h"
#include "tarea.h"

void jobSinCombiner_noSeEjecutoReduceFinal_jobNoEstaTerminado()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 1);

	t_tarea *map1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map1, newNodo("nodo1", "192.168.1.101", "5001"), "archivo1");
	tareaMarcarFinalizada(map1);

	CU_ASSERT_FALSE(jobEstaTerminado(job));
}

void jobSinCombiner_reduceFinalEnEjecucion_jobNoEstaTerminado()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 1);

	t_tarea *map1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map1, newNodo("nodo1", "192.168.1.101", "5001"), "archivo1");
	tareaMarcarFinalizada(map1);

	t_tarea *reduceFinal = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(reduceFinal, newNodo("nodo1", "192.168.1.101", "5001"), "reduce1");

	CU_ASSERT_FALSE(jobEstaTerminado(job));
}

void jobSinCombiner_ejecutaReduceFinal_jobEstaTerminado()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 1);

	t_tarea *map1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map1, newNodo("nodo1", "192.168.1.101", "5001"), "archivo1");
	tareaMarcarFinalizada(map1);

	t_tarea *reduceFinal = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(reduceFinal, newNodo("nodo1", "192.168.1.101", "5001"), "reduce1");
	tareaMarcarFinalizada(reduceFinal);

	CU_ASSERT_TRUE(jobEstaTerminado(job));
}

void jobConCombiner_noSeEjecutoReduceFinal_jobNoEstaTerminado()
{
	t_job *job = newJob(CON_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 1);

	t_tarea *map1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map1, newNodo("nodo1", "192.168.1.101", "5001"), "archivo1");
	tareaMarcarFinalizada(map1);

	CU_ASSERT_FALSE(jobEstaTerminado(job));
}

void jobConCombiner_reduceFinalEnEjecucion_jobNoEstaTerminado()
{
	t_job *job = newJob(CON_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 1);

	t_tarea *map1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map1, newNodo("nodo1", "192.168.1.101", "5001"), "archivo1");
	tareaMarcarFinalizada(map1);

	t_tarea *reduceIntermedio = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(reduceIntermedio, newNodo("nodo1", "192.168.1.101", "5001"), "reduce1");
	tareaMarcarFinalizada(reduceIntermedio);

	t_tarea *reduceFinal = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(reduceFinal, newNodo("nodo1", "192.168.1.101", "5001"), "reduceFinal");

	CU_ASSERT_FALSE(jobEstaTerminado(job));
}

void jobConCombiner_ejecutaReduceFinal_jobEstaTerminado()
{
	t_job *job = newJob(CON_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 1);

	t_tarea *map1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map1, newNodo("nodo1", "192.168.1.101", "5001"), "archivo1");
	tareaMarcarFinalizada(map1);

	t_tarea *reduceIntermedio = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(reduceIntermedio, newNodo("nodo1", "192.168.1.101", "5001"), "reduce1");
	tareaMarcarFinalizada(reduceIntermedio);

	t_tarea *reduceFinal = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(reduceFinal, newNodo("nodo1", "192.168.1.101", "5001"), "reduceFinal");
	tareaMarcarFinalizada(reduceFinal);

	CU_ASSERT_TRUE(jobEstaTerminado(job));
}




void agregarTestJobFinalizado(CU_pSuite suite)
{
	CU_ADD_TEST(suite, jobSinCombiner_ejecutaReduceFinal_jobEstaTerminado);
	CU_ADD_TEST(suite, jobSinCombiner_noSeEjecutoReduceFinal_jobNoEstaTerminado);
	CU_ADD_TEST(suite, jobSinCombiner_reduceFinalEnEjecucion_jobNoEstaTerminado);
	CU_ADD_TEST(suite, jobConCombiner_noSeEjecutoReduceFinal_jobNoEstaTerminado);
	CU_ADD_TEST(suite, jobConCombiner_reduceFinalEnEjecucion_jobNoEstaTerminado);
	CU_ADD_TEST(suite, jobConCombiner_ejecutaReduceFinal_jobEstaTerminado);
}
