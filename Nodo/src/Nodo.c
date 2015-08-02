#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "espacioDatos.h"
#include "bloqueDatos.h"
#include "configNodo.h"
#include "registroDatos.h"
#include "atenderConexiones.h"
#include "nuevaConexion.h"
#include "consultarNodo.h"
#include "libsocket.h"
#include "atenderAFileSystem.h"
#include "conectarAFileSystem.h"
#include "logueo.h"

#define SI_TENGO_ARCHIVO 'S'
#define NO_TENGO_ARCHIVO 'N'
#define SI_TERMINO 'S'

#define ERROR_REGISTRO_REMOTO	 "C"

int main(int argc, char *argv[]) {
	if (argc != 2)
	{
		fprintf(stderr, "Error: El nodo debe recibir como parametro el path del archivo de configuracion\n");
		fprintf(stderr, "Uso: %s pathArhivoConfig\n", argv[0]);
		exit(1);
	}


	/**********************************************************
	 * INICIALIZACION DE NODO
	 **********************************************************/
	ConfiguracionNodo *configuracionNodo = levantarArchivoConfiguracion(argv[1]);
	EspacioDatos *espacioDeDatos = newEspacioDatos(configuracionNodo->pathEspacioDatos);
	loguearYMostrarEnPantalla(LOG_LEVEL_INFO, "NODO %s inicializado", configuracionNodo->nombre);

	/**********************************************************
	 * HILO DE ATENCION A CONEXIONES ENTRANTES
	 **********************************************************/
	PuertoYEspacio puertoYEspacio = {
			.puerto = configuracionNodo->puerto,
			.espacioDeDatos = espacioDeDatos
	};
	pthread_t hiloAtencionConexionesEntrantes;
	pthread_create(&hiloAtencionConexionesEntrantes, NULL, atenderConexionesEntrantes, &puertoYEspacio);


	/**********************************************************
	 * HILO DE CONEXION AL FILESYSTEM
	 **********************************************************/
	SocketConecta *conexionFS = conectarAFileSystem(configuracionNodo,espacioDeDatos);
	EspacioYConexion espacioYConexion = {
				.conexion = conexionFS,
				.espacioDeDatos = espacioDeDatos
		};
	//crearia thread para atender conexion a FS
	pthread_t hiloConexionConFS;
	pthread_create(&hiloConexionConFS, NULL, atenderAFileSystem, &espacioYConexion);

	/**********************************************************
	 * ESPERA DE FINALIZACION DE HILOS
	 **********************************************************/
	pthread_join(hiloAtencionConexionesEntrantes, NULL); //este no seria al pedo? porque es al pedo esperar este el q importa es el de abajo
	pthread_join(hiloConexionConFS, NULL);


	/**********************************************************
	 * FINALIZACION
	 **********************************************************/
	return (EXIT_SUCCESS);
}
