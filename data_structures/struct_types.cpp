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
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "struct_types.hpp"
#include "nep_model_enum_types.hpp"

using namespace std;
using namespace nep_model_enum_types;
namespace nep_structures{

  /***************************************************/
  /************* Output stream ************************/
  /***************************************************/

  ostream& operator<<(ostream& os, const Command& c){
    os << c.command << " " << c.from << " " << c.to;
    return os;
  }

  ostream& operator<<(ostream& os, const CommandWithDevice& dc){
    os << dc.from << " " << dc.to << " " << dc.command;
    return os;
  }

  ostream& operator<<(ostream& os, const DeviceId& di){
    os << di.type << " " << di.id;
    return os;
  }

  ostream& operator<<(ostream& os, const TaskDeviceFinished& tc){
    os << tc.task << " " << tc.my_id << " " << tc.id_with << " " << tc.out_of_order << " " << tc.command;
    return os;
  }

  ostream& operator<<(ostream& os, const ActionFinished& tc){
    os << tc.id << " " << tc.action_id << " " << tc.location;
    return os;
  }

  ostream& operator<<(ostream& os, const PeopleLocation& tc){
    os << tc.location << " " << tc.id;
    return os;
  }

   ostream& operator<<(ostream& os, const PeopleClassifiedByLocation& tc){
    os << tc.location;
    for (int i=0; i < tc.ids.size(); i++){
      os << " " << tc.ids[i];
    } 
    return os;
  }

  ostream& operator<<(ostream& os, const Action2Do& tc){
    os << tc.action_id << " " << tc.location << " " << tc.executionTime;
    return os;
  }

   ostream& operator<<(ostream& os, const SetDeviceState& tc){
    os << tc.to << " " << tc.outOfOrder;
    return os;
  }
  ostream& operator<<(ostream& os, const SetNetworkState& tc){
    os << tc.to << " " << tc.outOfOrder;
    return os;
  }
  
  /***************************************************/
  /************* Input stream ************************/
  /***************************************************/

  
  istream& operator>> (istream& is, Command& c){
    is >> c.command;
    is >> c.from;
    is >> c.to;
    return is;
  }

  istream& operator>> (istream& is,CommandWithDevice& dc){
    is >> dc.from;
    is >> dc.to;
    is >> dc.command;
    return is;
  }

  istream& operator>> (istream& is,DeviceId& di){
    is >> di.type;
    is >> di.id;
    return is;
  }

  istream& operator>> (istream& is, TaskDeviceFinished& tc){
    is >> tc.task;
    is >> tc.my_id;
    is >> tc.id_with;
    is >> tc.out_of_order;
    is >> tc.command;
    return is;
  }
  
  istream& operator>> (istream& is, ActionFinished& tc){
    is >> tc.id;
    is >> tc.action_id;
    is >> tc.location;
    return is;
  }
  
  istream& operator>> (istream& is, PeopleLocation& tc){
    is >> tc.location;
    is >> tc.id;
    return is;
  }
   istream& operator>> (istream& is, PeopleClassifiedByLocation& tc){
    is >> tc.location;
    string aux;
    is >> aux;
    tc.ids.push_back(aux);
    return is;
  }

  istream& operator>> (istream& is, Action2Do& tc){
    is >> tc.action_id;
    is >> tc.location;
    is >> tc.executionTime;
    return is;
  }

  istream& operator>> (istream& is, SetDeviceState& tc){
    is >> tc.to;
    is >> tc.outOfOrder;
    return is;
  }
  
  istream& operator>> (istream& is, SetNetworkState& tc){
    is >> tc.to;
    is >> tc.outOfOrder;
    return is;
  }
  
/*******************************************************/
/******************** == *******************************/
/*******************************************************/

bool operator==(const Command& a, const Command& b) {
  bool res = true;
  res = res && (a.from == b.from);
  res = res && (a.to == b.to);
  res = res && (a.command == b.command);
  return res;    
}

bool operator==(const DeviceId& di1, const DeviceId& di2){
  bool res = true;
  res = res && (di1.type == di2.type);
  res = res && (di1.id == di2.id);
  return res; 
}

bool operator==(const CommandWithDevice& a, const CommandWithDevice& b){
   bool res = true;
  res = res && (a.to == b.to);
  res = res && (a.from == b.from);
  res = res && (a.command == b.command);
  return res; 

}
bool operator==(const TaskDeviceFinished& a, const TaskDeviceFinished& b){
   bool res = true;
  res = res && (a.task == b.task);
  res = res && (a.my_id == b.my_id);
  res = res && (a.id_with == b.id_with);
  res = res && (a.out_of_order == b.out_of_order);
  res = res && (a.command == b.command);
  return res; 

}

bool operator==(const ActionFinished& a, const ActionFinished& b){
   bool res = true;
  res = res && (a.id == b.id);
  res = res && (a.action_id == b.action_id);
  res = res && (a.location == b.location);
  return res; 
}

bool operator==(const PeopleLocation& a, const PeopleLocation& b){
   bool res = true;
  res = res && (a.id == b.id);
  res = res && (a.location == b.location);
  return res; 

}

bool operator==(const Action2Do& a, const Action2Do& b){
   bool res = true;
  res = res && (a.executionTime == b.executionTime);
  res = res && (a.action_id == b.action_id);
  res = res && (a.location == b.location);
  return res; 
}

/*******************************************************/
/******************** != *******************************/
/*******************************************************/

bool operator!=(const Command& a, const Command& b) {
  return !(a==b);    
}

bool operator!=(const DeviceId& di1, const DeviceId& di2){
  return !(di1==di2); 
}

}