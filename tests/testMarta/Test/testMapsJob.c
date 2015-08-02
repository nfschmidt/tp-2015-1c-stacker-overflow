#include <CUnit/Basic.h>
#include "tarea.h"
#include "job.h"


void job1Archivo1Bloque_pedimosSiguienteTarea_esTareaMapEnBloque0()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",1);

	t_tarea *tarea = jobObtenerSiguienteTarea(job);

	CU_ASSERT_TRUE(tareaEsMap(tarea));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tarea),0);
	CU_ASSERT_STRING_EQUAL(tareaObtenerNombreArchivoMDFS(tarea),"/holy.txt");
}


void job1Archivo2Bloques1TareaEnEjecucion_pedimosSiguienteTarea_esTareaMapEnBloque1()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",2);

	t_tarea *tarea = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea, newNodo("nodo1", "192.168.1.101", "5001"), "archivo1");

	t_tarea *otraTarea = jobObtenerSiguienteTarea(job);

	CU_ASSERT_TRUE(tareaEsMap(otraTarea));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(otraTarea), 1);
	CU_ASSERT_STRING_EQUAL(tareaObtenerNombreArchivoMDFS(otraTarea),"/holy.txt");
}


void job1Archivo2Bloques2TareaEnEjecucion_pedimosSiguienteTarea_noHayMasTareasDisponibles()
{
	t_job *job = newJob(SIN_COMBINER);

	jobAgregarArchivoMDFS(job,"/holy.txt",2);

	t_tarea *tarea = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea, newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	t_tarea *otraTarea = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(otraTarea,newNodo("nodo1", "192.168.1.101", "5001"),"archivo2");
	t_tarea *tercerTarea = jobObtenerSiguienteTarea(job);

	CU_ASSERT_PTR_EQUAL(tercerTarea,NULL);
}

void jobCon2Archivos1Bloque_pedimos2Tareas_tareaBloque0DeCadaArchivo()
{

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",1);
	jobAgregarArchivoMDFS(job,"/holy2.txt",1);


	t_tarea *tarea = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	t_tarea *otraTarea = jobObtenerSiguienteTarea(job);

	CU_ASSERT_TRUE(tareaEsMap(tarea));
	CU_ASSERT_STRING_EQUAL(tareaObtenerNombreArchivoMDFS(tarea),"/holy.txt");
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tarea),0);


	CU_ASSERT_TRUE(tareaEsMap(otraTarea));
	CU_ASSERT_STRING_EQUAL(tareaObtenerNombreArchivoMDFS(otraTarea),"/holy2.txt");
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(otraTarea),0);


}

void jobCon2Archivos2Bloque_pedimos4Tareas_tareaBloque0y1DeCadaArchivo()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",2);
	jobAgregarArchivoMDFS(job,"/holy2.txt",2);


	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	t_tarea *tarea2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea2,newNodo("nodo1", "192.168.1.101", "5001"),"archivo2");
	t_tarea *tarea3 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea3, newNodo("nodo1", "192.168.1.101", "5001"),"archivo3");
	t_tarea *tarea4 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea4,newNodo("nodo1", "192.168.1.101", "5001"),"archivo4");

	CU_ASSERT_TRUE(tareaEsMap(tarea1));
	CU_ASSERT_STRING_EQUAL(tareaObtenerNombreArchivoMDFS(tarea1),"/holy.txt");
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tarea1),0);

	CU_ASSERT_TRUE(tareaEsMap(tarea2));
	CU_ASSERT_STRING_EQUAL(tareaObtenerNombreArchivoMDFS(tarea2),"/holy.txt");
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tarea2),1);

	CU_ASSERT_TRUE(tareaEsMap(tarea3));
	CU_ASSERT_STRING_EQUAL(tareaObtenerNombreArchivoMDFS(tarea3),"/holy2.txt");
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tarea3),0);

	CU_ASSERT_TRUE(tareaEsMap(tarea4));
	CU_ASSERT_STRING_EQUAL(tareaObtenerNombreArchivoMDFS(tarea4),"/holy2.txt");
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tarea4),1);


}

void jobCon1Archivos2BloquesY2TareasEnEjecucion_fallaUnaTarea_devuelveTareaQueFallo()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",2);



	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	t_tarea *tarea2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea2,newNodo("nodo1", "192.168.1.101", "5001"),"archivo2");

	tareaMarcarNoEjecutada(tarea1);
	t_tarea *tarea3 = jobObtenerSiguienteTarea(job);

	CU_ASSERT_TRUE(tareaEsMap(tarea3));
	CU_ASSERT_STRING_EQUAL(tareaObtenerNombreArchivoMDFS(tarea3),tareaObtenerNombreArchivoMDFS(tarea1));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tarea3),tareaMapObtenerBloque(tarea1));
}

void jobCon3TareasMap_pidoTareaConIdEspecifico_devuelveTareaMapConEseId()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",3);

	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tarea1->id = 1;
	t_tarea *tarea2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea2,newNodo("nodo1", "192.168.1.101", "5001"),"archivo2");
	tarea2->id = 2;
	t_tarea *tarea3 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea2,newNodo("nodo1", "192.168.1.101", "5001"),"archivo3");
	tarea3->id = 3;

	t_tarea *tareaRecibida = jobObtenerTareaConId(job, 2);

	CU_ASSERT_PTR_EQUAL(tareaRecibida, tarea2);
}



void agregarTestDeMapsJob(CU_pSuite suite)
{
	CU_ADD_TEST(suite,job1Archivo1Bloque_pedimosSiguienteTarea_esTareaMapEnBloque0);
	CU_ADD_TEST(suite, job1Archivo2Bloques1TareaEnEjecucion_pedimosSiguienteTarea_esTareaMapEnBloque1);
	CU_ADD_TEST(suite,job1Archivo2Bloques2TareaEnEjecucion_pedimosSiguienteTarea_noHayMasTareasDisponibles);
	CU_ADD_TEST(suite,jobCon2Archivos1Bloque_pedimos2Tareas_tareaBloque0DeCadaArchivo);
	CU_ADD_TEST(suite,jobCon2Archivos2Bloque_pedimos4Tareas_tareaBloque0y1DeCadaArchivo);
	CU_ADD_TEST(suite,jobCon1Archivos2BloquesY2TareasEnEjecucion_fallaUnaTarea_devuelveTareaQueFallo);
	CU_ADD_TEST(suite, jobCon3TareasMap_pidoTareaConIdEspecifico_devuelveTareaMapConEseId);
}




