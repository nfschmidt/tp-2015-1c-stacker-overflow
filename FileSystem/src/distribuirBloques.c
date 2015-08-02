#include  <stdlib.h>
#include  <string.h>
#include  <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include "nodo.h"
#include <semaphore.h>
#include "asignarBloque.h"
#include "responderANodo.h"
#include "libsocket.h"
#include "estructuraFS.h"
#include "distribuirBloques.h"
#include "estadoFileSystem.h"
#include "enviarBloqueANodos.h"
#include "logueo.h"
static int verificarEspacioParaCopiaBloques(t_list *listaAux);

long int posicionALeerReubicarArchivo(off_t ultimaPosBarraN, off_t contadorBloque)
{
	long int diferenciaContadorYBarraN = 0;
	if (ultimaPosBarraN == 0)
		ultimaPosBarraN = contadorBloque;

	if (ultimaPosBarraN != contadorBloque)
		diferenciaContadorYBarraN = 0 - ( contadorBloque - ultimaPosBarraN);

	return diferenciaContadorYBarraN;
}

void copiarMemoriaBloques(off_t ultimaPosBarraN,char **bloque)
{
	*bloque = realloc(*bloque,ultimaPosBarraN + 2);
	(*bloque)[ultimaPosBarraN + 1] = '\0';

}

int verEnviarOIncrementarBloques(char *bloque,t_elementoFS *archivoNuevo, int cantidadBloques,
		int enviar,t_list *listaAux)
{
	if (enviar == 1)
		return enviarBloqueANodos(bloque,archivoNuevo,cantidadBloques);
	else
		return verificarEspacioParaCopiaBloques(listaAux);



	return 1;
}

static int verificarEspacioParaCopiaBloques(t_list *listaAux)
{
	if(list_size(listaAux)>= 3)
	{
		t_nodo *nodo1 = seleccionarNodoMenosCargado(listaAux,1);
		t_nodo *nodo2 = seleccionarNodoMenosCargado(listaAux,2);
		t_nodo *nodo3 = seleccionarNodoMenosCargado(listaAux,3);

		if (nodo1 == NULL)
			return 0;
		incrementarBloquesUsados(nodo1);

		if (nodo2 == NULL)
			return 0;
		incrementarBloquesUsados(nodo2);

		if (nodo3 == NULL)
			return 0;
		incrementarBloquesUsados(nodo3);

		return 1;
	}else
		return 0;
}
