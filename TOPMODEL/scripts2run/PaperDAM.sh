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

function pause(){
	read -p "$*"
}

for p in 0 100
do
	echo PROBABILITY $p
	directory="../results/ModelSize${3}devices1_${p}${1}"
	#echo $directory
	if [ -d "$directory" ]; then
		rm -r ../results/ModelSize${3}devices1_${p}${1}
	fi
	mkdir ../results/ModelSize${3}devices1_${p}${1}
	mkdir ../results/ModelSize${3}devices1_${p}${1}/INPUTS
	mkdir ../results/ModelSize${3}devices1_${p}${1}/OUTPUTS
	i=1
	#pause 'Press enter'
	while (($i < 2))
	do
		echo SIMULATION $i
		cd ../../scripts
		./generateDevicesBroken.out ${3} ../NEPData/XMLs/ 1 ${p} ${1}
		cd ../TOPMODEL/inputs/DevicesInput
		mv ModelSize${3}devices1_${p}${1}.txt ../Scenario/Devices.txt
		cd ../../executables
		#Runtop
		echo start running
		./${2}
		if [ -f "SimulationFinished.txt" ]; then
			echo SIMULATION FISHED PROPERLY
			#pause 'Press enter'
			rm SimulationFinished.txt
			#SimplifyLog
			cat TOP_OUTPUT.txt | grep "task" > TOP_Simplified1.txt
			cat TOP_Simplified1.txt | grep "{[A-Za-z0123456789 ]" > TOP_Simplified2.txt
			cat TOP_Simplified2.txt | grep "taskFinisedAcknow: {[A-Za-z0123456789 ]" > TOP_Simplified3.txt
			cat TOP_Simplified3.txt | grep -o "taskFinisedAcknow: {[A-Za-z0123456789_ ]*[-]*[A-Za-z0123456789_ ]*}" > TOP_Simplified4.txt
			cat TOP_Simplified4.txt | grep -o "{[A-Za-z0123456789_ ]*[-]*[A-Za-z0123456789_ ]*" > TOP_Simplified5.txt
			cat TOP_Simplified5.txt | grep -o "[A-Za-z0123456789_ ]*[-]*[A-Za-z0123456789_ ]*" > TOPOUT${i}.txt
			rm TOP_OUTPUT.txt
			rm TOP_Simplified1.txt
			rm TOP_Simplified2.txt
			rm TOP_Simplified3.txt
			rm TOP_Simplified4.txt
			rm TOP_Simplified5.txt
			#pause 'Press enter'
			#echo $i
			#ls
			#Move the files to save them
			#echo TOPOUT${i}.txt
			#echo ../results/ModelSize${3}devices1_${p}${1}/OUTPUTS/TOPOUT${i}.txt
			mv TOPOUT${i}.txt ../results/ModelSize${3}devices1_${p}${1}/OUTPUTS/TOPOUT${i}.txt
			#ls
			cd ../inputs/Scenario/
			mv  Devices.txt ../../results/ModelSize${3}devices1_${p}${1}/INPUTS/Devices${i}.txt
			cd ../../scripts2run
			i=$((i+1))
			#pause 'press enter'
			#echo $i
			#ls
		else
			rm TOP_OUTPUT.txt
			cd ../inputs/Scenario/
			cd ../../scripts2run
		fi
	done
done


for p in 10 20 30
do
	echo PROBABILITY $p
	directory="../results/ModelSize${3}devices1_${p}${1}"
	#echo $directory
	if [ -d "$directory" ]; then
		rm -r ../results/ModelSize${3}devices1_${p}${1}
	fi
	mkdir ../results/ModelSize${3}devices1_${p}${1}
	mkdir ../results/ModelSize${3}devices1_${p}${1}/INPUTS
	mkdir ../results/ModelSize${3}devices1_${p}${1}/OUTPUTS
	i=1
	#pause 'Press enter'
	while (($i < 100))
	do
		echo SIMULATION $i
		cd ../../scripts
		./generateDevicesBroken.out ${3} ../NEPData/XMLs/ 1 ${p} ${1}
		cd ../TOPMODEL/inputs/DevicesInput
		mv ModelSize${3}devices1_${p}${1}.txt ../Scenario/Devices.txt
		cd ../../executables
		#Runtop
		echo start running
		./${2}
		if [ -f "SimulationFinished.txt" ]; then
			echo SIMULATION FISHED PROPERLY
			#pause 'Press enter'
			rm SimulationFinished.txt
			#SimplifyLog
			cat TOP_OUTPUT.txt | grep "task" > TOP_Simplified1.txt
			cat TOP_Simplified1.txt | grep "{[A-Za-z0123456789 ]" > TOP_Simplified2.txt
			cat TOP_Simplified2.txt | grep "taskFinisedAcknow: {[A-Za-z0123456789 ]" > TOP_Simplified3.txt
			cat TOP_Simplified3.txt | grep -o "taskFinisedAcknow: {[A-Za-z0123456789_ ]*[-]*[A-Za-z0123456789_ ]*}" > TOP_Simplified4.txt
			cat TOP_Simplified4.txt | grep -o "{[A-Za-z0123456789_ ]*[-]*[A-Za-z0123456789_ ]*" > TOP_Simplified5.txt
			cat TOP_Simplified5.txt | grep -o "[A-Za-z0123456789_ ]*[-]*[A-Za-z0123456789_ ]*" > TOPOUT${i}.txt
			rm TOP_OUTPUT.txt
			rm TOP_Simplified1.txt
			rm TOP_Simplified2.txt
			rm TOP_Simplified3.txt
			rm TOP_Simplified4.txt
			rm TOP_Simplified5.txt
			#pause 'Press enter'
			#echo $i
			#ls
			#Move the files to save them
			#echo TOPOUT${i}.txt
			#echo ../results/ModelSize${3}devices1_${p}${1}/OUTPUTS/TOPOUT${i}.txt
			mv TOPOUT${i}.txt ../results/ModelSize${3}devices1_${p}${1}/OUTPUTS/TOPOUT${i}.txt
			#ls
			cd ../inputs/Scenario/
			mv  Devices.txt ../../results/ModelSize${3}devices1_${p}${1}/INPUTS/Devices${i}.txt
			cd ../../scripts2run
			i=$((i+1))
			#pause 'press enter'
			#echo $i
			#ls
		else
			rm TOP_OUTPUT.txt
			cd ../inputs/Scenario/
			cd ../../scripts2run
		fi
	done
done

for p in 40 50 60 70 80 90
do
	echo PROBABILITY $p
	directory="../results/ModelSize${3}devices1_${p}${1}"
	#echo $directory
	if [ -d "$directory" ]; then
		rm -r ../results/ModelSize${3}devices1_${p}${1}
	fi
	mkdir ../results/ModelSize${3}devices1_${p}${1}
	mkdir ../results/ModelSize${3}devices1_${p}${1}/INPUTS
	mkdir ../results/ModelSize${3}devices1_${p}${1}/OUTPUTS
	i=1
	#pause 'Press enter'
	while (($i < 40))
	do
		echo SIMULATION $i
		cd ../../scripts
		./generateDevicesBroken.out ${3} ../NEPData/XMLs/ 1 ${p} ${1}
		cd ../TOPMODEL/inputs/DevicesInput
		mv ModelSize${3}devices1_${p}${1}.txt ../Scenario/Devices.txt
		cd ../../executables
		#Runtop
		echo start running
		./${2}
		if [ -f "SimulationFinished.txt" ]; then
			echo SIMULATION FISHED PROPERLY
			#pause 'Press enter'
			rm SimulationFinished.txt
			#SimplifyLog
			cat TOP_OUTPUT.txt | grep "task" > TOP_Simplified1.txt
			cat TOP_Simplified1.txt | grep "{[A-Za-z0123456789 ]" > TOP_Simplified2.txt
			cat TOP_Simplified2.txt | grep "taskFinisedAcknow: {[A-Za-z0123456789 ]" > TOP_Simplified3.txt
			cat TOP_Simplified3.txt | grep -o "taskFinisedAcknow: {[A-Za-z0123456789_ ]*[-]*[A-Za-z0123456789_ ]*}" > TOP_Simplified4.txt
			cat TOP_Simplified4.txt | grep -o "{[A-Za-z0123456789_ ]*[-]*[A-Za-z0123456789_ ]*" > TOP_Simplified5.txt
			cat TOP_Simplified5.txt | grep -o "[A-Za-z0123456789_ ]*[-]*[A-Za-z0123456789_ ]*" > TOPOUT${i}.txt
			rm TOP_OUTPUT.txt
			rm TOP_Simplified1.txt
			rm TOP_Simplified2.txt
			rm TOP_Simplified3.txt
			rm TOP_Simplified4.txt
			rm TOP_Simplified5.txt
			#pause 'Press enter'
			#echo $i
			#ls
			#Move the files to save them
			#echo TOPOUT${i}.txt
			#echo ../results/ModelSize${3}devices1_${p}${1}/OUTPUTS/TOPOUT${i}.txt
			mv TOPOUT${i}.txt ../results/ModelSize${3}devices1_${p}${1}/OUTPUTS/TOPOUT${i}.txt
			#ls
			cd ../inputs/Scenario/
			mv  Devices.txt ../../results/ModelSize${3}devices1_${p}${1}/INPUTS/Devices${i}.txt
			cd ../../scripts2run
			i=$((i+1))
			#pause 'press enter'
			#echo $i
			#ls
		else
			rm TOP_OUTPUT.txt
			cd ../inputs/Scenario/
			cd ../../scripts2run
		fi
	done
done



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
