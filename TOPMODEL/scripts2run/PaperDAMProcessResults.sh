#!/bin/bash

if [ "$#" != "3" ]; then
	echo "You are using this program with wrong parameters. Te program should be invoked as follow: "
	echo ${1} 
	echo ${2}
	echo ${3}
	echo " Name of the device that will break"
	echo " Name of the TOPMODEL executable"
	echo " Number people in the scenario"
	exit 1
fi
cd ../resultsProcessed
for p in 0 10 20 30 40 50 60 70 80 90 100
do
	cd ../results/ModelSize${3}devices1_${p}${1}/OUTPUTS
	shopt -s nullglob
	numfiles=(*)
	numfiles=${#numfiles[@]}
	cd ../../../resultsProcessed
	./ProcessResults.out ../results/ModelSize${3}devices1_${p}${1}/OUTPUTS/ ${numfiles} ../resultsProcessed/datafiles/ModelSize${3}devices1_${p}${1}
done
python ./GraficosBoxPlot_IC.py -s ${3} -d ${1}
