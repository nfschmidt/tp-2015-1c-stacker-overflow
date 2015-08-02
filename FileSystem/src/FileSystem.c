#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/config.h>
#include "archivosConfiguracion.h"
#include "configFileSystem.h"
#include "estadoFileSystem.h"
#include <string.h>
#include "test.h"
#include "estructuraFS.h"
#include <pthread.h>
#include "atenderConexiones.h"
#include "inicializarFS.h"
#include "ConsolaFS.h"
#include "conexionMongo.h"
#include "nodo.h"
#include <mongoc.h>


 void eliminarLogsMongo(mongoc_log_level_t  log_level, const char *log_domain,const char *message, void *user_data)
 {

 }


int main(int argc,char *argv[])
{

	//Inicializa el FileStystem levantando el Archivo de configuracion, creando las listas y directorios
	iniciarFS(argv[1]);

	mongoc_log_set_handler(eliminarLogsMongo,NULL);
	//HILO ATENDER CONEXIONES
	pthread_t hiloAtenderConexiones;
	pthread_create(&hiloAtenderConexiones,NULL,escucharConexionesFileSystem,fileSystem->puerto);


	//HILO CONSOLA
	pthread_t hiloAtenderConsola;
	pthread_create(&hiloAtenderConsola,NULL,iniciarConsolaFS,fileSystem->puerto);


	//correrTest();


	pthread_join(hiloAtenderConsola,NULL);
	pthread_join(hiloAtenderConexiones,NULL);



	return 0;

}




