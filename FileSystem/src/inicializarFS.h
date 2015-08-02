
#ifndef INICIALIZARFS_H_
#define INICIALIZARFS_H_
#include "configFileSystem.h"
#include "estructuraFS.h"

t_fileSystem *fileSystem;



void iniciarFS(char *path);
char *puertoFS(t_fileSystem *fileSystem);


#endif /* INICIALIZARFS_H_ */
