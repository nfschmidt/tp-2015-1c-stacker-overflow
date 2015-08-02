#include <CUnit/Basic.h>
#include <commons/collections/list.h>
#include "job.h"
#include "nodo.h"
#include "archivoRemoto.h"
#include "planificador.h"
#include "tarea.h"
#include "cargaDeRutinas.h"

static t_list *_mockObtenerDetalleBloque1Nodo(char *path, int bloque)
{
	t_list *resultado = list_create();
	t_nodo *nodo = newNodo("nodo1", "192.168.1.101", "5001");
	nodo->bloque = 0;

	list_add(resultado, nodo);

	return resultado;
}

static t_list *_mockObtenerDetalleBloque2Nodos(char *path, int bloque)
{
	t_list *resultado = list_create();
	t_nodo *nodo1 = newNodo("nodo1", "192.168.1.101", "5001");
	nodo1->bloque = 0;
	t_nodo *nodo2 = newNodo("nodo2", "192.168.1.102", "5002");
	nodo2->bloque = 1;

	list_add(resultado, nodo1);
	list_add(resultado, nodo2);

	return resultado;
}

static t_list *_mockObtenerDetalleBloque4Nodos(char *path, int bloque)
{
	t_list *resultado = list_create();
	t_nodo *nodo1 = newNodo("nodo1", "192.168.1.101", "5001");
	t_nodo *nodo2 = newNodo("nodo2", "192.168.1.102", "5002");
	t_nodo *nodo3 = newNodo("nodo3", "192.168.1.103", "5002");
	t_nodo *nodo4 = newNodo("nodo4", "192.168.1.104", "5002");

	list_add(resultado, nodo1);
	list_add(resultado, nodo2);
	list_add(resultado, nodo3);
	list_add(resultado, nodo4);

	return resultado;
}

static int notificacionDeJobFinalizadoHecha;
static void _mockNotificarJobFinalizado(t_job *job)
{
	notificacionDeJobFinalizadoHecha = 1;
}

static int jobFueNotificado;
static int _mockNotificarTareaAJobOK(t_job *job, t_tarea *tarea)
{
	jobFueNotificado = 1;
	return 1;
}

static int _mockNotificarTareaAJobFalla(t_job *job, t_tarea *tarea)
{
	jobFueNotificado = 0;
	return 0;
}

static int _mockNotificarTareaAJob_FallaSinCombiner(t_job *job, t_tarea *tarea)
{
	if (job->tipo == SIN_COMBINER)
	{
		jobFueNotificado = 0;
		return 0;
	}

	jobFueNotificado = 1;
	return 1;
}


void _mockRecibirResultadoDeSubidaDeArchivoAMDFS(t_job *job)
{

}


static void inicializarTest()
{
	setCargaDeMap(1);
	setCargaDeReduce(3);
	setCargaMaxima(10);
	notificacionDeJobFinalizadoHecha = 0;
	jobFueNotificado = 0;
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;
	notificarFinalizacionJob = _mockNotificarJobFinalizado;
	notificarTareaAJob = _mockNotificarTareaAJobOK;
	recibirResultadoDeSubidaDeArchivoAMDFS = _mockRecibirResultadoDeSubidaDeArchivoAMDFS;
}


void unNodojobSinCombinerDe1Bloque_pidoTareas_obtengo1TareaMapYUnaDeReduce()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 1);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);

	t_tarea *tareaMap = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(tareaMap->id);

	t_tarea *tareaReduce = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(tareaReduce->id);
	t_tarea *tareaNula = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_TRUE(tareaEsMap(tareaMap));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tareaMap), 0);
	t_nodo *nodoMap = tareaObtenerNodoQueEjecuto(tareaMap);
	CU_ASSERT_STRING_EQUAL(nodoMap->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoMap->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoMap->puerto, "5001");


	CU_ASSERT_TRUE(tareaEsReduce(tareaReduce));
	t_nodo *nodoReduce = tareaObtenerNodoQueEjecuto(tareaReduce);
	CU_ASSERT_STRING_EQUAL(nodoReduce->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoReduce->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoReduce->puerto, "5001");

	t_list *listaArchivosRemotos = tareaReduceObtenerArchivosRemotos(tareaReduce);
	CU_ASSERT_EQUAL(list_size(listaArchivosRemotos), 1);
	t_archivoRemoto *archivoRemoto = list_get(listaArchivosRemotos, 0);

	t_nodo *nodoArchivoRemoto = archivoRemotoObtenerNodo(archivoRemoto);
	CU_ASSERT_STRING_EQUAL(nodoArchivoRemoto->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoArchivoRemoto->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoArchivoRemoto->puerto, "5001");

	CU_ASSERT_PTR_EQUAL(tareaNula, NULL);
}

void unNodoJobSinCombinerDe2Bloques_pidoTareasYLasFinalizo_obtengo2TareasMapYUnaReduce()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);

	t_tarea *tareaMap1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(tareaMap1->id);

	t_tarea *tareaMap2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(tareaMap2->id);

	t_tarea *tareaReduce = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(tareaReduce->id);
	t_tarea *tareaNula = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_TRUE(tareaEsMap(tareaMap1));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tareaMap1), 0);
	t_nodo *nodoMap = tareaObtenerNodoQueEjecuto(tareaMap1);
	CU_ASSERT_STRING_EQUAL(nodoMap->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoMap->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoMap->puerto, "5001");

	CU_ASSERT_TRUE(tareaEsMap(tareaMap2));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tareaMap2), 1);
	t_nodo *nodoMap2 = tareaObtenerNodoQueEjecuto(tareaMap2);
	CU_ASSERT_STRING_EQUAL(nodoMap2->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoMap2->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoMap2->puerto, "5001");

	CU_ASSERT_TRUE(tareaEsReduce(tareaReduce));
	t_nodo *nodoReduce = tareaObtenerNodoQueEjecuto(tareaReduce);
	CU_ASSERT_STRING_EQUAL(nodoReduce->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoReduce->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoReduce->puerto, "5001");

	t_list *listaArchivosRemotos = tareaReduceObtenerArchivosRemotos(tareaReduce);
	CU_ASSERT_EQUAL(list_size(listaArchivosRemotos), 2);
	t_archivoRemoto *archivoRemoto = list_get(listaArchivosRemotos, 0);

	t_nodo *nodoArchivoRemoto = archivoRemotoObtenerNodo(archivoRemoto);
	CU_ASSERT_STRING_EQUAL(nodoArchivoRemoto->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoArchivoRemoto->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoArchivoRemoto->puerto, "5001");

	t_archivoRemoto *archivoRemoto2 = list_get(listaArchivosRemotos, 1);

	t_nodo *nodoArchivoRemoto2 = archivoRemotoObtenerNodo(archivoRemoto2);
	CU_ASSERT_STRING_EQUAL(nodoArchivoRemoto2->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoArchivoRemoto2->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoArchivoRemoto2->puerto, "5001");


	CU_ASSERT_PTR_EQUAL(tareaNula, NULL);
}

void unNodoJobSinCombinerDe2Bloques_pidoTareasYNOLasFinalizo_obtengo2TareasMapYSinTareasReduce()
{
	inicializarTest();

	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);

	t_tarea *tareaMap1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMap2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaNula = planificadorObtenerSiguienteTareaYComunicarlaAJob();


	CU_ASSERT_TRUE(tareaEsMap(tareaMap1));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tareaMap1), 0);
	t_nodo *nodoMap = tareaObtenerNodoQueEjecuto(tareaMap1);
	CU_ASSERT_STRING_EQUAL(nodoMap->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoMap->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoMap->puerto, "5001");

	CU_ASSERT_TRUE(tareaEsMap(tareaMap2));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tareaMap2), 1);
	t_nodo *nodoMap2 = tareaObtenerNodoQueEjecuto(tareaMap2);
	CU_ASSERT_STRING_EQUAL(nodoMap2->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoMap2->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoMap2->puerto, "5001");

	CU_ASSERT_PTR_EQUAL(tareaNula, NULL);
}

void unNodoJobSinCombinerDe2Bloques_pidoTareasYFinalizo1_obtengo2TareasMapYSinTareasReduce()
{
	inicializarTest();

	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);

	t_tarea *tareaMap1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(tareaMap1->id);
	t_tarea *tareaMap2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaNula = planificadorObtenerSiguienteTareaYComunicarlaAJob();


	CU_ASSERT_TRUE(tareaEsMap(tareaMap1));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tareaMap1), 0);
	t_nodo *nodoMap = tareaObtenerNodoQueEjecuto(tareaMap1);
	CU_ASSERT_STRING_EQUAL(nodoMap->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoMap->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoMap->puerto, "5001");

	CU_ASSERT_TRUE(tareaEsMap(tareaMap2));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tareaMap2), 1);
	t_nodo *nodoMap2 = tareaObtenerNodoQueEjecuto(tareaMap2);
	CU_ASSERT_STRING_EQUAL(nodoMap2->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoMap2->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoMap2->puerto, "5001");

	CU_ASSERT_PTR_EQUAL(tareaNula, NULL);
}

void unNodoJobSinCombinerDe2Bloques_pido2TareasYMarco1FAllidaPidoOtraTarea_tareaMapQueFallo()
{
	inicializarTest();

	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);

	t_tarea *tareaMap1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMap2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaNula = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	planificadorMarcarTareaFallida(tareaMap1->id);
	t_tarea *tareaMapReplanificada = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_TRUE(tareaEsMap(tareaMap1));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tareaMap1), 0);
	t_nodo *nodoMap = tareaObtenerNodoQueEjecuto(tareaMap1);
	CU_ASSERT_STRING_EQUAL(nodoMap->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoMap->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoMap->puerto, "5001");

	CU_ASSERT_TRUE(tareaEsMap(tareaMap2));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tareaMap2), 1);
	t_nodo *nodoMap2 = tareaObtenerNodoQueEjecuto(tareaMap2);
	CU_ASSERT_STRING_EQUAL(nodoMap2->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoMap2->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoMap2->puerto, "5001");

	CU_ASSERT_PTR_EQUAL(tareaNula, NULL);

	CU_ASSERT_TRUE(tareaEsMap(tareaMapReplanificada));
	CU_ASSERT_EQUAL(tareaMapObtenerBloque(tareaMapReplanificada), 0);
	t_nodo *nodoMapReplanificado = tareaObtenerNodoQueEjecuto(tareaMapReplanificada);
	CU_ASSERT_STRING_EQUAL(nodoMapReplanificado->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(nodoMapReplanificado->ip, "192.168.3.101");
	CU_ASSERT_STRING_EQUAL(nodoMapReplanificado->puerto, "5001");
}

void unNodoJobSinCombinerDe2Bloques_seFinalizanTodasLasTareas_seNotificaQueFinalizoElJob()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;
	notificarFinalizacionJob = _mockNotificarJobFinalizado;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);

	t_tarea *tareaMap1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(tareaMap1->id);
	t_tarea *tareaMap2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(tareaMap2->id);
	t_tarea *reduceFinal = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(reduceFinal->id);

	CU_ASSERT_TRUE(notificacionDeJobFinalizadoHecha);
}

void unNodoJobSinCombinerDe2Bloques_fallaReduce_seDaDeBajaElJob()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);

	t_tarea *tareaMap1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(tareaMap1->id);
	t_tarea *tareaMap2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(tareaMap2->id);
	t_tarea *reduceFinal = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFallida(reduceFinal->id);

	t_job *jobObtenido = planificadorObtenerJobDeTarea(reduceFinal->id);

	CU_ASSERT_PTR_NULL(jobObtenido);
}

void unNodoJobSinCombinerDe2Bloques_seFinalizanTodasLasTareas_seDaDeBajaElJob()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);

	t_tarea *tareaMap1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(tareaMap1->id);
	t_tarea *tareaMap2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(tareaMap2->id);
	t_tarea *reduceFinal = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFinalizada(reduceFinal->id);

	t_job *jobObtenido = planificadorObtenerJobDeTarea(reduceFinal->id);

	CU_ASSERT_PTR_NULL(jobObtenido);
}

void dosJobsDe2Bloques_sePiden4Tareas_seObtienenTareasDeJobsIntercalados()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	t_job *job2 = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job2, "/holy2.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);
	planificadorAgregarJob(job2);


	t_tarea *tareaMap1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMap2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMap3 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMap4 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	uint32_t idJobMap1 = tareaMap1->id >> 16;
	CU_ASSERT_EQUAL(idJobMap1, 1);
	uint32_t idJobMap2 = tareaMap2->id >> 16;
	CU_ASSERT_EQUAL(idJobMap2, 2);
	uint32_t idJobMap3 = tareaMap3->id >> 16;
	CU_ASSERT_EQUAL(idJobMap3, 1);
	uint32_t idJobMap4 = tareaMap4->id >> 16;
	CU_ASSERT_EQUAL(idJobMap4, 2);
}

void unJobDe2Bloques_sePiden3TareasSinFinalizar_laTerceraTareaEsNula()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);

	t_tarea *tareaMap1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMap2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaNula = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_PTR_NOT_NULL(tareaMap1);
	CU_ASSERT_PTR_NOT_NULL(tareaMap2);
	CU_ASSERT_PTR_NULL(tareaNula);
}

void dosJobsDe2Bloques_sePiden6Tareas_lasUltimas2SonNulas()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	t_job *job2 = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job2, "/holy2.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);
	planificadorAgregarJob(job2);


	t_tarea *tareaMap1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMap2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMap3 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMap4 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaNula1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaNula2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_PTR_NOT_NULL(tareaMap1);
	CU_ASSERT_PTR_NOT_NULL(tareaMap2);
	CU_ASSERT_PTR_NOT_NULL(tareaMap3);
	CU_ASSERT_PTR_NOT_NULL(tareaMap4);
	CU_ASSERT_PTR_NULL(tareaNula1);
	CU_ASSERT_PTR_NULL(tareaNula2);
}

void dosJobsDe2BloquesPrimerJobConMapsFinalizadosSegundoConMapsFallidos_se2Tareas_lasTareasSonDelJob2()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	t_job *job2 = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job2, "/holy2.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);
	planificadorAgregarJob(job2);

	t_tarea *tareaMap1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMap2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMap3 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMap4 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	planificadorMarcarTareaFinalizada(tareaMap1->id);
	planificadorMarcarTareaFinalizada(tareaMap3->id);
	planificadorMarcarTareaFallida(tareaMap2->id);
	planificadorMarcarTareaFallida(tareaMap4->id);

	t_tarea *tareaReduce = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMapReplanificada1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tareaMapReplanificada2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	uint32_t idJobMap2 = tareaMapReplanificada1->id >> 16;
	uint32_t idJobMap4 = tareaMapReplanificada2->id >> 16;
	CU_ASSERT_EQUAL(idJobMap2, 2);
	CU_ASSERT_EQUAL(idJobMap4, 2);
	CU_ASSERT_TRUE(tareaEsReduce(tareaReduce));
}

void jobDeUnBloque_pidoTarea_laTareaSeComunicaAlJob()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);

	planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_TRUE(jobFueNotificado);
}

void jobDeUnBloquePidoTarea_laTareaNoSeComunicaAlJob_seDaDeBajaElJob()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;
	notificarTareaAJob = _mockNotificarTareaAJobFalla;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);

	t_tarea *tareaNula = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_FALSE(jobFueNotificado);
	CU_ASSERT_PTR_NULL(tareaNula);
}

void dosJobsDeUnBloquePidoPrimeraTarea_laTareaNoSeComunicaAlJob_seObtieneTareaDeSegundoJob()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque1Nodo;

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 1);
	t_job *job2 = newJob(CON_COMBINER);
	jobAgregarArchivoMDFS(job2, "/holy2.txt", 1);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);
	planificadorAgregarJob(job2);

	notificarTareaAJob = _mockNotificarTareaAJob_FallaSinCombiner;
	t_tarea *tareaMapJob2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_PTR_NOT_NULL(tareaMapJob2);
	uint32_t idJob = tareaMapJob2->id >> 16;
	CU_ASSERT_EQUAL(idJob, 2);
}

void unNodo_seDaDeBajaElNodo2Veces_noPincha()
{
	inicializarTest();

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);
	t_tarea *tarea1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tarea2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	planificadorMarcarTareaFallidaPorBajaDeNodo(tarea1->id);
	planificadorMarcarTareaFallidaPorBajaDeNodo(tarea2->id);
}

void unNodo_asignan2Tareas_cargaDeNodoEs2()
{
	inicializarTest();

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);
	t_tarea *tarea1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tarea2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_EQUAL(tarea1->nodo->carga, 2);
	CU_ASSERT_EQUAL(tarea2->nodo->carga, 2);
}

void unNodoCon1TareaAsignada_fallaTareaPorNodoCaidoSeAsignaNuevaTareaAlNodo_cargaDeNodoEs1()
{
	inicializarTest();

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarNodo(newNodo("nodo1", "192.168.3.101", "5001"));
	planificadorAgregarJob(job);
	t_tarea *tarea1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorMarcarTareaFallidaPorBajaDeNodo(tarea1->id);
	t_tarea *tareaReplanificada = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_EQUAL(tareaReplanificada->nodo->carga, 1);
}

void dosNodos1Job2Bloques_pido2Tareas_seAsigna1ACadaNodo()
{
	inicializarTest();

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarJob(job);

	obtenerDetalleDeBloques = _mockObtenerDetalleBloque2Nodos;

	t_tarea *tarea1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tarea2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_STRING_EQUAL(tarea1->nodo->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(tarea2->nodo->nombre, "nodo2");
}

void dosNodos2Job2Bloques_pido4Tareas_seAsignan2ACadaNodoIntercaladas()
{
	inicializarTest();

	planificadorInicializar();

	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	t_job *job2 = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job2, "/holy.txt", 2);
	planificadorAgregarJob(job);
	planificadorAgregarJob(job2);

	obtenerDetalleDeBloques = _mockObtenerDetalleBloque2Nodos;

	t_tarea *tarea1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tarea2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tarea3 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *tarea4 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_STRING_EQUAL(tarea1->nodo->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(tarea2->nodo->nombre, "nodo2");
	CU_ASSERT_STRING_EQUAL(tarea3->nodo->nombre, "nodo1");
	CU_ASSERT_STRING_EQUAL(tarea4->nodo->nombre, "nodo2");
}

void noHayJobs_pido1Tarea_noPincha()
{
	inicializarTest();

	planificadorInicializar();

	planificadorObtenerSiguienteTareaYComunicarlaAJob();
}

void noHayJobs_pido2Tareas_noPincha()
{
	inicializarTest();

	planificadorInicializar();

	planificadorObtenerSiguienteTareaYComunicarlaAJob();
	planificadorObtenerSiguienteTareaYComunicarlaAJob();
}

void unJobYSeDaDeBajaJobPorNotificacionFallida_pidoTarea_reciboTareaNulaYNoPincha()
{
	inicializarTest();

	planificadorInicializar();
	t_job *job = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 2);
	planificadorAgregarJob(job);

	notificarTareaAJob = _mockNotificarTareaAJobFalla;
	t_tarea *tarea = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	t_tarea *tarea2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	CU_ASSERT_PTR_NULL(tarea);
	CU_ASSERT_PTR_NULL(tarea2);
}

void jobEnEjecucion_seDaDeBajaUnNodo_seReplanificanLasTareasEnEjecucionYFinalizadasDeEseNodoSinPinchar()
{
	inicializarTest();

	planificadorInicializar();

	t_job *job = newJob(CON_COMBINER);
	jobAgregarArchivoMDFS(job, "/holy.txt", 8);
	planificadorAgregarJob(job);

	obtenerDetalleDeBloques = _mockObtenerDetalleBloque4Nodos;

	t_tarea *map0 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map3 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map4 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map5 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map6 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map7 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	planificadorMarcarTareaFinalizada(map0->id);
	planificadorMarcarTareaFinalizada(map1->id);
	planificadorMarcarTareaFinalizada(map2->id);
	planificadorMarcarTareaFinalizada(map3->id);
	planificadorMarcarTareaFinalizada(map4->id);
	planificadorMarcarTareaFinalizada(map6->id);
	planificadorMarcarTareaFinalizada(map7->id);

	planificadorMarcarTareaFallidaPorBajaDeNodo(map5->id);


	planificadorObtenerSiguienteTareaYComunicarlaAJob();

	planificadorObtenerSiguienteTareaYComunicarlaAJob();
}

void aversipincha()
{
	inicializarTest();
	obtenerDetalleDeBloques = _mockObtenerDetalleBloque4Nodos;

	planificadorInicializar();

	t_job *job1 = newJob(CON_COMBINER);
	t_job *job2 = newJob(CON_COMBINER);
	t_job *job3 = newJob(SIN_COMBINER);
	jobAgregarArchivoMDFS(job1, "/holy1.txt", 8);
	jobAgregarArchivoMDFS(job1, "/holy2.txt", 8);
	jobAgregarArchivoMDFS(job1, "/holy3.txt", 8);
	jobAgregarArchivoMDFS(job1, "/holy4.txt", 8);

	jobAgregarArchivoMDFS(job2, "/linux.txt", 2);
	jobAgregarArchivoMDFS(job2, "/guten.txt", 1);

	jobAgregarArchivoMDFS(job3, "/linux.txt", 2);
	jobAgregarArchivoMDFS(job3, "/guten.txt", 1);


	planificadorAgregarJob(job1);

	t_tarea *map0job1nodo1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map1job1nodo2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map2job1nodo3 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map3job1nodo4 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map4job1nodo1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map5job1nodo2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map6job1nodo3 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map7job1nodo4 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	planificadorMarcarTareaFinalizada(map0job1nodo1->id);
	planificadorMarcarTareaFinalizada(map1job1nodo2->id);
	planificadorMarcarTareaFinalizada(map2job1nodo3->id);
	planificadorMarcarTareaFinalizada(map3job1nodo4->id);

	planificadorAgregarJob(job2);
	planificadorAgregarJob(job3);

	t_tarea *map0job2nodo1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map0job3nodo2 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map8job1nodo3 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map1job2nodo4 = planificadorObtenerSiguienteTareaYComunicarlaAJob();
	t_tarea *map1job3nodo1 = planificadorObtenerSiguienteTareaYComunicarlaAJob();

	planificadorMarcarTareaFallidaPorBajaDeNodo(map1job2nodo4->id);
	planificadorMarcarTareaFallidaPorBajaDeNodo(map7job1nodo4->id);
	planificadorMarcarTareaFallidaPorBajaDeNodo(map4job1nodo1->id);

	planificadorMarcarTareaFallidaPorBajaDeNodo(map5job1nodo2->id);
	planificadorMarcarTareaFallidaPorBajaDeNodo(map6job1nodo3->id);
	planificadorMarcarTareaFallidaPorBajaDeNodo(map0job2nodo1->id);
	planificadorMarcarTareaFallidaPorBajaDeNodo(map0job3nodo2->id);
	planificadorMarcarTareaFallidaPorBajaDeNodo(map8job1nodo3->id);
	planificadorMarcarTareaFallidaPorBajaDeNodo(map1job3nodo1->id);
}

void agregarTestPlanificador(CU_pSuite suite)
{
	CU_ADD_TEST(suite, unNodojobSinCombinerDe1Bloque_pidoTareas_obtengo1TareaMapYUnaDeReduce);
	CU_ADD_TEST(suite, unNodoJobSinCombinerDe2Bloques_pidoTareasYLasFinalizo_obtengo2TareasMapYUnaReduce);
	CU_ADD_TEST(suite, unNodoJobSinCombinerDe2Bloques_pidoTareasYNOLasFinalizo_obtengo2TareasMapYSinTareasReduce);
	CU_ADD_TEST(suite, unNodoJobSinCombinerDe2Bloques_pidoTareasYFinalizo1_obtengo2TareasMapYSinTareasReduce);
	CU_ADD_TEST(suite, unNodoJobSinCombinerDe2Bloques_pido2TareasYMarco1FAllidaPidoOtraTarea_tareaMapQueFallo);
	CU_ADD_TEST(suite, unNodoJobSinCombinerDe2Bloques_seFinalizanTodasLasTareas_seNotificaQueFinalizoElJob);
	CU_ADD_TEST(suite, unNodoJobSinCombinerDe2Bloques_fallaReduce_seDaDeBajaElJob);
	CU_ADD_TEST(suite, unNodoJobSinCombinerDe2Bloques_seFinalizanTodasLasTareas_seDaDeBajaElJob);
	CU_ADD_TEST(suite, dosJobsDe2Bloques_sePiden4Tareas_seObtienenTareasDeJobsIntercalados);
	CU_ADD_TEST(suite, unJobDe2Bloques_sePiden3TareasSinFinalizar_laTerceraTareaEsNula);
	CU_ADD_TEST(suite, dosJobsDe2Bloques_sePiden6Tareas_lasUltimas2SonNulas);
	CU_ADD_TEST(suite, dosJobsDe2BloquesPrimerJobConMapsFinalizadosSegundoConMapsFallidos_se2Tareas_lasTareasSonDelJob2);
	CU_ADD_TEST(suite, jobDeUnBloque_pidoTarea_laTareaSeComunicaAlJob);
	CU_ADD_TEST(suite, jobDeUnBloquePidoTarea_laTareaNoSeComunicaAlJob_seDaDeBajaElJob);
	CU_ADD_TEST(suite, dosJobsDeUnBloquePidoPrimeraTarea_laTareaNoSeComunicaAlJob_seObtieneTareaDeSegundoJob);
	CU_ADD_TEST(suite, unNodo_seDaDeBajaElNodo2Veces_noPincha);
	CU_ADD_TEST(suite, unNodo_asignan2Tareas_cargaDeNodoEs2);
	CU_ADD_TEST(suite, unNodoCon1TareaAsignada_fallaTareaPorNodoCaidoSeAsignaNuevaTareaAlNodo_cargaDeNodoEs1);
	CU_ADD_TEST(suite, dosNodos1Job2Bloques_pido2Tareas_seAsigna1ACadaNodo);
	CU_ADD_TEST(suite, dosNodos2Job2Bloques_pido4Tareas_seAsignan2ACadaNodoIntercaladas);
	CU_ADD_TEST(suite, noHayJobs_pido1Tarea_noPincha);
	CU_ADD_TEST(suite, noHayJobs_pido2Tareas_noPincha);
	CU_ADD_TEST(suite, unJobYSeDaDeBajaJobPorNotificacionFallida_pidoTarea_reciboTareaNulaYNoPincha);
	CU_ADD_TEST(suite, jobEnEjecucion_seDaDeBajaUnNodo_seReplanificanLasTareasEnEjecucionYFinalizadasDeEseNodoSinPinchar);
	CU_ADD_TEST(suite, aversipincha);
}
