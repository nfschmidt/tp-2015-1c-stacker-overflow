#include "procesos.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


#define ERROR_EXEC -1

static void ejecutarPrograma(const char *pathDePrograma);
static void manejarErrorExec(const char *pathDePrograma, int codigoDeError);


pid_t crearProcesoHijoNoBloqueante(const char *path, Archivo *nuevoStdin, Archivo *nuevoStdout)
{
	pid_t pid = fork();
	if (pid == 0)
	{
		establecerComoStdin(nuevoStdin);
		establecerComoStdout(nuevoStdout);

		ejecutarPrograma(path);
	}

	return pid;
}


int crearProcesoHijoBloqueante(const char *path, Archivo *nuevoStdin, Archivo *nuevoStdout)
{
	pid_t pid = crearProcesoHijoNoBloqueante(path, nuevoStdin, nuevoStdout);
	return esperarProcesoHijo(pid);
}


static void ejecutarPrograma(const char *pathDePrograma)
{
	int resultadoExec = execlp(pathDePrograma, pathDePrograma, NULL);

	if (resultadoExec == ERROR_EXEC)
		manejarErrorExec(pathDePrograma, errno);
}


static void manejarErrorExec(const char *pathDePrograma, int codigoDeError)
{
	fprintf(stderr, "Error al ejecutar %s: %s\n", pathDePrograma, strerror(codigoDeError));
	exit(1);
}


int esperarProcesoHijo(pid_t pid)
{
	int resultado;
	waitpid(pid, &resultado, 0);

	return resultado;
}
