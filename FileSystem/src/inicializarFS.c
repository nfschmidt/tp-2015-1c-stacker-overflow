#include "configFileSystem.h"
#include "estructuraFS.h"
#include "estadoFileSystem.h"

t_fileSystem *fileSystem;


void iniciarFS(char *path)
{

	//INICIALIZAR FILESYSTEM

	fileSystem = levantarArchivoConfiguracion(path);
	inicializarFileSystem(fileSystem);
	inicializarDirectoriosFS();

}

char *puertoFS(t_fileSystem *fileSystem)
{
	return fileSystem->puerto;
}

