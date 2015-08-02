#ifndef ARCHIVO_H_
#define ARCHIVO_H_

#define COPIA_OK 0
#define ERROR_COPIA 1

typedef struct Archivo
{
	char *(*obtenerRegistro)(struct Archivo *);
	void (*escribirRegistro)(struct Archivo *, const char *);
	void (*cerrarArchivo)(struct Archivo *);
	void (*establecerComoStdin)(struct Archivo *);
	void (*establecerComoStdout)(struct Archivo *);
} Archivo;


void escribirRegistro(Archivo *archivo, const char *registro);

char *obtenerRegistro(Archivo *archivo);

int copiarContenidoDeArchivo(Archivo *origen, Archivo *destino);

void establecerComoStdin(Archivo *archivo);

void establecerComoStdout(Archivo *archivo);

void cerrarArchivo(Archivo *archivo);

int huboError(char *registro);

#endif
