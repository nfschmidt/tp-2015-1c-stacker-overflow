#include <CUnit/Basic.h>
#include "job.h"
#include "tarea.h"
#include "archivoRemoto.h"

void jobConCombiner1Archivo1BloqueYConMapEjecutado_pedirSiguienteTarea_devuelveTareaReduce()
{
	t_job *job = newJob(CON_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",1);

	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1, newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tareaMarcarFinalizada(tarea1);

	t_tarea *tarea2 = jobObtenerSiguienteTarea(job);

	CU_ASSERT_TRUE(tareaEsReduce(tarea2));

	t_list *archivosRemotos = tareaReduceObtenerArchivosRemotos(tarea2);

	t_archivoRemoto *archivo = list_get(archivosRemotos,0);

	CU_ASSERT_EQUAL(list_size(archivosRemotos),1);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo),"archivo1");
}

void jobConCombiner1Archivo2BloqueYConMapEjecutado_pedirSiguienteTarea_devuelveTareaReduce()
{
	t_job *job = newJob(CON_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",2);

	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1, newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tareaMarcarFinalizada(tarea1);


	t_tarea *tarea2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea2,newNodo("nodo1", "192.168.1.101", "5001"),"archivo2");
	tareaMarcarFinalizada(tarea2);

	t_tarea *tarea3 = jobObtenerSiguienteTarea(job);

	CU_ASSERT_TRUE(tareaEsReduce(tarea3));

	t_list *archivosRemotos = tareaReduceObtenerArchivosRemotos(tarea3);

	t_archivoRemoto *archivo = list_get(archivosRemotos,0);

	CU_ASSERT_EQUAL(list_size(archivosRemotos),2);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo),"archivo1");

	t_archivoRemoto *archivo2 = list_get(archivosRemotos,1);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo2)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo2),"archivo2");
}

void jobConCombiner1Archivo2BloqueYConMapEjecutadoEnDistintosNodos_pedirSiguienteTarea_devuelveTareaReduce()
{
	t_job *job = newJob(CON_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",2);

	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tareaMarcarFinalizada(tarea1);


	t_tarea *tarea2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea2,newNodo("nodo2", "192.168.1.102", "5001"),"archivo2");
	tareaMarcarFinalizada(tarea2);

	t_tarea *tarea3 = jobObtenerSiguienteTarea(job);

	CU_ASSERT_TRUE(tareaEsReduce(tarea3));

	t_list *archivosRemotos = tareaReduceObtenerArchivosRemotos(tarea3);

	t_archivoRemoto *archivo = list_get(archivosRemotos,0);

	CU_ASSERT_EQUAL(list_size(archivosRemotos),1);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo),"archivo1");



}

void jobConCombiner1Archivo2BloqueYConPrimerReduceEjecutado_pedirSiguienteTarea_devuelveTareaReduce()
{
	t_job *job = newJob(CON_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",2);

	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tareaMarcarFinalizada(tarea1);


	t_tarea *tarea2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea2,newNodo("nodo2", "192.168.1.101", "5001"),"archivo2");
	tareaMarcarFinalizada(tarea2);

	t_tarea *tarea3 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea3,newNodo("nodo1", "192.168.1.101", "5001"),"reduce1");
	tareaMarcarFinalizada(tarea3);

	t_tarea *tarea4 = jobObtenerSiguienteTarea(job);

	CU_ASSERT_TRUE(tareaEsReduce(tarea4));

	t_list *archivosRemotos = tareaReduceObtenerArchivosRemotos(tarea4);

	t_archivoRemoto *archivo = list_get(archivosRemotos,0);

	CU_ASSERT_EQUAL(list_size(archivosRemotos),1);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo)->nombre,"nodo2");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo),"archivo2");
}

void jobConCombiner1Archivo2BloqueYPrimerReduceFalla_pedirSiguienteTarea_devuelveMismoReduce()
{
	t_job *job = newJob(CON_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",2);

	t_tarea *tarea1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tareaMarcarFinalizada(tarea1);

	t_tarea *tarea2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea2,newNodo("nodo2", "192.168.1.101", "5001"),"archivo2");
	tareaMarcarFinalizada(tarea2);

	t_tarea *tarea3 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(tarea3,newNodo("nodo1", "192.168.1.101", "5001"),"reduce1");
	tareaMarcarNoEjecutada(tarea3);

	t_tarea *tarea4 = jobObtenerSiguienteTarea(job);

	CU_ASSERT_PTR_EQUAL(tarea4, tarea3);
}

void jobConCombiner2Archivos2Bloques2NodosYMapsFinalizados_pedimosSiguientesTareas_devuelve2Reduces()
{
	t_job *job = newJob(CON_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",2);
	jobAgregarArchivoMDFS(job,"/holy2.txt",2);

	t_tarea *map1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tareaMarcarFinalizada(map1);

	t_tarea *map2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map2,newNodo("nodo2", "192.168.1.102", "5001"),"archivo2");
	tareaMarcarFinalizada(map2);

	t_tarea *map3 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map3,newNodo("nodo1", "192.168.1.101", "5001"),"archivo3");
	tareaMarcarFinalizada(map3);

	t_tarea *map4 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map4,newNodo("nodo2", "192.168.1.102", "5001"),"archivo4");
	tareaMarcarFinalizada(map4);

	t_tarea *reduce1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(reduce1,newNodo("nodo1", "192.168.1.101", "5001"),"reduce1");

	t_tarea *reduce2 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(reduce2,newNodo("nodo2", "192.168.1.102", "5001"),"reduce2");

	CU_ASSERT_TRUE(tareaEsReduce(reduce1));
	t_list *archivosRemotosReduce1 = tareaReduceObtenerArchivosRemotos(reduce1);
	CU_ASSERT_EQUAL(list_size(archivosRemotosReduce1),2);

	t_archivoRemoto *archivo1Reduce1 = list_get(archivosRemotosReduce1,0);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo1Reduce1)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo1Reduce1),"archivo1");

	t_archivoRemoto *archivo2Reduce1 = list_get(archivosRemotosReduce1, 1);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo2Reduce1)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo2Reduce1),"archivo3");



	CU_ASSERT_TRUE(tareaEsReduce(reduce2));
	t_list *archivosRemotosReduce2 = tareaReduceObtenerArchivosRemotos(reduce2);
	CU_ASSERT_EQUAL(list_size(archivosRemotosReduce2),2);

	t_archivoRemoto *archivo1Reduce2 = list_get(archivosRemotosReduce2,0);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo1Reduce2)->nombre,"nodo2");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo1Reduce2),"archivo2");

	t_archivoRemoto *archivo2Reduce2 = list_get(archivosRemotosReduce2, 1);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo2Reduce2)->nombre,"nodo2");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo2Reduce2),"archivo4");

}


void jobConCombiner1Archivo1BloqueYConReduceEjecutado_pedirSiguienteTarea_devuelveReduceFinal()
{
	t_job *job = newJob(CON_COMBINER);
	jobAgregarArchivoMDFS(job,"/holy.txt",1);

	t_tarea *map1 = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(map1,newNodo("nodo1", "192.168.1.101", "5001"),"archivo1");
	tareaMarcarFinalizada(map1);

	t_tarea *reduce = jobObtenerSiguienteTarea(job);
	tareaMarcarEnEjecucion(reduce,newNodo("nodo1", "192.168.1.101", "5001"),"reduce1");
	tareaMarcarFinalizada(reduce);

	t_tarea *reduceFinal = jobObtenerSiguienteTarea(job);

	CU_ASSERT_TRUE(tareaEsReduce(reduceFinal));

	t_list *archivosRemotos = tareaReduceObtenerArchivosRemotos(reduceFinal);

	t_archivoRemoto *archivo = list_get(archivosRemotos,0);

	CU_ASSERT_EQUAL(list_size(archivosRemotos),1);
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNodo(archivo)->nombre,"nodo1");
	CU_ASSERT_STRING_EQUAL(archivoRemotoObtenerNombreArchivo(archivo),"reduce1");
}


void agregarTestReduceConCombinerJob(CU_pSuite suite)
{
	CU_ADD_TEST(suite, jobConCombiner1Archivo1BloqueYConMapEjecutado_pedirSiguienteTarea_devuelveTareaReduce);
	CU_ADD_TEST(suite, jobConCombiner1Archivo2BloqueYConMapEjecutado_pedirSiguienteTarea_devuelveTareaReduce);
	CU_ADD_TEST(suite, jobConCombiner1Archivo2BloqueYConMapEjecutadoEnDistintosNodos_pedirSiguienteTarea_devuelveTareaReduce);
	CU_ADD_TEST(suite, jobConCombiner1Archivo2BloqueYConPrimerReduceEjecutado_pedirSiguienteTarea_devuelveTareaReduce);
	CU_ADD_TEST(suite, jobConCombiner1Archivo2BloqueYPrimerReduceFalla_pedirSiguienteTarea_devuelveMismoReduce);
	CU_ADD_TEST(suite, jobConCombiner2Archivos2Bloques2NodosYMapsFinalizados_pedimosSiguientesTareas_devuelve2Reduces);
	CU_ADD_TEST(suite, jobConCombiner1Archivo1BloqueYConReduceEjecutado_pedirSiguienteTarea_devuelveReduceFinal);
}
