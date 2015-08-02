#ifndef CONECTARAFILESYSTEM_H_
#define CONECTARAFILESYSTEM_H_

#include "espacioDatos.h"
#include "nuevaConexion.h"
#include "libsocket.h"
#include "configNodo.h"

SocketConecta *conectarAFileSystem(ConfiguracionNodo *nodo,EspacioDatos *espacio);

#endif
