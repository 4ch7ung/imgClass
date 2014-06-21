#!/bin/bash
if [ -z $1 ]
then
	echo "Usage: SVM_TEST.sh <svmify_folder> <exp_folder>"
	exit 1
fi
svmf=$1
expf=$2
for i in {1..10}
do
  histf="$expf/experiment$i/histos"
  "$svmf"/svm_run.sh "$svmf"/svmify $i 1 "$histf/learn" "$histf/test" "$expf/results/resSIFT-Precision$i.txt" "$expf/results/resSIFT-Recall$i.txt" "$expf/results/resSIFT-FScore$i.txt" &
  "$svmf"/svm_run.sh "$svmf"/svmify $i 2 "$histf/learn" "$histf/test" "$expf/results/resSURF-Precision$i.txt" "$expf/results/resSURF-Recall$i.txt" "$expf/results/resSURF-FScore$i.txt" &
done
