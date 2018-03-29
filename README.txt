This project contains an implementation example of the Diffusion Abstract Model (DAM) presented in the paper: "Formal Abstract Modeling of Dynamic Multiplex Networks"

Authors: Cristina Ruiz Martin, Gabriel Wainer, Adolfo López Paredes. 
INSISOC - Universidad de Valladolid
Department of Systems and Computer Engineering - Carleton University.

The model represents the communications established among the first responders in a nuclear emergency plan.

The artifacts attached are data and simulation models. The license and the instructions are provided in the root, in the files BSD-LICENSE.txt and Instructions.txt respectively
The project also contains third party libraries. Each of these libraries has its own license.

This project is available in the github repository of Simulation Everywhere:
https://github.com/SimulationEverywhere/NEP_DAM.git

-------------------------------------------------------------
System requirements:
Ubuntu 16.04 (it may also work in previous versions)
RAM 32GB
** To install Ubuntu on a Windows machine, please follow the instructions in this link:
https://www.tecmint.com/install-ubuntu-16-04-alongside-with-windows-10-or-8-in-dual-boot/
-------------------------------------------------------------
Dependencies:
	- Boost library for c++	(Instructions provided bellow to install Boost)
	- Python and the following python libraries must be installed: (Python is already pre-installed in Ubuntu 16.04. You will need to install the libraries following the instructions provided)
 		- pandas 
 		- matplotlib.pyplot
		- numpy
 		- gflags
 		- sys
 		- scipy
 		- math	
	- CDBoost. External library. It is provided in the folder "cadmium". It is a header only DEVS simulator. Do not need any installation.
	- tinyXML. External library. It is provided in the folder "tinyXML". It is a header only library. Do not need any installation.
	- DESTimes. External library. It is a time class for CDBoost simulator provided in the folder "DESTimes". It is a header only library. Do not need any installation.

** Instructions to Install Dependencies

Install Boost library
(1) Open Ubuntu terminal. To open Ubuntu terminal press: "CONTROL+ALT+t"
(2) Type on the Ubuntu terminal the following command and press ENTER: sudo apt-get install libboost-all-dev
(3) Type the administrative password and press ENTER
(4) When you are asked "Do you want to continue?" type: y and then press ENTER
(5) Wait until the installation is finished

Install Python libraries
(1) Open Ubuntu terminal. To open Ubuntu terminal press: "CONTROL+ALT+t"
(2) Type on the Ubuntu terminal the following command, press ENTER and wait until finished: sudo apt install python-pip
(3) Type the administrative password (if you are asked. The system may remember it)
(4) Type on the Ubuntu terminal the following command and, press ENTER and wait until finished: sudo apt install --upgrade-pip
(5) Type on the Ubuntu terminal the following command and, press ENTER and wait until finished: sudo pip install pandas
(6) Type on the Ubuntu terminal the following command and, press ENTER and wait until finished: sudo pip install matplotlib
(7) Type on the Ubuntu terminal the following command and, press ENTER and wait until finished: sudo pip install numpy
(8) Type on the Ubuntu terminal the following command and, press ENTER and wait until finished: sudo pip install python-gflags
(9) Type on the Ubuntu terminal the following command and, press ENTER and wait until finished: sudo apt-get install python-sys
(10) Type on the Ubuntu terminal the following command and, press ENTER and wait until finished: sudo pip install spicy
(11) Type on the Ubuntu terminal the following command and, press ENTER and wait until finished: sudo apt-get install python-math
(12) Type on the Ubuntu terminal the following command and, press ENTER and wait until finished: sudo apt-get install python-tk

----------------------------------------------------------------
Compiler:
	- The project was compiled and tested using g++7.2. Previous versions of g++ do not work because they cannot compile in c++17.

** Instructions to install gcc 7.2 and g++ 7.2 and make them as default compiler:
	
(1) Open Ubuntu terminal. To open Ubuntu terminal press: "CONTROL+ALT+t"
(2) Type on the Ubuntu terminal the following command and press ENTER: sudo add-apt-repository ppa:jonathonf/gcc-7.1
Type the administrative password (if you are asked. The system may remember it)
(3) When you are asked "Press [ENTER] to continue or ctrl-c to cancel adding it" press ENTER
(4) Type on the Ubuntu terminal the following command and press ENTER: sudo apt-get update
(5) Type the administrative password (if you are asked. The system may remember it)
(6) Type on the Ubuntu terminal the following command and press ENTER: sudo apt-get install gcc-7 g++-7
Type the administrative password (if you are asked. The system may remember it)
(7) When you are asked "Do you want to continue?" type: y and press ENTER
(8) Type on the Ubuntu terminal the following command and press ENTER: sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 60 --slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-7 --slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-7 --slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-7
(9) Type the administrative password (if you are asked. The system may remember it)
(10) Type on the Ubuntu terminal the following command and press ENTER: sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 60 --slave /usr/bin/g++-ar g++-ar /usr/bin/g++-ar-7 --slave /usr/bin/g++-nm g++-nm /usr/bin/g++-nm-7 --slave /usr/bin/g++-ranlib g++-ranlib /usr/bin/g++-ranlib-7
(11) Type the administrative password (if you are asked. The system may remember it)
(12) To check that you have g++-7 type on the terminal the following command and press ENTER: g++ -v

--------------------------------------------------------------
Files organization:
- atomics [this folder contains all the CDBoost atomic models used in this project]
	answerPersonTask.hpp
	answerPhoneTask.hpp
	answerRadioTask.hpp
	callByMultiplePhoneTask.hpp
	callByPhoneTask.hpp
	DevicePhone.hpp
	DevicePhoneMultiple.hpp
	DeviceRadio.hpp
	devicesSwitch.hpp
	DeviceTextClasNewBroken.hpp
	DeviceTextClasNewRead.hpp
	DeviceTextInbox.hpp
	DeviceTextOutbox.hpp
	doActionTask.hpp
	filterCommandGenerator.hpp
	filterDevicesMicroKeyboard.hpp
	filterDevicesNetwork.hpp
	filterDevicesState.hpp
	filterNetworks.hpp
	filterNetworksState.hpp
	filterPeopleInLocation.hpp
	filterPersonDeviceTypeFrom.hpp
	filterPersonDeviceTypeMyId.hpp
	filterPersonDeviceTypeTo.hpp
	iestream.hpp
	NetPhone.hpp
	NetRadio.hpp
	NetText.hpp
	PeopleInLocation.hpp
	PersonBehaviorRules.hpp
	readTextOnDigitalDevice.hpp
	readTextOnPaperDevice.hpp
	sendCommandByRadio.hpp
	sendCommandInPerson.hpp
	sendTextTask.hpp
	sinkDevices.hpp
	sinkReceivingBehavior.hpp
	sinkSendingBehavior.hpp	
	switchFace2Face.hpp
- cadmium [This folder contains CDBoost simulation. It is an external library]
- data_structures [This folder contains the message type and data structures used in the model]
	communication.cpp
	communication.hpp
	decision_maker_behaviour_enum_types.cpp
	decision_maker_behaviour_enum_types.hpp	
	decision_maker_behaviour_struct_types_V2.hpp
	nep_model_enum_types.cpp
	nep_model_enum_types.hpp
	struct_types.cpp
	struct_types.hpp
- DESTimes [This folder contains DESTimes library. It is an external library]
- NEPData [This folder contains data used to build the model]
	- Nodes [This folder contains the nodes of the network model]
		- Subsets [This folder contains subsets of the nodes of the network model to simulate different groups]
			RadiologicalGroup.txt
			NEPHead.txt
		NetworkNodes.txt
	- Relations [This folder contains the relations of the network model]
		NetworkRelations.txt
	- XMLs [This folder contains all the XML files where the behavior of the agents is defined]
- scripts [This folder contains the program to generate the model and the input data]
	- generateTOPinclude [This folder contains include files to generate the TOP model]
		functions2InstantiateAtomics.hpp
		functions2InstantiateCoupleds.hpp
		TOPBlock1
		TOPBlock2
	generateDevicesBroken.cpp
	generateTOPMODEL.cpp
	makefile
- tinyXML [This folder contains tinyXML library. It is an external library]
- TOPMODEL [This folder contains the TOPModel and the results of the model]
	- executables [This folder contains the TOP model once 	compiled] (empty until the model is compiled)
	- inputs [This folder contains the inputs to simulate the model]
		- DevicesInput [This folder is empty until the model devices inputs are generated using the compiled version of "generateDevicesBroken.cpp"]
		- Scenario [This folder contains the inputs used in each simulation] (The files are added using the script for each simulation)
			 Command.txt
	- results [This folder stores the outputs and inputs of each simulation] (It is empty until a simulation is run)
	- resultsProcessed [This folder contains the data processed, the graphical results of the simulations and the scripts to generate the graphical results] (The graphical results and data processed are not available until the script is run)
		-datafiles [This folder is empty until the results of the simulation are processed]
		GraficosBoxPlot_IC.py
		makefile
		ProcessResults.cpp
	- scripts2run [This folder contains the script to run the simulation and generate the results provided in the paper]
		PaperDAM.sh
	makefile
- tests [All the unit test and the inputs for the test are shown is in this folder]
	- decisionMaker [unit tests for the atomic model PersonBehaviorRules]
		- unit_test_decision_maker_v2
			main.cpp
			makefile
			requestIn.txt
			taskDeviceFinished.txt
			XML_Eg1.xlm
		- XML_test_lead_decision_maker_v2
			Equalfiles.sh
			main.cpp
			makefile
			XML_DecisionMakerBehaviour_Eg1_v2.xml
	- tests_atomics [unit test for all the atomic models except PersonBehaviorRules]
		- test_inputs [this folder contains the txt files that are used as inputs for the tests]
		- tests_mains [this folder contains the main file for each unit test]
		- test_results [this folder contains the results of the unit tests] (it is empty until the tests are run)
		makefile
		RunUnitTest.sh
README.txt
BSD-LICENSE.txt
--------------------------------------------------------------------------------------------------------------------------------
STEPS TO REPLICATE THE RESULTS SHOWN IN THE PAPER

(1) Follow the instruction provided in the sections "Dependencies" and "Compiler" to install the compiler and the dependencies
(2) Extract the content of the zip file in the folder documents
(3) Generate the executable files for the scripts
	(3.1) Open a new Ubuntu terminal. To open Ubuntu terminal press: "CONTROL+ALT+t"
	(3.2) Place the prompt in the folder "scripts". To do so, type on the terminal the following command and press ENTER: cd Documents/NEP_DAM/scripts
	(3.3) Type on the terminal the following command and press ENTER: make clean; make all
		The result will be two executable files in the folder "scripts": "generateTOPMODEL.out" & "generateDevicesBroken.out"
	(3.4) Place the prompt in the folder "TOPMODEL/resultsProcessed". To do so, type on the terminal the following command and press ENTER: cd ../TOPMODEL/resultsProcessed
	(3.5) Type on the terminal the following command and press ENTER: make clean; make all
	The result will be an executable file in the folder "TOPMODEL/scripts2run": "ResultsProcessed.out" 
(4) Generate the CPP and executable file for the top model
	(4.1) Place the prompt in the folder "scripts". To do so, type on the terminal the following command and press ENTER: cd ../../scripts
	(4.2) Type on the terminal the following command and press ENTER: ./generateTOPMODEL.out ../NEPData/Nodes/Subsets/RadiologicalGroup.txt
	The results will be a "Mainmodel.cpp" file in the folder "TOPMODEL"
	(4.3) Place the prompt in the folder "TOPMODEL". To do so, type on the terminal the following command and press ENTER: cd ../TOPMODEL
	(4.4) Type on the terminal the following command and press ENTER: make clean; make all
	The result will be a executable "TOPMODEL.out" in the folder "TOPMODEL/executables". 
	**In order to be able to compile the model (i.e. do step 4.4) and therefore replicate the results of the paper, 32GB RAM are needed. It will take around 20min.
	****A smaller subset of nodes (NEPHead.txt) is also provided as second scenario although the results are not included in the paper. To compile this model, a computer with 4GB RAM is enough. You just need to change in step 4.2 PadiologicalGroup.txt by NEPHead.txt
(5) Generate the simulation results and their graphs
	We provide two options that will generate the same results. In option 1, the simulations and results are generated in a single script. In option 2 we use two scripts, one for generating the simulation results and other for processing the results and generating the graphs. OPTION 2 is recommended
	OPTION 1
	(5.1) Open a new Ubuntu terminal. To open Ubuntu terminal press: "CONTROL+ALT+t"
	(5.2) Place the prompt in the folder "TOPMODEL/scripts2run". To do so, type on the terminal the following command and press ENTER: cd Documents/NEP_DAM/TOPMODEL/scripts2run
	(5.3) To be able to execute the script, type on the terminal the following command and press ENTER: chmod +x PaperDAM.sh
	(5.4) To run the simulations and generate the graphs, type on the terminal the following command and press ENTER: ./PaperDAM.sh RADIOLOGICAL_GROUP_DEVICE TOPMODEL.out 149
	** The simulation will take a couple of days since multiple simulations are generated for different failure probabilities of the device in order to get the 95% confidence interval. When the script finishes, you will find the graphs in the folder "TOPMODEL/resultsProcessed" and the simulation inputs and results in the folder "TOPMODEL/results". 
	The confidence interval, mean and medium will be the same although the distributions shown in the boxes may vary slightingly since the simulations are not deterministic
	** If you want to reduce the number of simulations, open the file "PaperDAM.sh" and reduce the number in line 96 and 160.
	*** If you want to simulate other scenarios (a failure in a different device) you can change RADIOLOGICAL_GROUP_DEVICE by one of the following devices:
	{MOBILEPHONE, LANDLINEPHONE, PRIVATELINEPHONE, SATELLITE_PHONE, RADIOLOGICAL_GROUP_DEVICE, BEEPER, RADIO_REMAN, RADIO_REMER, EMAIL, FAX, TRANKI_GC, TRANKI_E}
	**** If you want to simulate other subset of nodes (NEPHead) make sure you have followed step 4 using NEPHead.txt and in step 5.4 change 149 by 18
	OPTION 2
	(5.1) Open a new Ubuntu terminal. To open Ubuntu terminal press: "CONTROL+ALT+t"
	(5.2) Place the prompt in the folder "TOPMODEL/scripts2run". To do so, type on the terminal the following command and press ENTER: cd Documents/NEP_DAM/TOPMODEL/scripts2run
	(5.3) To be able to execute the script, type on the terminal the following command and press ENTER: chmod +x PaperDAMRunSimulation.sh
	(5.4) To run the simulations and generate the graphs, type on the terminal the following command and press ENTER: ./PaperDAMRunSimulation.sh RADIOLOGICAL_GROUP_DEVICE TOPMODEL.out 149
	** The simulation will take a couple of days since multiple simulations are generated for different failure probabilities of the device in order to get the 95% confidence interval. When the script finishes, you will find the simulation inputs and results in the folder "TOPMODEL/results"
	** If you want to reduce the number of simulations, open the file "PaperDAMRunSimulation.sh" and reduce the number in line 96 and 160
	*** If you want to simulate other scenarios (a failure in a different device) you can change in step 5.4 RADIOLOGICAL_GROUP_DEVICE by one of the following devices:
	{MOBILEPHONE, LANDLINEPHONE, PRIVATELINEPHONE, SATELLITE_PHONE, RADIOLOGICAL_GROUP_DEVICE, BEEPER, RADIO_REMAN, RADIO_REMER, EMAIL, FAX, TRANKI_GC, TRANKI_E}
	**** If you want to simulate other subset of nodes (NEPHead) make sure you have followed step 4 using NEPHead.txt and in step 5.4 change 149 by 18
	(5.5) To be able to execute the script, type on the terminal the following command and press ENTER: chmod +x PaperDAMProcessResults.sh
	(5.6) To run the simulations and generate the graphs, type on the terminal the following command and press ENTER: ./PaperDAMProcessResults.sh RADIOLOGICAL_GROUP_DEVICE TOPMODEL.out 149
	** When the script finishes, you will find the graphs in the folder "TOPMODEL/resultsProcessed"
	The confidence interval, mean and medium will be the same although the distributions shown in the boxes may vary slightly since the simulations are not deterministic
	*** Make sure you are using the same device (e.g. RADIOLOGICAL_GROUP_DEVICE) and the same number (e.g. 149) as in step 5.4 

----------------------------------------------------
ADITIONAL INFORMATION: RUN UNIT TESTS FOR THE ATOMIC MODELS

Unit tests for PersonBehavior atomic model

Test 1
(1) Open a new Ubuntu terminal. To open Ubuntu terminal press: "CONTROL+ALT+t"
(2) Place the prompt in the folder "tests/decisionMaker/unit_test_decision_maker_v2". To do so, type on the terminal the following command and press ENTER: cd Documents/NEP_DAM/tests/decisionMaker/unit_test_decision_maker_v2
(3) Type on the terminal the following command and press ENTER: make clean; make all
(4) Type on the terminal the following command and press ENTER: ./DecisionMaker.out
** You will find the results in the file "DecisionMaker.txt" placed in the folder "tests/decisionMaker/unit_test_decision_maker_v2"

Test 2
(1) Place the prompt in the folder "tests/decisionMaker/XML_test_load_parameters_decision_maker_v2". To do so, type on the terminal the following command and press ENTER: cd ../XML_test_load_parameters_decision_maker_v2
(2) Type on the terminal the following command and press ENTER: make clean; make all
(3) Type on the terminal the following command and press ENTER: ./DecisionMakerLoadFunctions.out
(4) To be able to execute the script, type on the terminal the following command and press ENTER: chmod +x ./EqualFiles.sh
(5) Type on the terminal the following command and press ENTER: ./EqualFiles.sh
** You will find the results in the file "XML_load_diferences" placed in the folder "XML_test_load_parameters_decision_maker_v2"

Unit tests for the other atomic models are run using a single script.
(1) Place the prompt in the folder "tests/tests_atomics". To do so, type on the terminal the following command and press ENTER: cd ../../tests_atomics
(2) Type on the terminal the following command and press ENTER: make clean; make all
(3) To be able to execute the script, type on the terminal the following command and press ENTER: chmod +x ./RunUnitTest.sh
(4) Type on the terminal the following command and press ENTER: ./RunUnitTest.sh
** You will find the results of all the tests in the folder "tests_results"