#include <libsocket.h>
#include <stdio.h>
#include <stdlib.h>

#include "configMarta.h"
#include "logueo.h"
#include "cargaDeRutinas.h"
#include "planificador.h"
#include "protocoloFileSystem.h"
#include "protocoloJob.h"
#include "atenderJobs.h"

int main(int argc, char * argv[]){

	if(argc != 2)
	{
		fprintf(stderr,"ERROR: Marta debe recibir por parametro el path del archivo de configuracion \n");
		exit(1);
	}

	t_configuracionMarta *configuracionMarta = levantarArchivoConfiguracion(argv[1]);

	setPathArchivoDeLog(configuracionMarta->pathArchivoLog);

	setCargaDeMap(configuracionMarta->pesoMap);
	setCargaDeReduce(configuracionMarta->pesoReduce);
	setCargaMaxima(configuracionMarta->cargaMaxima);

	establecerConexionConFileSystem(configuracionMarta->ipFileSystem, configuracionMarta->puertoFileSystem);

	planificadorInicializar();


	notificarTareaAJob = protocoloJobNotificarTarea;
	notificarFinalizacionJob = protocoloJobNotificarFin;
	recibirResultadoDeSubidaDeArchivoAMDFS = protocoloJobObtenerResultadoDeSubidaDeArchivo;
	atenderJobs(configuracionMarta->puertoMarta);

	return 0;
}
