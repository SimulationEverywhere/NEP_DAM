#include <math.h>
#include <assert.h> 
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>

#include "../../../data_structures/nep_model_enum_types.hpp"
#include "../../../data_structures/decision_maker_behaviour_struct_types_V2.hpp"
#include "../../../data_structures/decision_maker_behaviour_enum_types.hpp"

using namespace std;
using namespace nep_model_enum_types;
using namespace decision_maker_behaviour_structures;
using namespace decision_maker_behaviour_enum_types;

int main(){
	string file_name_in = string("XML_DecisionMakerBehaviour_Eg1_v2.xml");
	DecisionMakerBehaviour decison_maker_behaviour;
  const char * in = file_name_in.c_str();
  decison_maker_behaviour.load(in);
    
  #ifdef DEBUG_PARAMETERS
    string debugname = string("test_load_XML_DecisionMakerBehaviour_Eg1_v2.xml");      
    const char * out = debugname.c_str();
    decison_maker_behaviour.save(out);     
  #endif

	return 1;
}