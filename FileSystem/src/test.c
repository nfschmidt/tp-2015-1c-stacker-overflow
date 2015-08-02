#include <stdio.h>
#include "test.h"
#include "estadoFileSystem.h"
#include <stdlib.h>
#include "estructuraFS.h"
#include "atenderANodo.h"
#include "asignarBloque.h"
#include "responderANodo.h"

SocketConecta *conexion;


void testSiSeConectanLosNodosNoCambiaElEstado()
{

	t_fileSystem *fileSystem = malloc(sizeof(t_fileSystem));
	fileSystem->cantidadNodosConfiguracion = 2;
	inicializarFileSystem(fileSystem);
	agregarNodoConectado("127.0.0.1","3000","Nodo1",10,conexion);
	agregarNodoConectado("127.0.0.2","3000","Nodo2",11,conexion);
	agregarNodoConectado("127.0.0.3","3000","Nodo3",12,conexion);

	if (!esEstadoOperativo())
		printf("testSiSeConectanLosNodosNoCambiaElEstado OK\n");
	else
		printf("testSiSeConectanLosNodosNoCambiaElEstado FAIL\n");
}

void testHabilitacionDeNodosRequeridosYEstadoCambiaAOperativo()
{
	t_fileSystem *fileSystem = malloc(sizeof(t_fileSystem));
		fileSystem->cantidadNodosConfiguracion = 2;
		inicializarFileSystem(fileSystem);
		agregarNodoConectado("127.0.0.1","3000","Nodo1",10,conexion);
		agregarNodoConectado("127.0.0.2","3000","Nodo2",11,conexion);
		habilitarNodoConectado("Nodo1");
		habilitarNodoConectado("Nodo2");

		if (esEstadoOperativo())
			printf("testHabilitacionDeNodosRequeridosYEstadoCambiaAOperativo OK\n");
		else
			printf("testHabilitacionDeNodosRequeridosYEstadoCambiaAOperativo FAIL\n");
}

void testDeshabilitarUnNodoYCambiarElEstado()
{
	t_fileSystem *fileSystem = malloc(sizeof(t_fileSystem));
	fileSystem->cantidadNodosConfiguracion = 2;
	inicializarFileSystem(fileSystem);
	agregarNodoConectado("127.0.0.1","3000","Nodo1",10,conexion);
	agregarNodoConectado("127.0.0.2","3000","Nodo2",11,conexion);
	habilitarNodoConectado("Nodo1");
	habilitarNodoConectado("Nodo2");
	desconectarNodo("Nodo1");

	if(!esEstadoOperativo())
		printf("testDeshabilitarUnNodoYCambiarElEstado OK\n");
	else
		printf("testDeshabilitarUnNodoYCambiarElEstado FAIL\n");
}


void testTeniendoTresNodosYNecesitandoSoloDosQueSigaOperativoSiSeDesconectaUno()
{
	t_fileSystem *fileSystem = malloc(sizeof(t_fileSystem));
	fileSystem->cantidadNodosConfiguracion = 2;
	inicializarFileSystem(fileSystem);
	agregarNodoConectado("127.0.0.1","3000","Nodo1",10,conexion);
	agregarNodoConectado("127.0.0.2","3000","Nodo2",11,conexion);
	agregarNodoConectado("127.0.0.3","3000","Nodo3",12,conexion);
	habilitarNodoConectado("Nodo1");
	habilitarNodoConectado("Nodo2");
	habilitarNodoConectado("Nodo3");
	desconectarNodo("Nodo1");

	if (esEstadoOperativo())
		printf("testTeniendoTresNodosYNecesitandoSoloDosQueSigaOperativoSiSeDesconectaUno OK\n");
	else
		printf("testTeniendoTresNodosYNecesitandoSoloDosQueSigaOperativoSiSeDesconectaUno FAIL\n");

}


void testTeniendoDosNodosHabilitadosDeshabilitarUnoYNoEstaMasOperativo()
{
		t_fileSystem *fileSystem = malloc(sizeof(t_fileSystem));
		fileSystem->cantidadNodosConfiguracion = 2;
		inicializarFileSystem(fileSystem);
		agregarNodoConectado("127.0.0.1","3000","Nodo1",10,conexion);
		agregarNodoConectado("127.0.0.2","3000","Nodo2",11,conexion);
		habilitarNodoConectado("Nodo1");
		habilitarNodoConectado("Nodo2");
		deshabilitarNodoConectado("Nodo1");

		if (!esEstadoOperativo())
			printf("testTeniendoDosNodosHabilitadosDeshabilitarUnoYNoEstaMasOperativo OK\n");
		else
			printf("testTeniendoDosNodosHabilitadosDeshabilitarUnoYNoEstaMasOperativo FAIL\n");

}


void testInicializarFileSystemQuedandoFSConRootCreado()
{
	inicializarDirectoriosFS();

}


void pruebainicializarDirectoriosFS()
{
	t_elementoFS *raiz;
	root = newElementoFS("/",DIRECTORIO);
	raiz = newElementoFS("raiz",DIRECTORIO);
	agregarElementoASubDirectorio(root,raiz);
	agregarElementoASubDirectorio(raiz,newElementoFS("user",DIRECTORIO));
	crearDirectorio("//raiz","diego",root);
	crearDirectorio("//raiz/diego","facultad",root);
	crearDirectorio("//raiz/","jose",root);
	crearArchivo("//raiz/diego/facultad","tpOperativos.txt",root);
	renombrarDirectorio("//raiz/diego/facultad","FACULTAD",root);
	renombrarArchivo("//raiz/diego/FACULTAD/tpOperativos.txt","tpOp.txt",root);
	crearArchivo("//raiz/diego/FACULTAD","tpDiseño.txt",root);
	crearArchivo("//raiz/diego/FACULTAD","tpLegislacion.txt",root);
	imprimirArbol(root,0);
	eliminarDirectorio("/",root);
	imprimirArbol(root,0);

}


void testTeniendo3NodosConectadosSeFijaSiPuedeAsiganarleBloques()
{
	t_fileSystem *fileSystem = malloc(sizeof(t_fileSystem));
		fileSystem->cantidadNodosConfiguracion = 3;
		inicializarFileSystem(fileSystem);
		agregarNodoConectado("127.0.0.1","3000","Nodo1",10,conexion);
		agregarNodoConectado("127.0.0.2","3000","Nodo2",11,conexion);
		agregarNodoConectado("127.0.0.3","3000","Nodo3",12,conexion);
		habilitarNodoConectado("Nodo1");
		habilitarNodoConectado("Nodo2");
		habilitarNodoConectado("Nodo3");

		generarArchivoPesado("/home/utnso/Escritorio/prueba.txt");
	//	armarBloques("/home/utnso/Escritorio/prueba.txt",listaNodosConectados);

}



void correrTest()
{	testSiSeConectanLosNodosNoCambiaElEstado();
	testHabilitacionDeNodosRequeridosYEstadoCambiaAOperativo();
	testDeshabilitarUnNodoYCambiarElEstado();
	testTeniendoTresNodosYNecesitandoSoloDosQueSigaOperativoSiSeDesconectaUno();
	testTeniendoDosNodosHabilitadosDeshabilitarUnoYNoEstaMasOperativo();
	pruebainicializarDirectoriosFS();
	testTeniendo3NodosConectadosSeFijaSiPuedeAsiganarleBloques();

	//seleccionarNodosParaTodosLosBloques(listaNodosConectados,"/home/utnso/Escritorio/diego.txt");


	//testInicializarFileSystemQuedandoFSConRootCreado();
}

