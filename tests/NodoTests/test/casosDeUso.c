#include <string.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <libsocket.h>
#include "ArchivoLocal.h"
#include "atenderConexiones.h"
#include "espacioDatos.h"
#include "bloqueDatos.h"
#include "mapReduce.h"
#include "ArchivoRemoto.h"
#include "configNodo.h"

#define NOMBRE_ARCHIVO_SERVIDOR "archivoServidor.txt"
#define PATH_TEMPORAL_SERVIDOR "/home/utnso/Escritorio/"
#define ARCHIVO_SERVIDOR 	PATH_TEMPORAL_SERVIDOR NOMBRE_ARCHIVO_SERVIDOR
#define PUERTO_SERVIDOR "12345"
#define RUTINA_MAP "../../utilidades/rutinasMap/rutinaMapAUsar"
#define RUTINA_REDUCE "../../utilidades/rutinasReduce/rutinaReduceAUsar"
#define ARCHIVO_SALIDA_MAP "/tmp/salidaMap.txt"
#define ARCHIVO_SALIDA_REDUCE "/tmp/salidaReduce.txt"

static void mostrarMensaje(char *m);

static int ejecutarServidor();
static int testEjecutarRutinaMap();
static int testEjecutarRutinaMapInexistente();
static int testEjecutarRutinaReduce();
static int ejecutarRutinaReduceInexistente();
static int ejecutarRutinaReduceRemoto();
static int ejecutarRutinaReduceRemotoArchivoInexistente();
static int ejecutarServidorReal(char *pathConfig);
static int ejecutarArchivoLocal(char *path);

void ejecutarCasoDeUso(char *argv[])
{
	int resultado;

	if (strcmp(argv[1], "servidor") == 0)
		resultado = ejecutarServidor();

	if (strcmp(argv[1], "map") == 0)
		resultado = testEjecutarRutinaMap();

	if (strcmp(argv[1], "mapError") == 0)
		resultado = testEjecutarRutinaMapInexistente();

	if (strcmp(argv[1], "reduce") == 0)
		resultado = testEjecutarRutinaReduce();

	if (strcmp(argv[1], "reduceError") == 0)
		resultado = ejecutarRutinaReduceInexistente();

	if (strcmp(argv[1], "reduceRemoto") == 0)
		resultado = ejecutarRutinaReduceRemoto();

	if (strcmp(argv[1], "reduceRemotoArchivoInexistente") == 0)
		resultado = ejecutarRutinaReduceRemotoArchivoInexistente();

	if (strcmp(argv[1], "servidorReal") == 0)
		resultado = ejecutarServidorReal(argv[2]);

	if (strcmp(argv[1], "archivoLocal") == 0)
		resultado = ejecutarArchivoLocal(argv[2]);

	printf("Codigo de salida: %d\n", resultado); fflush(stdout);
	exit(resultado);
}


static int ejecutarServidor()
{
	mostrarMensaje("Creando espacio de datos de 10 bloques");
	system("../../utilidades/creadorEspacioDatos /tmp/espacio.bin 10");
	EspacioDatos *espacioDeDatos = newEspacioDatos("/tmp/espacio.bin");


	mostrarMensaje("Ejecutando servidor");
	mostrarMensaje("Creando archivo " ARCHIVO_SERVIDOR);
	Archivo *archivoLocal = (Archivo*) newArchivoLocal(ARCHIVO_SERVIDOR, ESCRITURA);
	int i = 0;
	for (i = 0; i <= 100000; i ++)
	{
		char registro[100];
		sprintf(registro, "REGISTRO %d\n", i);
		escribirRegistro(archivoLocal, registro);
	}
	cerrarArchivo(archivoLocal);

	mostrarMensaje("Escuchando conexiones");
	escucharConexionesANodo(PUERTO_SERVIDOR, espacioDeDatos);

	return 0;
}

static int testEjecutarRutinaMap()
{
	mostrarMensaje("Creando espacio de datos de 4 bloques");
	system("../../utilidades/creadorEspacioDatos /tmp/espacio.bin 4");
	EspacioDatos *espacioDeDatos = newEspacioDatos("/tmp/espacio.bin");

	BloqueDatos *bloque2 = getBloque(espacioDeDatos, 2);
	Archivo *salidaMapBloque2 = (Archivo*) newArchivoLocal(ARCHIVO_SALIDA_MAP, ESCRITURA);

	mostrarMensaje("Ejecutando rutina de map sobre bloque 2");
	int resultado = ejecutarRutinaDeMap(RUTINA_MAP, bloque2, salidaMapBloque2);
	cerrarArchivo(salidaMapBloque2);

	return resultado;
}

static int testEjecutarRutinaMapInexistente()
{
	mostrarMensaje("Creando espacio de datos de 4 bloques");
	system("../../utilidades/creadorEspacioDatos /tmp/espacio.bin 4");
	EspacioDatos *espacioDeDatos = newEspacioDatos("/tmp/espacio.bin");

	mostrarMensaje("Ejecutando rutina de map inexistente sobre bloque 2");
	BloqueDatos *bloque2 = getBloque(espacioDeDatos, 2);
	Archivo *salidaMapBloque2 = (Archivo*) newArchivoLocal(ARCHIVO_SALIDA_MAP, ESCRITURA);
	int resultado = ejecutarRutinaDeMap(RUTINA_MAP "_no_existe", bloque2, salidaMapBloque2);
	cerrarArchivo(salidaMapBloque2);

	return resultado;
}

static int testEjecutarRutinaReduce()
{
	testEjecutarRutinaMap();

	t_list *listaDeArchivosAReducir = list_create();
	Archivo *salidaMap = (Archivo*) newArchivoLocal(ARCHIVO_SALIDA_MAP, LECTURA);
	list_add(listaDeArchivosAReducir, salidaMap);
	Archivo *salidaReduce = (Archivo*) newArchivoLocal(ARCHIVO_SALIDA_REDUCE, ESCRITURA);

	mostrarMensaje("Ejecutando rutina de reduce sobre salida de map");
	int resultado = ejecutarRutinaDeReduce(RUTINA_REDUCE, listaDeArchivosAReducir, salidaReduce);

	return resultado;
}

static int ejecutarRutinaReduceInexistente()
{
	testEjecutarRutinaMap();

	t_list *listaDeArchivosAReducir = list_create();
	Archivo *salidaMap = (Archivo*) newArchivoLocal(ARCHIVO_SALIDA_MAP, LECTURA);
	list_add(listaDeArchivosAReducir, salidaMap);
	Archivo *salidaReduce = (Archivo*) newArchivoLocal(ARCHIVO_SALIDA_REDUCE, ESCRITURA);

	mostrarMensaje("Ejecutando rutina de reduce inexistente sobre salida de map");
	int resultado = ejecutarRutinaDeReduce(RUTINA_REDUCE "_no_existe",
			listaDeArchivosAReducir, salidaReduce);

	return resultado;
}

static int ejecutarRutinaReduceRemoto()
{
	testEjecutarRutinaMap();

	t_list *listaDeArchivosAReducir = list_create();
	list_add(listaDeArchivosAReducir, newArchivoLocal(ARCHIVO_SALIDA_MAP, LECTURA));

	SocketConecta *conexion = newSocketConecta("127.0.0.1", PUERTO_SERVIDOR);
	conectarAServidor(conexion);
	list_add(listaDeArchivosAReducir, newArchivoRemoto(NOMBRE_ARCHIVO_SERVIDOR, conexion));

	Archivo *salidaReduce = (Archivo*) newArchivoLocal(ARCHIVO_SALIDA_REDUCE, ESCRITURA);

	mostrarMensaje("Ejecutando rutina de reduce sobre salida de map y archivo remoto");
	int resultado = ejecutarRutinaDeReduce(RUTINA_REDUCE, listaDeArchivosAReducir, salidaReduce);

	return resultado;
}

static int ejecutarRutinaReduceRemotoArchivoInexistente()
{
	testEjecutarRutinaMap();

	t_list *listaDeArchivosAReducir = list_create();
	list_add(listaDeArchivosAReducir, newArchivoLocal(ARCHIVO_SALIDA_MAP, LECTURA));

	SocketConecta *conexion = newSocketConecta("127.0.0.1", PUERTO_SERVIDOR);
	conectarAServidor(conexion);
	list_add(listaDeArchivosAReducir, newArchivoRemoto(NOMBRE_ARCHIVO_SERVIDOR "no_existe", conexion));

	Archivo *salidaReduce = (Archivo*) newArchivoLocal(ARCHIVO_SALIDA_REDUCE, ESCRITURA);

	mostrarMensaje("Ejecutando rutina de reduce sobre salida de map y archivo remoto inexistente");
	int resultado = ejecutarRutinaDeReduce(RUTINA_REDUCE, listaDeArchivosAReducir, salidaReduce);

	return resultado;
}

static void mostrarMensaje(char *m)
{
	printf("%s\n", m);
	fflush(stdout);
}

static int ejecutarServidorReal(char *pathConfig)
{
	ConfiguracionNodo *config = levantarArchivoConfiguracion(pathConfig);

	mostrarMensaje("Cargando espacio de datos");
	EspacioDatos *espacioDeDatos = newEspacioDatos(config->pathEspacioDatos);

	mostrarMensaje("Escuchando conexiones");

	escucharConexionesANodo(config->puerto, espacioDeDatos);

	return 0;
}

static int ejecutarArchivoLocal(char *path)
{
	ArchivoLocal *archivo = newArchivoLocal(path, LECTURA);

	long i = 0;
	while (obtenerRegistro((Archivo *)archivo))
		i++;

	printf("Registros leidos: %lu\n", i);
	cerrarArchivo((Archivo*) archivo);

	archivo = newArchivoLocal(path, LECTURA);

	i = 0;
	while (obtenerTamanioSiguienteRegistro(archivo))
	{
		obtenerRegistro((Archivo *)archivo);
		i++;
	}

	printf("Registros leidos: %lu\n", i);
	cerrarArchivo((Archivo*) archivo);

	return 0;
}
