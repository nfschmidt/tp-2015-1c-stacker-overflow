#ifndef HILOREDUCER_H_
#define HILOREDUCER_H_

typedef struct
{
	char *ipNodo;
	char *puertoNodo;
	char *reducer;
	SocketConecta *conectarAMarta;
	char *archivosYNodos;
	int idReduce;
	char *resultado;
	char *resulReduce;
	int soyUltimo;
}t_hiloReducer;

void *enviarReducerYObtenerResultado(void *arghiloR);

#endif /* HILOREDUCER_H_ */
