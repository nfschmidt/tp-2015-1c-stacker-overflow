 #ifndef NODO_H_
#define NODO_H_
#include "libsocket.h"

typedef struct {

  char *ip;
  int  estado;
  char *puerto;
  char *nombre;
  int bloquesDisponibles;
  int bloquesUsados;
  int *bloques;
  SocketConecta *conexionNodoServidor;
  pthread_mutex_t mutex_conexionNodo;

}t_nodo;

 t_nodo *newNodo(char *ip, char *puerto, char *nombre, int bloquesTotales, SocketConecta *conexion);
 t_nodo *newNodoPersistido(char *nombreNodo,int bloquesTotales,int bloquesUsados);
void habilitarNodo(t_nodo *nodo);
int tieneNombre(t_nodo *nodo,char *ip);
void liberarNodo(t_nodo *nodo);
void deshabilitarNodo(t_nodo *nodo);
int estaHabilitado(t_nodo *nodo);
void incrementarBloquesUsados(t_nodo *nodo);
int *inicilizarArrayBloques(int bloquesTotales);
int estaHabilitadoNodo(char *nombreNodo, t_list *listaNodosConectados);
void deshabilitarNodoPorDesconexion(t_nodo *nodo);
t_nodo *buscarNodoHabilitado(char *nombreNodo);
void ponerArrayBloquesEn0(int *array,int bloquesTotales);
void reservarConexionNodo(t_nodo *nodo);
void devolverConexionNodo(t_nodo *nodo);
#endif /* NODO_H_ */
