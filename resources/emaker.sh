#!/bin/bash

DEFAULT_PROJECT_DIR="./tpe-pi"
PROJECT_DIR=$1
SVN=$(whereis svn | cut -d' ' -f2)
SVN_COMMAND="$SVN checkout"
SVN_USER_ENABLE="--username"
SVN_URL="https://tp-pi-itba.googlecode.com/svn/"

if [ "$1" = "help" ]; then
    echo ""
    echo ""
    echo "============================================================="
    echo ""
    echo ""
    echo "AYUDA:"
    echo "  Para inicializar el entorno:"
    echo "      $PS1 $0 directorio usuario-svn-google"
    echo ""
    echo ""
    echo "PARA ACTUALIZAR:"
    echo "  Vas al directorio correspondiente y ejecutás:"
    echo "      $PS1 svn up"
    echo ""
    echo ""
    echo "PARA AGREGAR COSAS AL REPO (no se guarda hasta que lo subís):"
    echo "  Para agregar un archivo (supongamos que es main.c), hacés"
    echo "  en el directorio donde está:"
    echo "      $PS1 svn add main.c"
    echo ""
    echo ""
    echo "PARA SUBIR LOS DATOS AL REPO:"
    echo "  Vas al directorio raíz del repo (src) y ejecutás:"
    echo "      $PS1 svn ci"
    echo "  Te va a pedir que ingreses un mensaje de commit para saber"
    echo "  de lo que se trata la modificación. Se breve y descriptivo."
    echo ""
    echo ""
    echo "============================================================="
    echo ""
    echo ""
    exit 0
fi


# Buscando svn
if [ -e SVN ]; then
    echo "No se encuentra svn"
    exit 1
fi

if [ -z $2] && [ -z $1 ]; then
    echo "ERROR: Tenés que dar un usuario."
    exit 1
fi

if [ -z "$2" ]; then
    PROJECT_DIR=DEFAULT_PROJECT_DIR
    SVN_USER = "$1"
else
    PROJECT_DIR="$1"
    SVN_USER = "$2"
fi


echo "Directorio del proyecto $PROJECT_DIR"
echo "Creando estructura básica:"
mkdir -p "$DEFAULT_PROJECT_DIR/"
echo "Bajando archivos del repositorio:"
echo "  -trunk:"
$SVN_COMMAND $SVN_URL"/trunk/" "$DEFAULT_PROJECT_DIR/trunk/" $SVN_USER_ENABLE $SVN_USER
echo "  -wiki:"
$SVN_COMMAND $SVN_URL"/wiki/" "$DEFAULT_PROJECT_DIR/wiki/" $SVN_USER_ENABLE $SVN_USER
