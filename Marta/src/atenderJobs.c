#include "atenderJobs.h"
#include "planificador.h"
#include "protocoloJob.h"
#include <libsocket.h>
#include <string.h>
#include "logueo.h"

static void atenderNuevoJob(SocketConecta *conexion);
static void atenderConexionCaida(SocketConecta *conexion);
static void atenderConexionConMensaje(SocketConecta *conexionConMensaje);

#define MENSAJE_TAREA_OK 0
#define MENSAJE_TAREA_FALLO 1
#define MENSAJE_NODO_DESCONECTADO 2
#define MENSAJE_SUBIDA_ARCHIVO_OK 1
#define MENSAJE_SUBIDA_ARCHIVO_NO_HAY_ESPACIO 2
#define MENSAJE_SUBIDA_ARCHIVO_SE_DESCONECTO_NODO 0

void atenderJobs(char *puerto)
{
	SocketEscucha *listener = newSocketEscucha("localhost", puerto);
	escucharConexiones(listener);

	Selector *selector = newSelector(listener);

	while (1)
	{
		esperarEventos(selector);

		if(hayNuevaConexion(selector))
		{
			SocketConecta *nuevaConexion = obtenerNuevaConexion(selector);
			atenderNuevoJob(nuevaConexion);
		}

		if (hayMensajesEntrantes(selector))
		{
			t_list *listaConexionesConMensajes = obtenerConexionesConMensajesEntrantes(selector);
			int i; for (i = 0; i < list_size(listaConexionesConMensajes); i++)
			{
				SocketConecta *conexionConMensaje = list_get(listaConexionesConMensajes, i);
				atenderConexionConMensaje(conexionConMensaje);
			}
		}

		if (hayConexionesCaidas(selector))
		{
			t_list *conexionesCaidas = obtenerConexionesCaidas(selector);
			int i; for (i = 0; i < list_size(conexionesCaidas); i++)
			{
				SocketConecta *conexionCaida = list_get(conexionesCaidas, i);
				atenderConexionCaida(conexionCaida);
			}
		}

		while(planificadorObtenerSiguienteTareaYComunicarlaAJob() != NULL)
			;
	}
}

static void atenderNuevoJob(SocketConecta *conexion)
{
	loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Se establecio una nueva conexion con un JOB");
	t_job *job = protocoloJobObtenerDatosJob(conexion);

	if (job != NULL)
		planificadorAgregarJob(job);
	else
	{
		t_job *job = newJob(SIN_COMBINER);
		job->conexion = conexion;

		protocoloJobNotificarFin(job);
	}
}

static void atenderConexionCaida(SocketConecta *conexion)
{
	uint32_t idJob = planificadorDarDeBajaJobPorDesconexion(conexion);
	cerrarSocketConecta(conexion);
	if ((int)idJob != -1)
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "El JOB %d se desconecto. Job dado de baja.", idJob);
}

static void atenderConexionConMensaje(SocketConecta *conexion)
{
	int resultado;
	if (!estaConectado(conexion))
		return;

	recibirMensaje(conexion, &resultado, sizeof(int));

	if (!estaConectado(conexion))
		return;

	uint32_t idTarea;
	recibirMensaje(conexion, &idTarea, sizeof(uint32_t));

	if (!estaConectado(conexion))
		return;

	uint32_t idJob = idTarea >> 16;

	switch(resultado)
	{
	case MENSAJE_TAREA_OK:
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Se recibio resultado de TAREA %d de JOB %d. TAREA finalizada correctamente.", idTarea, idJob);
		planificadorMarcarTareaFinalizada(idTarea);
		break;
	case MENSAJE_TAREA_FALLO:
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Se recibio resultado de TAREA %d de JOB %d. La TAREA fallo.", idTarea, idJob);
		planificadorMarcarTareaFallida(idTarea);
		break;
	case MENSAJE_NODO_DESCONECTADO:
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "Se recibio resultado de TAREA %d de JOB %d. La TAREA fallo por desconexion de nodo.", idTarea, idJob);
		planificadorMarcarTareaFallidaPorBajaDeNodo(idTarea);
		break;
	}
}
