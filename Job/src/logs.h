#ifndef LOGS_H_
#define LOGS_H_

void iniciarYEstablecerRutaArchivoLog(char *ruta);
void borrarSemaforoLogs();
void logDebug(char *mensaje);
void logConexionMarta(char *mensaje,char *ipMarta,char *puertoMarta);
void logCreacionHiloMapper(char *mensaje,char *ipNodo,char *puertoNodo,char *mapper);
void logCreacionHiloReducer(char *mensaje,char *ipNodo,char *puertoNodo,char *reduce,char *resultado);
void logConMensaje(char *mensaje);
void logCabeceraEnvia(char *cabecera);
void logCabeceraRecibe(char *cabecera);
void logOperacion(char *mensaje,int idOp);
void logFinJob(char *mensaje,char *rutinaMap,char *rutinaReduce,char *resultado);


#endif /* LOGS_H_ */
