#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <assert.h>
#include <algorithm>
#include <limits>
#include <stdlib.h>

#include "../data_structures/decision_maker_behaviour_struct_types_V2.hpp"
#include "../data_structures/nep_model_enum_types.hpp"

using namespace std;
using namespace nep_model_enum_types;
using namespace decision_maker_behaviour_structures;

int main(int argc, char ** argv) {

  if (argc < 4) {
   	cout << "you are using this program with wrong parameters. Te program should be invoked as follow:";
   	cout << argv[0] << " \"number of persons in the scenario\" \"path to the folder where the person are stored\" \"#types of devices that break\" \"break probabilty\" \"device\"" <<  endl;
   	return 1;
  } 

  int numberOfPersons = stoi(argv[1]);
  string folder = argv[2];
  int numeberOfDevices = stoi(argv[3]);
  std::vector<pair<int, string>> brokenDevices;
  string fileOut = string("../TOPMODEL/inputs/DevicesInput/ModelSize")+to_string(numberOfPersons)+string("devices")+to_string(numeberOfDevices);
  ofstream myModelfile;
 
  
  if(argc < (4+2*numeberOfDevices)){
    cout << "you are using this program with wrong parameters. Te program should be invoked as follow:";
    cout << argv[0] << " \"number of persons in the scenario\" \"path to the folder where the person are stored\" \"#types of devices that break\" \"break probabilty\" \"device\"" <<  endl;
    return 1;
  }else{
    int j = 4;
    for (int i = 0; i < numeberOfDevices; i++){
      pair<int, string> device = make_pair(stoi(argv[j]), argv[j+1]);
      fileOut += string("_")+argv[j]+argv[j+1];
      j++;
      j++;
      brokenDevices.push_back(device);
    }
    fileOut += string(".txt");
    myModelfile.open(fileOut);
  }
 
  for(int i = 1; i <= numberOfPersons; i++){
    string file_name_in = folder+string("P")+to_string(i)+string(".xml");
    //cout << file_name_in << endl;
    const char * in = file_name_in.c_str();
    DecisionMakerBehaviour person;
    person.load(in);
    if(stoi(person.id) != i) assert(false && "Error coding the files");
    for(int k = 0; k < person.myDevices.size(); k++){
      for(int z = 0; z<brokenDevices.size(); z++){
        if(ToString(person.myDevices[k].device) == brokenDevices[z].second){
          unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // seed variable is used to ensure a different set of results for each simulation
          srand (seed);
          if((rand() % 100) < brokenDevices[z].first){
            myModelfile << "00:00:00:001 " << brokenDevices[z].second << " " <<  person.id << " " << "1" << endl;
          }


          break;
        }
      }
    }
}
return 0;
}