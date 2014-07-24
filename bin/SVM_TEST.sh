#!/bin/bash
if [ -z $1 ]
then
	echo "Usage: SVM_TEST.sh <svmify_folder> <exp_folder>"
	exit 1
fi
svmf=$1
expf=$2
for i in {0..4}
do
  histf="$expf/fold_$i/histos"
  "$svmf"/svm_run.sh "$svmf"/svmify $i 1 "$histf/train" "$histf/test" "$expf/fold_$i/results/resSIFT-Precision$i.txt" "$expf/fold_$i/results/resSIFT-Recall$i.txt" "$expf/fold_$i/results/resSIFT-FScore$i.txt" &
  "$svmf"/svm_run.sh "$svmf"/svmify $i 2 "$histf/train" "$histf/test" "$expf/fold_$i/results/resSURF-Precision$i.txt" "$expf/fold_$i/results/resSURF-Recall$i.txt" "$expf/fold_$i/results/resSURF-FScore$i.txt" &
done
