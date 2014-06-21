#!/bin/bash
sp="$( cd "$( dirname "$0" )" && pwd )"
if [[ -z $2 ]]
then
	echo "Usage: exec.sh <method> <ex_folder>"
	exit 1
fi
#method=$1
temp=`tty`
let t=${temp:11}+2
next_tty="/dev/ttys00$t"
echo "Next tty = $next_tty"
ex_folder=$2
histo_folder="$ex_folder/histos"

if [[ ! -f "$ex_folder/descriptors$1.xml" ]]
then
	echo "Extracting $1 features"
	$sp/extractor $1 "$ex_folder" `$sp/findpictures.sh $ex_folder/learn`
else
	echo "[SKIP]: Skipping $1 descriptor extraction"
fi

if [ $1 = "SIFT" ]
then
	method=1
else 
	method=2
fi

if [[ ! -f "$ex_folder/clusterCenters$1-1000.xml" ]]
then
	echo "Clustering $1 features"
	$sp/clusterize "$method" "$ex_folder" "$ex_folder"
else
	echo "[SKIP]: Skipping $1 descriptor clusterisation"
fi

if [[ ! -f "$histo_folder/test/hystogram$1-1000.xml" ]]
then
	echo "Computing $1 histograms for test examples"
	$sp/classify "$method" "$ex_folder" "$histo_folder/test" `$sp/findpictures.sh $ex_folder/test` >$next_tty &
else
	echo "[SKIP]: Skipping $1 histo computation for test examples"
fi

if [[ ! -f "$histo_folder/learn/hystogram$1-1000.xml" ]]
then
	echo "Computing $1 histograms for learning examples"
	$sp/classify "$method" "$ex_folder" "$histo_folder/learn" `$sp/findpictures.sh $ex_folder/learn`
else
	echo "[SKIP]: Skipping $1 histo computation for learning examples"
fi

if [[ -f "$histo_folder/test/hystogram$1-1000.xml" -a -f "$histo_folder/learn/hystogram$1-1000.xml" ]]
then
	echo "SVM need some time to calc"
	$sp/svmify "$method" "$histo_folder/learn" "$histo_folder/test" "$ex_folder/results$1.txt" 
else
	echo "[WAIT]: Histogram computation is not done yet"
fi