#!/bin/bash
sp="$( cd "$( dirname "$0" )" && pwd)"
if [[ -z "$1" ]]
then
	echo "Usage: prepare.sh <ex_folder>"
	exit 1
fi
ex_folder=$1
mkdir -p "$ex_folder/histos"
mkdir -p "$ex_folder/histos/train"
mkdir -p "$ex_folder/histos/test"
mkdir -p "$ex_folder/logs"
