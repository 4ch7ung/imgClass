#!/bin/bash
if [[ -z $1 ]]
then
	folder="/tmp"
else
	folder=$1
fi
if [[ -d $folder ]]
then echo $folder
else exit 1
fi
for i in {1..5}
do
	path="$folder/experiment$i"
	mkdir "$path"
	echo "$path"
	mkdir "$path/Good"
	mkdir "$path/Bad"
	mkdir "$path/Good/learn"
	mkdir "$path/Good/test"
	mkdir "$path/Bad/learn"
	mkdir "$path/Bad/test"
	cp ~/Pictures/*.jpg "$path/Good/learn"
	cd "$path/Good/learn"
	mv ./tits* "$path/Bad/learn"
	for j in {1..10}
	do 
		x=$((RANDOM % (50-$j) + 1))
		a=$(ls | xargs | awk "{ print \$$x; }")
		mv $a ../test;
	done
	cd "$path/Bad/learn"
	for j in {1..10}
	do 
		x=$((RANDOM % (50-$j) + 1))
		a=$(ls | xargs | awk "{ print \$$x; }")
		mv $a ../test; 
	done
	mkdir "$path/learn"
	echo "$path/learn"
	mkdir "$path/test"
	echo "$path/test"
	mv $path/Good/learn/* $path/learn
	mv $path/Bad/learn/* $path/learn
	mv $path/Good/test/* $path/test
	mv $path/Bad/test/* $path/test
	mkdir "$path/histos"
	echo "$path/histos"
	mkdir "$path/histos/learn"
	echo "$path/histos/learn"
	mkdir "$path/histos/test"
	echo "$path/histos/test"
	rm -rf $path/Good
	rm -rf $path/Bad
done