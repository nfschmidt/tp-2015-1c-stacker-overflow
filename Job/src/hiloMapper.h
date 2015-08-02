#ifndef HILOMAPPER_H_
#define HILOMAPPER_H_

typedef struct
{
	char *ipNodo;
	char *puertoNodo;
	char *mapper;
	SocketConecta *conectarAMarta;
	int bloque;
	char *resulMap;//Donde se guarda el resultado de map
	int idMap;
}t_hiloMapper;

void *enviarMapperYObtenerResultado(void *arghiloM);

#endif /* HILOMAPPER_H_ */
