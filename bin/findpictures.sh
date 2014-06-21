#!/bin/bash
if [[ -z $1 ]]
then
	echo "Usage: findpictures.sh <folder>"
	exit 1
fi
folder=$1
find $folder -type f -regex "$folder/..*\..*" | xargs