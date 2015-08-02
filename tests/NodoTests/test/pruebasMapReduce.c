#include "espacioDatos.h"
#include "bloqueDatos.h"
#include "ArchivoLocal.h"
#include "mapReduce.h"
#include "atenderConexiones.h"
#include "ArchivoRemoto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ejecucionDeRutina.h"

#define RUTINA_MAP "../../utilidades/rutinasMap/rutinaMapAUsar"
#define RUTINA_REDUCE "../../utilidades/rutinasReduce/rutinaReduceAUsar"

static EspacioDatos *espacioDeDatos;

static void inicializarEspacioDeDatos()
{
	/* Creo un espacio de datos para probar :D */
	printf("Creando espacio de datos...\n");
	system("../../utilidades/creadorEspacioDatos /tmp/espacio.bin 9");
	espacioDeDatos = newEspacioDatos("/tmp/espacio.bin");
}

static void ejecutar3RutinasDeMap()
{
	inicializarEspacioDeDatos();

	printf("Ejecutando map sobre bloque 1\n");
	BloqueDatos *bloque1 = getBloque(espacioDeDatos, 1);
	Archivo *salidaMapBloque1 = (Archivo*) newArchivoLocal("/tmp/salidaMapBloque1", ESCRITURA);
	ejecutarRutinaDeMap(RUTINA_MAP, bloque1, salidaMapBloque1);
	cerrarArchivo(salidaMapBloque1);

	printf("Ejecutando map sobre bloque 4\n");
	BloqueDatos *bloque4 = getBloque(espacioDeDatos, 4);
	Archivo *salidaMapBloque4 = (Archivo*) newArchivoLocal("/tmp/salidaMapBloque4", ESCRITURA);
	ejecutarRutinaDeMap(RUTINA_MAP, bloque4, salidaMapBloque4);
	cerrarArchivo(salidaMapBloque4);

	printf("Ejecutando map sobre bloque 8\n");
	BloqueDatos *bloque8 = getBloque(espacioDeDatos, 8);
	Archivo *salidaMapBloque8 = (Archivo*) newArchivoLocal("/tmp/salidaMapBloque8", ESCRITURA);
	ejecutarRutinaDeMap(RUTINA_MAP, bloque8, salidaMapBloque8);
	cerrarArchivo(salidaMapBloque8);
}

void ejecutar3RutinasDeMapYReducirlas()
{

	ejecutar3RutinasDeMap();


	/*
	 * Ejecutar reduce sobre los resultados de map
	 */
	printf("Ejecutando reduce sobre la salida de los maps anteriores\n");
	Archivo *entrada1Reduce = (Archivo*) newArchivoLocal("/tmp/salidaMapBloque1", LECTURA);
	Archivo *entrada2Reduce = (Archivo*) newArchivoLocal("/tmp/salidaMapBloque4", LECTURA);
	Archivo *entrada3Reduce = (Archivo*) newArchivoLocal("/tmp/salidaMapBloque8", LECTURA);
	Archivo *salidaReduce = (Archivo*) newArchivoLocal("/tmp/salidaReduce", ESCRITURA);
	t_list *listaDeArchivosAReducir = list_create();
	list_add(listaDeArchivosAReducir, entrada1Reduce);
	list_add(listaDeArchivosAReducir, entrada2Reduce);
	list_add(listaDeArchivosAReducir, entrada3Reduce);

	ejecutarRutinaDeReduce(RUTINA_REDUCE, listaDeArchivosAReducir, salidaReduce);

	cerrarArchivo(entrada1Reduce);
	cerrarArchivo(entrada2Reduce);
	cerrarArchivo(entrada3Reduce);
	cerrarArchivo(salidaReduce);
}




#define PATH_TEMPORAL "/home/utnso/Escritorio/"
#define NOMBRE_ARCHIVO "archivoPrueba.txt"
#define PATH_ARCHIVO PATH_TEMPORAL NOMBRE_ARCHIVO
#define PUERTO "12345"

void mapSobreBloqueDeDatosYReduceSobreArchivosLocalesYRemotos(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Warning: tenes que indicar si es cliente o servidor para probar el protocolo\n");
		return;
	}


	if (strcmp(argv[1], "servidor") == 0)
	{
		Archivo *archivoLocal = (Archivo*) newArchivoLocal(PATH_ARCHIVO, ESCRITURA);
		long i = 0;
		for (i = 1; i <= 100000; i ++)
		{
			char registro[100];
			sprintf(registro, "1\n");
			escribirRegistro(archivoLocal, registro);
		}
		cerrarArchivo(archivoLocal);

		escucharConexionesANodo(PUERTO, NULL);
	}
	else
	{
		char *ipServer;
		if (argc == 3)
			ipServer = strdup(argv[2]);
		else
			ipServer = strdup("127.0.0.1");


		ejecutar3RutinasDeMap();


		/*
		 * Ejecutar reduce sobre los resultados de map
		 */
		printf("Ejecutando reduce sobre la salida de los maps anteriores y 1 de red!\n");
		Archivo *entrada1Reduce = (Archivo*) newArchivoLocal("/tmp/salidaMapBloque1", LECTURA);
		Archivo *entrada2Reduce = (Archivo*) newArchivoLocal("/tmp/salidaMapBloque4", LECTURA);
		Archivo *entrada3Reduce = (Archivo*) newArchivoLocal("/tmp/salidaMapBloque8", LECTURA);

		SocketConecta *conexion = newSocketConecta(ipServer, PUERTO);
		conectarAServidor(conexion);
		Archivo *archivoRemoto = (Archivo*) newArchivoRemoto(NOMBRE_ARCHIVO, conexion);

		t_list *listaDeArchivosAReducir = list_create();
		list_add(listaDeArchivosAReducir, entrada1Reduce);
		list_add(listaDeArchivosAReducir, entrada2Reduce);
		list_add(listaDeArchivosAReducir, entrada3Reduce);
		list_add(listaDeArchivosAReducir, archivoRemoto);

		Archivo *salidaReduce = (Archivo*) newArchivoLocal("/tmp/salidaReduce", ESCRITURA);

		int resultadoRutina = ejecutarRutinaDeReduce(RUTINA_REDUCE, listaDeArchivosAReducir, salidaReduce);
		if (resultadoRutina == RUTINA_OK)
			printf("La rutina de reduce termino OK\n");
		else if (resultadoRutina == RUTINA_ERROR_EJECUCION)
			printf("La rutina de reduce termino con errores: Error de ejecucion\n");
		else if(resultadoRutina == RUTINA_ERROR_LECTURA_DATOS)
			printf("La rutina de reduce termino con errores: Error de lectura de datos\n");

		cerrarArchivo(entrada1Reduce);
		cerrarArchivo(entrada2Reduce);
		cerrarArchivo(entrada3Reduce);
		cerrarArchivo(archivoRemoto);
		cerrarArchivo(salidaReduce);
	}
}


#undef PATH_TEMPORAL
#undef NOMBRE_ARCHIVO
#undef PATH_ARCHIVO
#undef PUERTO
