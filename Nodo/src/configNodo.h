/*
 * configNodo.h
 *
 *  Created on: 8/5/2015
 *      Author: utnso
 */

#ifndef CONFIGNODO_H_
#define CONFIGNODO_H_

typedef struct
{
	char *puerto_fs;
	char *ip_fs;
	char *pathEspacioDatos;
	char *dirTemp;
	int  nuevo; //pasar a bool
	char *ip;
	char *puerto;
	char *nombre;
	char *pathArchivoLog;
}ConfiguracionNodo;


ConfiguracionNodo *levantarArchivoConfiguracion(char *path);
char *obtenerPathEspacioTemporal();
ConfiguracionNodo *obtenerConfiguracionNodo();

#endif /* CONFIGNODO_H_ */
