#!/bin/bash

DIRECTORIO_INSTALACION="/home/utnso/ReduceMapFast"


DIRECTORIOS_SCRIPTS_JOB[1]="/home/utnso/weather-mr"
DIRECTORIOS_SCRIPTS_JOB[2]="/home/utnso/mr-py-WordCount"
DIRECTORIOS_SCRIPTS_JOB[3]="/home/utnso/mr-py-WordCount"
DIRECTORIOS_SCRIPTS_JOB[4]="/home/utnso/mr-c-letterCount"
DIRECTORIOS_SCRIPTS_JOB[5]="/home/utnso/mr-py-twitterSentiment"

SIZE_ESPACIO_DATOS[1]="2048M"
SIZE_ESPACIO_DATOS[2]="2048M"
SIZE_ESPACIO_DATOS[3]="2048M"
SIZE_ESPACIO_DATOS[4]="2048M"


function mostrarUso {
	echo
	echo "$0 {[-r directorio_instalacion] | [-n num_nodo:size_espacio] | [-j num_job:directorio_scripts]} -f ip_FS -m ip_MaRTA {IPs}"
	echo
}

##########################################
#	Parseo de opciones
##########################################
while getopts ":r:j:n:f:m:h" OPT
do
	case "$OPT" in
		r)
			DIRECTORIO_INSTALACION="$OPTARG"
			;;
		j)
			JOB=$(echo "$OPTARG" | awk -F':' '{print $1}')
			DIRECTORIO=$(echo "$OPTARG" | awk -F':' '{print $2}')
			DIRECTORIOS_SCRIPTS_JOB[$JOB]="$DIRECTORIO"
			;;
		n)
			NODO=$(echo "$OPTARG" | awk -F':' '{print $1}')
			SIZE=$(echo "$OPTARG" | awk -F':' '{print $2}')
			SIZE_ESPACIO_DATOS[$NODO]="$SIZE"
			;;
		f)
			IP_FS="$OPTARG"
			;;
		m)
			IP_MARTA="$OPTARG"
			;;
		h)
			mostrarUso
			exit 0
			;;
		:)
			echo "Error: la opcion -$OPTARG necesita un argumento" >&2
			mostrarUso
			exit 1
			;;
	esac
done

[[ "a$IP_FS" = "a" ]] && echo "Error: la opcion -f es obligatoria" >&2 && mostrarUso && exit 1
[[ "a$IP_MARTA" = "a" ]] && echo "Error: la opcion -m es obligatoria" >&2 && mostrarUso && exit 1

shift "$((OPTIND - 1))"

DIRECTORIO_CONFIGURACION="${DIRECTORIO_INSTALACION}/config"
DIRECTORIO_ESPACIOS_DATOS="${DIRECTORIO_INSTALACION}/espacioDatos"



##########################################
#	Funciones
##########################################

function ejecutarComandoRemoto {
	ssh -o "StrictHostKeyChecking no" -i $HOME/.ssh/id_rsa_RMF_stacker_overflow "$1" "$2" 
	
	if [[ $? -ne 0 ]]
	then
		echo "ERROR: no se pudo ejecutar el comando remoto '$2' en $1"
		return 1
	fi
}


##########################################
#	MAIN
##########################################

for IP in $@
do
	echo "Configurando $IP..."

	COMANDO_CONFIG_IP_FS="sed -i 's/IP_FS=.*/IP_FS=$IP_FS/' ${DIRECTORIO_CONFIGURACION}/nodo*.conf /home/utnso/ReduceMapFast/config/marta.conf"
	COMANDO_CONFIG_IP_MARTA="sed -i 's/IP_MARTA=.*/IP_MARTA=$IP_MARTA/' ${DIRECTORIO_CONFIGURACION}/job*.conf"

	COMANDO_CONFIG_JOBS=""
	for JOB in {1..5}
	do
		COMANDO_CONFIG_JOBS="${COMANDO_CONFIG_JOBS} sed -i 's@\\(MAPPER\\|REDUCE\\)=.*/\\(.*\\)\$@\\1=${DIRECTORIOS_SCRIPTS_JOB[$JOB]}/\\2@' ${DIRECTORIO_CONFIGURACION}/job${JOB}.conf;"
	done
	
	COMANDO_SIZE_ESPACIO_DATOS=""
	for NODO in {1..4}
	do
		COMANDO_SIZE_ESPACIO_DATOS="${COMANDO_SIZE_ESPACIO_DATOS} truncate -s ${SIZE_ESPACIO_DATOS[$NODO]} ${DIRECTORIO_ESPACIOS_DATOS}/espacioDatosNodo${NODO}.bin;"
	done


	COMANDO="${COMANDO_CONFIG_IP_FS}; ${COMANDO_CONFIG_IP_MARTA}; ${COMANDO_CONFIG_JOBS} ${COMANDO_SIZE_ESPACIO_DATOS}"
	ejecutarComandoRemoto $IP "$COMANDO"

	echo "Configuracion finalizada en $IP"
done
