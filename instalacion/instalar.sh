#!/bin/bash

if [[ $# -eq 0 ]]
then
	DIRECTORIO_INSTALACION="/home/utnso/ReduceMapFast"
else
	DIRECTORIO_INSTALACION="$1"
fi

DIRECTORIO_ARCHIVOS_CONFIGURACION="${DIRECTORIO_INSTALACION}/config"
DIRECTORIO_LOGS="${DIRECTORIO_INSTALACION}/logs"
DIRECTORIO_ESPACIO_DATOS="${DIRECTORIO_INSTALACION}/espacioDatos"
DIRECTORIO_SCRIPT="$(dirname $0)"
DIRECTORIO_RAIZ="${DIRECTORIO_SCRIPT}/.."


function mostrarMensaje {
	[[ $1 = "INFO" ]] && echo "INFO: $2"
	[[ $1 = "ERROR" ]] && echo "ERROR: $2" >&2
}

function crearDirectorioInstalacion {
	if test -d $DIRECTORIO_INSTALACION
	then
		rm -vfr $DIRECTORIO_INSTALACION
	fi

	mkdir $DIRECTORIO_INSTALACION

	return $?
}

function crearDirectorioArchivosConfiguracion {
	mkdir -v "$DIRECTORIO_ARCHIVOS_CONFIGURACION"
	return $?
}

function crearDirectorioLogs {
	mkdir -v "$DIRECTORIO_LOGS"
	return $?
}

function crearDirectorioEspacioDatos {
	mkdir -v "$DIRECTORIO_ESPACIO_DATOS"
	return $?
}

function crearEspaciosDatos {
	mostrarMensaje INFO "Creando espacio de datos de nodo 1"
	truncate -s 2048M "${DIRECTORIO_ESPACIO_DATOS}/espacioDatosNodo1.bin" || return 1
	mostrarMensaje INFO "Creando espacio de datos de nodo 2"
	truncate -s 2048M "${DIRECTORIO_ESPACIO_DATOS}/espacioDatosNodo2.bin" || return 1
	mostrarMensaje INFO "Creando espacio de datos de nodo 3"
	truncate -s 2048M "${DIRECTORIO_ESPACIO_DATOS}/espacioDatosNodo3.bin" || return 1
	mostrarMensaje INFO "Creando espacio de datos de nodo 4"
	truncate -s 2048M "${DIRECTORIO_ESPACIO_DATOS}/espacioDatosNodo4.bin" || return 1
	return 0
}

function compilar {
	pushd "${DIRECTORIO_RAIZ}/$1/Debug" >/dev/null
	make clean
	make all
	RESULTADO=$?
	popd >/dev/null

	return $RESULTADO
}

function instalar {
	compilar "$1"
	cp -v "${DIRECTORIO_RAIZ}/$1/Debug/$1" ${DIRECTORIO_INSTALACION}
	return $?
}

function compilarArchivoConfig {
	compilar "ArchivoConfig"
	return $?
}

function compilarLibsocket {
	compilar "libsocket"
	return $?
}

function instalarMongo {
	dpkg -l | grep mongo >/dev/null
	if [[ $? -ne 0 ]]
	then
		sudo apt-get -y install mongodb
	fi
}

function instalarDriversMongo {
	RESULTADO=0
	ls /usr/local/lib | grep mongo >/dev/null
	if [[ $? -ne 0 ]]
	then
		cp "${DIRECTORIO_RAIZ}/mongoDBDriver/mongo-c-driver-1.1.9.tar.gz" /tmp
		pushd /tmp
		tar -xvzf mongo-c-driver-1.1.9.tar.gz
		cd mongo-c-driver-1.1.9
		./configure
		make
		sudo make install
		RESULTADO=$?
		popd
	fi
	
	return $RESULTADO
}

function borrarDatosMongo {
	mongo --eval 'db.dropDatabase()' MDFS
	return $?
}
function actualizarLdLibraryPath {
	echo $LD_LIBRARY_PATH | grep /usr/local/lib >/dev/null
	if [[ $? -ne 0 ]]
	then
		echo "" >> ~/.bashrc
		echo "# Necesario para Reduce Map Fast" >> ~/.bashrc
		echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib' >> ~/.bashrc
		RELOGIN_NECESARIO="SI"
		return $?
	fi

	return 0
}

function instalarFilesystem {
	instalar "FileSystem"
	return $?
}

function instalarMarta {
	instalar "Marta"
	return $?
}

function instalarNodo {
	instalar "Nodo"
	return $?
}

function instalarJob {
	instalar "Job"
	return $?
}

function realizarTarea {
	TAREA="$1"

	mostrarMensaje INFO "Ejecutando tarea: $2"

	$TAREA > "/tmp/$TAREA.log" 2>&1

	if [[ $? -ne 0 ]]
	then
		mostrarMensaje ERROR "Ocurrió un error durante la ejecución de la tarea: $2. Se aborta la instalacion"	
		echo "Detalles: "
		cat /tmp/$TAREA.log
		exit 1
	else
		mostrarMensaje INFO "Tarea finalizada: $2"
	fi
}

function instalarArchivosConfiguracion {
	cp -v "${DIRECTORIO_RAIZ}/conf/archivosDeEvaluacion/"* $DIRECTORIO_ARCHIVOS_CONFIGURACION
	return $?
}

function crearEspaciosTemporales {
	mkdir -p /home/utnso/ReduceMapFast/espaciosTemporales/nodo1 || return 1
	mkdir -p /home/utnso/ReduceMapFast/espaciosTemporales/nodo2 || return 1
	mkdir -p /home/utnso/ReduceMapFast/espaciosTemporales/nodo3 || return 1
	mkdir -p /home/utnso/ReduceMapFast/espaciosTemporales/nodo4 || return 1
	return 0
}

function instalarCommons {
	if [[ -d /usr/include/commons ]]
	then
		return 0
	fi

        pushd "${DIRECTORIO_RAIZ}/so-commons-library"
        make clean
        make
	sudo make install
        RESULTADO=$?
        popd

        return $RESULTADO
}

function instalarKeysSSH {
	if [[ ! -d $HOME/.ssh ]]
	then
		mkdir $HOME/.ssh || return 1
	fi

	cp ${DIRECTORIO_SCRIPT}/id_rsa_RMF_stacker_overflow $HOME/.ssh || return 1
	cp ${DIRECTORIO_SCRIPT}/id_rsa_RMF_stacker_overflow.pub $HOME/.ssh || return 1
	cat ${DIRECTORIO_SCRIPT}/id_rsa_RMF_stacker_overflow.pub > $HOME/.ssh/authorized_keys || return 1
	chmod 700 $HOME/.ssh/id_rsa_RMF_stacker_overflow || return 1

	return 0
}


#########################################
#	MAIN
#########################################

realizarTarea instalarKeysSSH "Instalar Keys SSH"

realizarTarea crearDirectorioInstalacion "Crear directorio de instalacion"
realizarTarea crearDirectorioArchivosConfiguracion "Crear directorio de archivos de configuracion"
realizarTarea crearDirectorioLogs "Crear directorio de archivos de log"
realizarTarea instalarArchivosConfiguracion "Instalar archivos de configuracion"

realizarTarea instalarCommons "Instalar Commons Library"

realizarTarea compilarArchivoConfig "Compilar ArchivoConfig"
realizarTarea compilarLibsocket "Compilar libsocket"

realizarTarea instalarMongo "Instalar MongoDB"
realizarTarea instalarDriversMongo "Instalar drivers de C de MongoDB"
realizarTarea borrarDatosMongo "Borrando datos de MongoDB"
realizarTarea actualizarLdLibraryPath "Configurar LD_LIBRARY_PATH"

realizarTarea instalarJob "Instalar Job"
realizarTarea instalarFilesystem "Instalar FileSystem"
realizarTarea instalarMarta "Instalar MaRTA"
realizarTarea instalarNodo "Instalar Nodo"

realizarTarea crearDirectorioEspacioDatos "Crear directorio de espacios de datos"
realizarTarea crearEspaciosDatos "Crear espacios de datos"
realizarTarea crearEspaciosTemporales "Crear espacios temporales"

mostrarMensaje INFO "Instalacion satisfactoria"


if [[ "$RELOGIN_NECESARIO" = "SI" ]]
then
	mostrarMensaje INFO "Es necesario volver a iniciar sesion para finalizar la instalacion"
fi

exit 0
