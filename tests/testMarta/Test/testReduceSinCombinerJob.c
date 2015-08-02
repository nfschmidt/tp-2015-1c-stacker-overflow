#include <CUnit/Basic.h>
#include "job.h"
#include "tarea.h"
#include "archivoRemoto.h"


void job1Archivo1BloqueYConMapEjecutado_pedirSiguienteTarea_devuelveTareaReduce()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",1);

	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tareaMarcarFinalizada(tarea1);

	t_tarea *tarea2 = jobObtenerSiguienteTarea(job);

	CU_ASSERT_TRUE(tareaEsReduce(tarea2));

	t_list *archivosRemotos = tareaReduceObtenerArchivosRemotos(tarea2);

	t_archivoRemoto *archivo = list_get(archivosRemotos,0);

	CU_ASSERT_EQUAL(list_size(archivosRemotos),1);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo),"archivo1");
}

void job2Archivo1BloqueYConMapsEjecutados_pedirSiguienteTarea_devuelveTareaReduce()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",1);
	jobAgregarArchivoMDFS(job,"/holy2.txt",1);

	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tareaMarcarFinalizada(tarea1);

	t_tarea *tarea2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea2,newNodo("nodo1", "192.168.1.101", "5001"),"archivo2");
	tareaMarcarFinalizada(tarea2);

	t_tarea *tareaReduce = jobObtenerSiguienteTarea(job);

	t_list *archivosRemotos = tareaReduceObtenerArchivosRemotos(tareaReduce);
	CU_ASSERT_TRUE(tareaEsReduce(tareaReduce));
	CU_ASSERT_EQUAL(list_size(archivosRemotos),2);

	t_archivoRemoto *archivo1 = list_get(archivosRemotos,0);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo1)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo1),"archivo1");

	t_archivoRemoto *archivo2 = list_get(archivosRemotos,1);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo2)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo2),"archivo2");


}

void job2Archivo2BloqueYConMapsEjecutados_pedirSiguienteTarea_devuelveTareaReduce()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",2);
	jobAgregarArchivoMDFS(job,"/holy2.txt",2);

	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tareaMarcarFinalizada(tarea1);

	t_tarea *tarea2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea2,newNodo("nodo1", "192.168.1.101", "5001"),"archivo2");
	tareaMarcarFinalizada(tarea2);

	t_tarea *tarea3 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea3,newNodo("nodo1", "192.168.1.101", "5001"),"archivo3");
	tareaMarcarFinalizada(tarea3);

	t_tarea *tarea4 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea4,newNodo("nodo1", "192.168.1.101", "5001"),"archivo4");
	tareaMarcarFinalizada(tarea4);


	t_tarea *tareaReduce = jobObtenerSiguienteTarea(job);

	t_list *archivosRemotos = tareaReduceObtenerArchivosRemotos(tareaReduce);
	CU_ASSERT_TRUE(tareaEsReduce(tareaReduce));
	CU_ASSERT_EQUAL(list_size(archivosRemotos),4);

	t_archivoRemoto *archivo1 = list_get(archivosRemotos,0);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo1)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo1),"archivo1");

	t_archivoRemoto *archivo2 = list_get(archivosRemotos,1);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo2)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo2),"archivo2");

	t_archivoRemoto *archivo3 = list_get(archivosRemotos,2);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo3)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo3),"archivo3");

	t_archivoRemoto *archivo4 = list_get(archivosRemotos,3);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo4)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo4),"archivo4");


}

void job2Archivo2BloqueYConMapsEjecutadosEnDistintosNodos_pedirSiguienteTarea_devuelveTareaReduce()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",2);
	jobAgregarArchivoMDFS(job,"/holy2.txt",2);

	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tareaMarcarFinalizada(tarea1);

	t_tarea *tarea2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea2,newNodo("nodo2", "192.168.1.102", "5001"),"archivo2");
	tareaMarcarFinalizada(tarea2);

	t_tarea *tarea3 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea3,newNodo("nodo1", "192.168.1.101", "5001"),"archivo3");
	tareaMarcarFinalizada(tarea3);

	t_tarea *tarea4 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea4,newNodo("nodo2", "192.168.1.102", "5001"),"archivo4");
	tareaMarcarFinalizada(tarea4);


	t_tarea *tareaReduce = jobObtenerSiguienteTarea(job);

	t_list *archivosRemotos = tareaReduceObtenerArchivosRemotos(tareaReduce);
	CU_ASSERT_TRUE(tareaEsReduce(tareaReduce));
	CU_ASSERT_EQUAL(list_size(archivosRemotos),4);

	t_archivoRemoto *archivo1 = list_get(archivosRemotos,0);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo1)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo1),"archivo1");

	t_archivoRemoto *archivo2 = list_get(archivosRemotos,1);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo2)->nombre,"nodo2");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo2),"archivo2");

	t_archivoRemoto *archivo3 = list_get(archivosRemotos,2);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo3)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo3),"archivo3");

	t_archivoRemoto *archivo4 = list_get(archivosRemotos,3);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo4)->nombre,"nodo2");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo4),"archivo4");


}

void job1Archivo1BloqueYConReduceEnEjecucion_pedirSiguienteTarea_noHayMasTareas()
{
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",1);

	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tareaMarcarFinalizada(tarea1);

	t_tarea *tareaReduce = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tareaReduce,newNodo("nodo1", "192.168.1.101", "5001"),"resultadoFinal");

	t_tarea *tareaInexistente = jobObtenerSiguienteTarea(job);

	CU_ASSERT_PTR_EQUAL(tareaInexistente,NULL);
}




void agregarTestReduceSinCombinerJob(CU_pSuite suite)
{

	CU_ADD_TEST(suite,job1Archivo1BloqueYConMapEjecutado_pedirSiguienteTarea_devuelveTareaReduce);
	CU_ADD_TEST(suite,job2Archivo1BloqueYConMapsEjecutados_pedirSiguienteTarea_devuelveTareaReduce);
	CU_ADD_TEST(suite,job2Archivo2BloqueYConMapsEjecutados_pedirSiguienteTarea_devuelveTareaReduce);
	CU_ADD_TEST(suite,job2Archivo2BloqueYConMapsEjecutadosEnDistintosNodos_pedirSiguienteTarea_devuelveTareaReduce);
	CU_ADD_TEST(suite,job1Archivo1BloqueYConReduceEnEjecucion_pedirSiguienteTarea_noHayMasTareas);
}
