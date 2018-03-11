#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <assert.h>
#include <algorithm>
#include <limits>
#include <stdlib.h>

#include "generateTOPinclude/functions2InstantiateAtomics.hpp"
#include "generateTOPinclude/functions2InstantiateCoupleds.hpp"
#include "../data_structures/decision_maker_behaviour_struct_types_V2.hpp"
#include "../data_structures/nep_model_enum_types.hpp"

using namespace std;
using namespace nep_model_enum_types;
using namespace decision_maker_behaviour_structures;

struct Nodes{
    string id;
    string label;
    string location;
};

istream& operator>> (istream& is, Nodes& n);
ostream& operator<<(ostream& os, const Nodes& n);

int main(int argc, char ** argv) {

  if (argc < 2) {
   	cout << "you are using this program with wrong parameters. Te program should be invoked as follow:";
   	cout << argv[0] << "\"Path to the file where the nodes are stored \"" << endl;
   	return 1;
  } 
  string nodefile = argv[1];
  string folder = string("../NEPData/XMLs/");
  string mainModel = string("../TOPMODEL/Mainmodel.cpp");
  ofstream myModelfile;
  myModelfile.open(mainModel);
  vector<string> TOP = open_coupled(string("TOP"));
  string tIPORTS = "";
  string tOPORTS = "outp_taskDeviceFinished, outp_taskActionFinished";
  string tSUBMODELS;
  string tIC;
  string tEIC;
  string tEOC;
  vector<Nodes> network_nodes;
  {
    ifstream infile(nodefile);
     Nodes node;
      if(!infile) {
          cout << "Cannot open input file.\n";
          return 1;
      }
      //infile.get();
      if(infile.eof()!=true) {
          string str;
          getline(infile, str);
      }else{
          cout << "file empty" << endl;
          return 1;
      }
      while(infile.eof()!=true){
          infile >> node;
          network_nodes.push_back(node);
          //cout << node << endl;
      }
    infile.close();
  }
  
  /****Print block 1  headers & port declaration for coupleds******/
 {
    ifstream infile("generateTOPinclude/TOPBlock1");
    string content = "";
    for(int i=0 ; infile.eof()!=true ; i++){ // get content of infile
        content += infile.get();
    }
    content.erase(content.end()-1); 
    infile.close();
    myModelfile << content << endl; // output
 }
  //cout << "1 - headers & port declaration file parsed" << endl;

  myModelfile << string("/********************************************/") << endl;
  myModelfile << string("/****** COMMAND GENERATOR *******************/") << endl;
  myModelfile << string("/********************************************/") << endl;
  myModelfile << string("template<typename TIME>") << endl;
  myModelfile << string("class CommandGenerator : public iestream_input<Command,TIME> {") << endl;
  myModelfile << string("public:") << endl;
  myModelfile << string("  CommandGenerator(): iestream_input<Command,TIME>(\"../inputs/Scenario/Command.txt\") {};") << endl;
  myModelfile << string("};") << endl;

  myModelfile << string("/********************************************/") << endl;
  myModelfile << string("/****** DEVICES STATE ***********************/") << endl;
  myModelfile << string("/********************************************/") << endl;
  myModelfile << string("template<typename TIME>") << endl;
  myModelfile << string("class DeviceState : public iestream_input<SetDeviceState,TIME> {") << endl;
  myModelfile << string("public:") << endl;
  myModelfile << string("  DeviceState(): iestream_input<SetDeviceState,TIME>(\"../inputs/Scenario/Devices.txt\") {};") << endl;
  myModelfile << string("};") << endl;

  myModelfile << string("/********************************************/") << endl;
  myModelfile << string("/****** NETWORKS STATE **********************/") << endl;
  myModelfile << string("/********************************************/") << endl;
  myModelfile << string("template<typename TIME>") << endl;
  myModelfile << string("class NetworksState : public iestream_input<SetNetworkState,TIME> {") << endl;
  myModelfile << string("public:") << endl;
  myModelfile << string("  NetworksState(): iestream_input<SetNetworkState,TIME>(\"../inputs/Scenario/NetState.txt\") {};") << endl;
  myModelfile << string("};") << endl;

  myModelfile << string("/********************************/") << endl;
  myModelfile << string("/*** FACE TO FACE CONNECTOR *****/") << endl;
  myModelfile << string("/********************************/") << endl;
  myModelfile << create_atomic_NetPhone(string("Face2Face"), string("0"), string("00:00:00:100")) << endl;  

  myModelfile << string("/**************************************/") << endl;
  myModelfile << string("/*** PEOPLE IN LOCATION  **************/") << endl;
  myModelfile << string("/**************************************/") << endl;
  myModelfile << "/*Do not need instantiation*/" << endl;  

  myModelfile << string("/************************/") << endl;
  myModelfile << string("/****DEFINE NETWORKS*****/") << endl;
  myModelfile << string("/************************/") << endl;
  pair<vector<string>,vector<string>> NETWORKS = NetworksCoupledModel(); 
  for(int j = 0; j<NETWORKS.first.size(); j++){
        myModelfile << NETWORKS.first[j] << endl;
  }      
  myModelfile << endl;
  myModelfile << print_coupled(NETWORKS.second) << endl;
  myModelfile << endl;            
  
  //ADD COMPONENTS TO THE TOP MODEL
  tSUBMODELS += "CommandGenerator, DeviceState, NetworksState, NetPhoneFace2Face, PeopleInLocation, NETWORKS,";
  tIC += "cadmium::modeling::IC<NetworksState, iestream_input_defs<SetNetworkState>::out, NETWORKS, inp_NetworksState>,";  

  //cout << "2 - Generators (3), Face to face connector, pleopleInLocation & NETWORKS generated" << endl;
  vector<string> modelsId;
  /*******Define persons and devices. They are coupled models*****/
    for(int i = 0; i < network_nodes.size(); i++){
      //cout << "Person " << i << endl;
      myModelfile << string("/********************************/") << endl;
      myModelfile << string("/***DEFINE PERSON & DEVICE ")+network_nodes[i].id+string("****/") << endl;
      myModelfile << string("/********************************/") << endl;
      string file_name_in = folder+network_nodes[i].label+string(".xml");
      //cout << file_name_in << endl;
      const char * in = file_name_in.c_str();
      DecisionMakerBehaviour person;
      person.load(in);
      if(person.id != network_nodes[i].id) assert(false && "Error coding the files");
      modelsId.push_back(person.id);
      /****DEVICES*****/
      pair<vector<string>,vector<vector<string>>> DEVICES = DevicesCoupledModel(person);      
      for(int j = 0; j<DEVICES.first.size(); j++){
        myModelfile << DEVICES.first[j] << endl;
      }      
      myModelfile << endl;
      for(int j = 0; j<DEVICES.second.size(); j++){
        myModelfile << print_coupled(DEVICES.second[j]) << endl;
        myModelfile << endl;            
      }
      //cout << "Devices de la persona " << i << " generados" << endl;  
      /****PERSON*****/
        pair<vector<string>, vector<vector<string>>> PERSON = PersonCoupledModel((string("../")+file_name_in), person);
        for(int j = 0; j<PERSON.first.size(); j++){
            myModelfile << PERSON.first[j] << endl;
        }
        myModelfile << endl;
        for(int j = 0; j<PERSON.second.size(); j++){
            myModelfile << print_coupled(PERSON.second[j]) << endl;
            myModelfile << endl;
        }
        //cout << "Persona " << i << " generada" << endl;
      }

      int peopledAdded = 0;
      int numberCoupled = 0;
      while(peopledAdded < network_nodes.size()){        
        int personInCoupled = 0;
        ++numberCoupled;
        vector<string> CM = open_coupled(string("CM")+to_string(numberCoupled));
        string cmIPORTS = "inp_generator,inp_DevicesState,inp_peopleInMyLocation, inp_atmosphere, inp_network";
        string cmOPORTS = "outp_taskDeviceFinished, outp_taskActionFinished, outp_myLocation, outp_atmosphere, outp_network";
        string cmSUBMODELS;
        string cmIC;
        string cmEIC;
        string cmEOC;
        while(personInCoupled < 30 && peopledAdded < network_nodes.size()){                   
          cmSUBMODELS += "DEVICES"+modelsId[peopledAdded]+",";
          cmSUBMODELS += "PERSON"+modelsId[peopledAdded]+",";
          cmEIC += "cadmium::modeling::EIC<inp_generator,PERSON"+modelsId[peopledAdded]+", inp_generator>,";
          cmEIC += "cadmium::modeling::EIC<inp_DevicesState,DEVICES"+modelsId[peopledAdded]+", inp_DevicesState>,";
          cmEIC += "cadmium::modeling::EIC<inp_peopleInMyLocation,PERSON"+modelsId[peopledAdded]+", inp_peopleInMyLocation>,";
          cmEIC += "cadmium::modeling::EIC<inp_atmosphere,PERSON"+modelsId[peopledAdded]+", inp_atmosphere>,";
          cmEIC += "cadmium::modeling::EIC<inp_network,DEVICES"+modelsId[peopledAdded]+", inp_network>,";

          cmEOC += "cadmium::modeling::EOC<PERSON"+modelsId[peopledAdded]+", outp_taskDeviceFinished, outp_taskDeviceFinished>,";
          cmEOC += "cadmium::modeling::EOC<PERSON"+modelsId[peopledAdded]+", outp_taskActionFinished, outp_taskActionFinished>,";
          cmEOC += "cadmium::modeling::EOC<PERSON"+modelsId[peopledAdded]+", outp_atmosphere, outp_atmosphere>,";
          cmEOC += "cadmium::modeling::EOC<PERSON"+modelsId[peopledAdded]+", outp_myLocation, outp_myLocation>,"; 
          cmEOC += "cadmium::modeling::EOC<DEVICES"+modelsId[peopledAdded]+", outp_network, outp_network>,";
          
          cmIC += "cadmium::modeling::IC<PERSON"+modelsId[peopledAdded]+", outp_out_com, DEVICES"+modelsId[peopledAdded]+", inp_in_com>,";
          cmIC += "cadmium::modeling::IC<DEVICES"+modelsId[peopledAdded]+", outp_out_com, PERSON"+modelsId[peopledAdded]+", inp_in_com>,";
          ++personInCoupled;
          ++peopledAdded; 
        }
        cmSUBMODELS.pop_back();
        cmIC.pop_back();
        cmEIC.pop_back();
        cmEOC.pop_back();
        CM[0] += cmIPORTS;
        CM[1] += cmOPORTS;
        CM[2] += cmSUBMODELS;  
        CM[3] += cmEIC;
        CM[4] += cmEOC;
        CM[5] += cmIC;
        close_coupled(CM);  
        myModelfile << print_coupled(CM) << endl;

        /****ADD IT TO THE TOP MODEL AND MAKE CONEXIONS*****/
        tSUBMODELS += "CM"+to_string(numberCoupled)+",";
        //cout << "CM - NUMERO" << numberCoupled << endl;

        tIC += "cadmium::modeling::IC<PeopleInLocation, PeopleInLocation_defs::out, CM"+to_string(numberCoupled)+", inp_peopleInMyLocation>,";  
        tIC += "cadmium::modeling::IC<CM"+to_string(numberCoupled)+", outp_myLocation, PeopleInLocation, PeopleInLocation_defs::in>,";
      
        tIC += "cadmium::modeling::IC<CommandGenerator, iestream_input_defs<Command>::out, CM"+to_string(numberCoupled)+", inp_generator>,";  
        tIC += "cadmium::modeling::IC<DeviceState, iestream_input_defs<SetDeviceState>::out, CM"+to_string(numberCoupled)+", inp_DevicesState>,";

        tIC += "cadmium::modeling::IC<CM"+to_string(numberCoupled)+", outp_atmosphere, NetPhoneFace2Face, NetPhone_defs::in>,";              
        tIC += "cadmium::modeling::IC<NetPhoneFace2Face, NetPhone_defs::out, CM"+to_string(numberCoupled)+", inp_atmosphere>,";
        

        tIC += "cadmium::modeling::IC<CM"+to_string(numberCoupled)+", outp_network, NETWORKS, inp_in_com>,";
        tIC += "cadmium::modeling::IC<NETWORKS, outp_out_com, CM"+to_string(numberCoupled)+", inp_network>,";
      
        tEOC += "cadmium::modeling::EOC<CM"+to_string(numberCoupled)+", outp_taskDeviceFinished, outp_taskDeviceFinished>,";
        tEOC += "cadmium::modeling::EOC<CM"+to_string(numberCoupled)+", outp_taskActionFinished, outp_taskActionFinished>,"; 
      } 

  myModelfile << string("/************************/") << endl;
  myModelfile << string("/*******TOP MODEL********/") << endl;
  myModelfile << string("/************************/") << endl;
  tSUBMODELS.pop_back();
  tIC.pop_back();
  tEOC.pop_back();
   
  TOP[0] += tIPORTS;
  TOP[1] += tOPORTS;
  TOP[2] += tSUBMODELS;  
  TOP[3] += tEIC;
  TOP[4] += tEOC;
  TOP[5] += tIC;
  close_coupled(TOP);  
  myModelfile << print_coupled(TOP) << endl;
  /****Print last block *****/
    {
    	ifstream infile("generateTOPinclude/TOPBlock2");
      string content = "";
      for(int i=0 ; infile.eof()!=true ; i++){ // get content of infile
          content += infile.get();
      }
      content.erase(content.end()-1); 
      infile.close();
      myModelfile << content << endl; // output
	 }
  myModelfile.close();
  cout << "fin" << endl;  
  return 0;
}


ostream& operator<<(ostream& os, const Nodes& n){
    os << n.id << " " << n.label << " " << n.location;
    return os;
}

istream& operator>> (istream& is, Nodes& n){
    is >> n.id;
    is >> n.label;
    is >> n.location;
    return is;
}