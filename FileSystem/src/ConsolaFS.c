#include "ConsolaFS.h"
#include "consola.h"
#include "estructuraFS.h"
#include <stdio.h>
#include <pthread.h>
#include "nodo.h"
#include "estadoFileSystem.h"
#include "asignarBloque.h"
#include <stdlib.h>
#include "copiarArchivoAMDFS.h"
#include "conexionMongo.h"
#include "logueo.h"
#include <commons/string.h>


static void consolaImprimirMenu(char **argumentos);
static void consolaFormatearMDFS(char **argumentos);
static void consolaCrearDirectorio(char **argumentos);
static void consolaImprimirArbol(char **argumentos);
static void consolaEliminarArchivo(char **argumentos);
static void consolaRenombrarArchivo(char **argumentos);
static void consolaMoverArchivo(char **argumentos);
static void consolaEliminarDirectorio(char **argumentos);
static void consolaRenombrarDirectorio(char **argumentos);
static void consolaMoverDirectorio(char **argumentos);
static void consolaHabilitarNodo(char **argumentos);
static void consolaDeshabilitarNodoConectado(char **argumentos);
static void consolaCopiarUnArchivoLocalAlMDFS(char **argumentos);
static void consolaBorrarBloqueDeUnArchivo(char **argumentos);
static void consolaVerBloquesDeUnArchivo(char **argumentos);
static void consolaCopiarBloqueDeUnArchivo(char **argumentos);
static void consolaCopiarArchivoDeMDFSALinux(char **argumentos);
static void consolaSolicitarMD5Archivo(char **argumentos);
static void consolaConsultarEstadoFileSystem(char **argumentos);
static void consolaImprimirEstadoNodo(char **argumentos);
static void consolaImprimirEstadoArchivo(char **argumentos);
static int valor;

void *iniciarConsolaFS(void *args)
{
	Consola *consola = newConsola("MDFS> ");


	agregarComando(consola,"comandos",consolaImprimirMenu,0);
	agregarComando(consola,"formatear",consolaFormatearMDFS,0);
	agregarComando(consola,"subirArchivo",consolaCopiarUnArchivoLocalAlMDFS,3);
	agregarComando(consola,"eliminarArchivo",consolaEliminarArchivo,1);
	agregarComando(consola,"renombrarArchivo",consolaRenombrarArchivo,2);
	agregarComando(consola,"moverArchivo",consolaMoverArchivo,2);
	agregarComando(consola,"crearDirectorio",consolaCrearDirectorio,2);
	agregarComando(consola,"eliminarDirectorio",consolaEliminarDirectorio,1);
	agregarComando(consola,"renombrarDirectorio",consolaRenombrarDirectorio,2);
	agregarComando(consola,"moverDirectorio",consolaMoverDirectorio,2);
	agregarComando(consola,"imprimirArbol",consolaImprimirArbol,0);
	agregarComando(consola,"habilitarNodo",consolaHabilitarNodo,1);
	agregarComando(consola,"deshabilitarNodo",consolaDeshabilitarNodoConectado,1);
	agregarComando(consola,"borrarBloque",consolaBorrarBloqueDeUnArchivo,3);
	agregarComando(consola,"verBloque",consolaVerBloquesDeUnArchivo,2);
	agregarComando(consola,"copiarBloque",consolaCopiarBloqueDeUnArchivo,3);
	agregarComando(consola,"copiarArchivoALinux",consolaCopiarArchivoDeMDFSALinux,2);
	agregarComando(consola,"md5",consolaSolicitarMD5Archivo,1);
	agregarComando(consola,"estadoFS",consolaConsultarEstadoFileSystem,0);
	agregarComando(consola,"estadoNodo",consolaImprimirEstadoNodo,1);
	agregarComando(consola,"estadoArchivo",consolaImprimirEstadoArchivo,1);

	printf("\n");
	printf("INICIANDO ReduceMapFast\n");
	printf("\n");
	printf("BIENVENIDOS A LA CONSOLA DEL MDFS\n");
	printf("\n");
	printf("INGRESE LA PALABRA comandos PARA IMPRIMIR EL MENU\n");
	ejecutarConsola(consola);

	return 0;
}

static void consolaImprimirMenu(char **argumentos)
{

	printf("> Los comandos se ingresan con su numero seguido de sus argumentos separados por espacios \n"
			"> COMANDOS \n"
			"	OPERACIONES  \n"
				"	  IMPRIMIR MENU:......................................................................................comandos \n"
				"	  FORMATEAR EL MDFS:..................................................................................formatear \n"
				"	  COPIAR UN ARCHIVO LOCAL AL MDFS(rutaLinux rutaMDFS nombreArchivo):..................................subirArchivo \n"
				"	  ELIMINAR ARCHIVO(ruta):.............................................................................eliminarArchivo \n"
				" 	  RENOMBRAR ARCHIVO(ruta nombre):.....................................................................renombrarArchivo \n"
				"	  MOVER ARCHIVO(rutaOrigen rutaDestino):..............................................................moverArchivo \n"
				"	  CREAR DIRECTORIO(ruta nombre):......................................................................crearDirectorio \n"
				"	  ELIMINAR DIRECTORIO(ruta):..........................................................................eliminarDirectorio \n"
				"	  RENOMBRAR DIRECTORIO(ruta nombre):..................................................................renombrarDirectorio \n"
				"	  MOVER DIRECTORIO(rutaOrigen rutaDestino):...........................................................moverDirectorio \n"
				"	  IMPRIMIR ARBOL DE DIECTORIOS:.......................................................................imprimirArbol \n"
				"	  HABILITAR NODO(nombreNodo):.........................................................................habilitarNodo \n"
				"	  DESHABILITAR NODO(nombreNodo):......................................................................deshabilitarNodo \n"
				"     BORRAR BLOQUE DE UN ARCHIVO(ruta nombreNodo Bloque):................................................borrarBloque \n"
			 	"     VER BLOQUE DE UN ARCHIVO(rutaArchivo Bloque):.......................................................verBloque \n"
				"     COPIAR BLOQUE DE UN ARCHIVO(rutaArchivo nombreNodo numeroBloque):...................................copiarBloque \n"
				"     COPIAR ARCHIVO DE MDFS A FILESYSTEM LOCAL(rutaArchivo rutaLinux):...................................copiarArchivoALinux\n"
				"	  SOLICITAR EL MD5 DEL ARCHIVO (rutaArchivo):.........................................................md5\n"
				"	  VER ESTADO DEL FILESYSTEM:..........................................................................estadoFS\n"
				"	  VER ESTADO DEL NODO: (nombreNodo)...................................................................estadoNodo\n"
				"	  VER ESTADO DEL ARCHIVO : (rutaMDFS).................................................................estadoArchivo\n");
	return ;
}


static void consolaFormatearMDFS(char **argumentos)
{
	printf("Formateando el MDFS...\n");
	borrarDatosElementos();
	borrarDatosNodos();

	formatearMDFS(root);

	inicializarMongoParaElementos(root);
	persistirNodos();


	mostrarEstadoDelFSEnBytesSoloLog();
	printf("Formateando con exito!...\n");
}


static void consolaEliminarArchivo(char **argumentos)
{
	printf("Eliminando el Archivo %s ...\n",argumentos[0]);
	valor = eliminarArchivo(argumentos[0],root);
	if(valor == 1)
	{
		borrarDatosElementos();
		inicializarMongoParaElementos(root);
		borrarDatosNodos();
		persistirNodos();
		printf("¡Archivo eliminado con exito!\n");
		mostrarEstadoDelFSEnBytesSoloLog();
	}
	else
	{
		printf("¡No se pudo eliminar el archivo!\n");
	}
}


static void consolaRenombrarArchivo(char **argumentos)
{
	printf("Renombrando el archivo...\n");
	valor = renombrarArchivo(argumentos[0],argumentos[1],root);

	if(valor == 1)
		{
			borrarDatosElementos();
			inicializarMongoParaElementos(root);
			printf("¡Archivo renombrado con exito!\n");
		}else
			{
			printf("¡No se pudo renombrar el archivo!\n");
			}
}

static void consolaMoverArchivo(char **argumentos)
{
	printf("Moviendo el archivo %s...\n",argumentos[0]);
	valor = moverArchivo(argumentos[0],argumentos[1],root);
	if(valor == 1)
		{
			borrarDatosElementos();
			inicializarMongoParaElementos(root);
			printf("¡Archivo movido con exito!\n");
		}else
			{
				printf("¡No se pudo mover el archivo!\n");
			}

}

static void consolaCrearDirectorio(char **argumentos)
{
	printf("Creando el directorio %s ... \n",argumentos[1]);
	valor = crearDirectorio(argumentos[0],argumentos[1],root);
	if(valor == 1)
		{
			borrarDatosElementos();
			inicializarMongoParaElementos(root);
			printf("¡Directorio creado con exito!\n");
		}else
			{
				printf("¡No se pudo crear el directorio!\n");
			}
}

static void consolaEliminarDirectorio(char **argumentos)
{
	printf("Eliminando el directorio %s...\n",argumentos[0]);
	valor = eliminarDirectorio(argumentos[0],root);
	if(valor == 1)
		{
			borrarDatosElementos();
			inicializarMongoParaElementos(root);
			borrarDatosNodos();
			persistirNodos();

			printf("¡Directorio eliminado con exito!\n");
			mostrarEstadoDelFSEnBytesSoloLog();
		}else
			{
				printf("¡No se pudo eliminar el directorio!\n");
			}
}

static void consolaRenombrarDirectorio(char **argumentos)
{
	printf("Renombrando el directorio %s...\n",argumentos[0]);
	valor = renombrarDirectorio(argumentos[0],argumentos[1],root);
	if(valor == 1)
		{
			borrarDatosElementos();
			inicializarMongoParaElementos(root);
			printf("¡Directorio renombrado con exito!\n");
		}else
			{
				printf("¡No se pudo renombrar el directorio!\n");
			}

}

static void consolaMoverDirectorio(char **argumentos)
{
	printf("Moviendo el directorio %s...\n",argumentos[0]);
	valor = moverDirectorio(argumentos[0],argumentos[1],root);
	if(valor == 1)
		{
			borrarDatosElementos();
			inicializarMongoParaElementos(root);
			printf("¡Directorio movido con exito!\n");
		}else
			{
				printf("¡No se pudo mover el directorio!\n");
			}
}

static void consolaImprimirArbol(char **argumentos)
{
	printf("Imprimiendo el Arbol de Directorios...\n");
	imprimirArbol(root,0);
}


static void consolaHabilitarNodo(char **argumentos)
{
	printf("Habilitando el nodo...\n");
	valor = habilitarNodoConectado(argumentos[0]);
	if(valor == 1)
		{
			borrarDatosNodos();
			persistirNodos();
			printf("¡Nodo Habilitado con exito!\n");
		}else
		{
			printf("¡No se pudo habilitar el nodo!\n");
		}

}

static void consolaDeshabilitarNodoConectado(char **argumentos)
{
	printf("Deshabilitando el nodo...\n");
	valor = deshabilitarNodoConectado(argumentos[0]);
	if(valor == 1)
	{
		borrarDatosNodos();
		persistirNodos();
		printf("¡Nodo deshabilitado con exito!\n");
	}else
	{
		printf("¡No se pudo deshabilitar el nodo!\n");
	}
}

static void consolaCopiarUnArchivoLocalAlMDFS(char **argumentos)
{

	if (copiarArchivoAMDFS(argumentos[0],argumentos[1], argumentos[2]))
	{
		borrarDatosElementos();
		inicializarMongoParaElementos(root);
		borrarDatosNodos();
		persistirNodos();
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO,"¡Bloques copiados con exito!");
		mostrarEstadoDelFSEnBytesSoloLog();
	}
	else
	{
		char *rutaAEliminar = string_new();
		string_append(&rutaAEliminar,argumentos[1]);
		string_append(&rutaAEliminar,"/");
		string_append(&rutaAEliminar,argumentos[2]);
		eliminarArchivo(rutaAEliminar,root);
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO,"¡Los bloques no se pudieron copiar!\n");

	}
}

static void consolaBorrarBloqueDeUnArchivo(char **argumentos)
{
	printf("Borrando bloques del archivo...\n");
	valor = borrarBloqueDeUnArchivo(argumentos[0],argumentos[1],atoi(argumentos[2]));
	if(valor == 1)
	{

		borrarDatosElementos();
		inicializarMongoParaElementos(root);
		borrarDatosNodos();
		persistirNodos();
		mostrarEstadoDelFSEnBytesSoloLog();
		printf("¡Bloque borrados con exito!\n");
	}else
	{
		printf("¡No se puedo borrar el bloque!\n");
	}
}


static void consolaCopiarBloqueDeUnArchivo(char **argumentos)
{
	printf("Copiando el Bloque %d del nodo %s...\n",atoi(argumentos[2]),argumentos[1]);
	valor = copiarBloqueDeUnArchivo(argumentos[0],argumentos[1],atoi(argumentos[2]));
	if(valor == 1)
	{

		borrarDatosElementos();
		inicializarMongoParaElementos(root);
		borrarDatosNodos();
		persistirNodos();
		mostrarEstadoDelFSEnBytesSoloLog();
		printf("¡Bloque copiado con exito!\n");
	}else
	{
		printf("¡No se pudo copiar el Bloque\n");
	}

}


static void consolaVerBloquesDeUnArchivo(char **argumentos)
{

	printf("Abriendo el bloque %d del archivo %s ...\n",atoi(argumentos[1]),argumentos[0]);

	valor = verBloqueDelArchivo(argumentos[0],atoi(argumentos[1]));
	if(valor == 1)
	{
		printf("¡El bloque se mostro con exito!\n");fflush(stdout);
	}
	else
	{
		printf("¡El bloque no se pudo mostrar con exito!\n");fflush(stdout);
	}

}

static void consolaCopiarArchivoDeMDFSALinux(char **argumentos)
{
	loguear(LOG_LEVEL_INFO,"Copiando el archivo %s a %s...",argumentos[0],argumentos[1]);
	imprimirEstadoArchivoLog(argumentos[0]);
	valor = copiarArchivoDeMDFSALiniux(argumentos[0],argumentos[1]);
	if(valor == 1)
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO,"¡El archivo se copio con exito!\n");
	}else
	{
		loguearYMostrarEnPantalla(LOG_LEVEL_INFO,"¡No se pudo copiar el archivo!\n");
	}

}



static void consolaSolicitarMD5Archivo(char **argumentos)
{
	printf("Solicitando el MD5 del archivo %s...\n",argumentos[0]);fflush(stdout);
	valor = solicitarMD5Archivo(argumentos[0]);
	if(valor == 1)
	{
		printf("¡Solicitud realizada con exito!\n");fflush(stdout);
	}
	else
	{
		printf("¡No se pudo solicitar el MD5!\n");fflush(stdout);

	}
}

static void consolaConsultarEstadoFileSystem(char **argumentos)
{
	mostrarEstadoDelFSEnBytesLogYPantalla();
}

static void consolaImprimirEstadoNodo(char **argumentos)
{
	imprimirEstadoNodo(argumentos[0]);
}

static void consolaImprimirEstadoArchivo(char **argumentos)
{
	imprimirEstadoArchivo(argumentos[0]);
}
