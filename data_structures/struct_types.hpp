#ifndef CADMIUM_STRUCT_TYPES_HPP
#define CADMIUM_STRUCT_TYPES_HPP

#include <assert.h>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <boost/any.hpp>

#include "nep_model_enum_types.hpp"

using namespace std;
using namespace nep_model_enum_types;

namespace nep_structures{
/*******************************************/
/************ Command **********************/
/*******************************************/

struct Command{

  Command(){}
  Command(string i_from, string i_to, string i_command)
   :from(i_from),to(i_to), command(i_command)  {}

  string             from;
  string             to;
  string             command;
  void clear() {
   Command(); 
  }
};

/*******************************************/
/**************** DeviceId *****************/
/*******************************************/

struct DeviceId{
  DeviceId(){}
  DeviceId(DeviceType i_type, string i_id)
    :type(i_type), id(i_id){}

  DeviceType type;
  string     id;
  void clear() {
   DeviceId(); 
  }  
};


/*******************************************/
/************ Command with Device TO *******/
/*******************************************/

struct CommandWithDevice{
  CommandWithDevice(){}
  CommandWithDevice(DeviceId i_from, DeviceId i_to, Command i_command)
    :to(i_to), from(i_from), command(i_command){}

  DeviceId to;
  DeviceId from;
  Command  command;

  void clear() {
   CommandWithDevice(); 
  }
};


/********************************************/
/********* TaskDeviceFinished *******************/
/*******************************************/

struct TaskDeviceFinished{
  TaskDeviceFinished(){}
  TaskDeviceFinished(TaskType i_task,DeviceId i_my_id, DeviceId i_id_with, bool i_outOfOrder, Command i_command)
    :task(i_task),my_id(i_my_id),id_with(i_id_with), out_of_order(i_outOfOrder), command(i_command){}
  TaskDeviceFinished(TaskType i_task,DeviceId i_my_id, DeviceId i_id_with, bool i_outOfOrder, string i_from, string i_to, string i_command)
    :task(i_task),my_id(i_my_id),id_with(i_id_with),out_of_order(i_outOfOrder), command(i_from, i_to, i_command){}
  
  TaskType     task;
  DeviceId     my_id;
  DeviceId     id_with;
  bool         out_of_order;
  Command      command;

  void clear() {
   TaskDeviceFinished(); 
  }  
};

/********************************************/
/********* ActionFinished *******************/
/*******************************************/
struct ActionFinished{
  ActionFinished(){}
  ActionFinished(string id, string action_id, string location){
    this->action_id = action_id;
    this->location = location;
    this->id = id;
  }
    
  string     id;
  string     action_id;
  string     location;

  void clear() {
   ActionFinished(); 
  }  
};

/***************************************/
/********* Action2Do *******************/
/***************************************/
struct Action2Do{
  Action2Do(){}
  Action2Do(string action_id, string location, string executionTime){
    this->action_id = action_id;
    this->location = location;
    this->executionTime = executionTime;
  }    
  string     action_id;
  string     location;
  string     executionTime;

  void clear() {
   Action2Do(); 
  }  
};


/********************************************/
/********* PeopleLocation *******************/
/*******************************************/

struct PeopleLocation{
    PeopleLocation(){}
    PeopleLocation(string location, string id){
      this->location =location;
      this->id = id;
    }
    string location;
    string id;    
  };

/********************************************/
/********* PeopleClassifiedByLocation *******************/
/*******************************************/
struct PeopleClassifiedByLocation{
    PeopleClassifiedByLocation(){}
    PeopleClassifiedByLocation(string location, vector<string> ids){
      this->location =location;
      this->ids = ids;
    }
    string location;
    vector<string> ids;    
  };

/***********************************************/
/********SetDeviceState****************/
/***********************************************/
  struct SetDeviceState{
    SetDeviceState(){};
    SetDeviceState(DeviceId to, bool outOfOrder){
      this->to=to;
      this->outOfOrder=outOfOrder;
    }
    DeviceId to;
    bool outOfOrder;
  };
/***********************************************/
/********SetNetworkState****************/
/***********************************************/
  struct SetNetworkState{
    SetNetworkState(){};
    SetNetworkState(NetworkId to, bool outOfOrder){
      this->to=to;
      this->outOfOrder=outOfOrder;
    }
    NetworkId to;
    bool outOfOrder;
  };

/**************************************************/
istream& operator>> (istream& is, Command& c);
istream& operator>> (istream& is, CommandWithDevice& dc);
istream& operator>> (istream& is, DeviceId& di);
istream& operator>> (istream& is, TaskDeviceFinished& tc);
istream& operator>> (istream& is, ActionFinished& tc);
istream& operator>> (istream& is, PeopleLocation& tc);
istream& operator>> (istream& is, PeopleClassifiedByLocation& tc);
istream& operator>> (istream& is, Action2Do& tc);
istream& operator>> (istream& is, SetDeviceState& tc);
istream& operator>> (istream& is, SetNetworkState& tc);

ostream& operator<<(ostream& os, const Command& c);
ostream& operator<<(ostream& os, const CommandWithDevice& dc);
ostream& operator<<(ostream& os, const DeviceId& di);
ostream& operator<<(ostream& os, const TaskDeviceFinished& tc);
ostream& operator<<(ostream& os, const ActionFinished& tc);
ostream& operator<<(ostream& os, const PeopleLocation& tc);
ostream& operator<<(ostream& os, const PeopleClassifiedByLocation& tc);
ostream& operator<<(ostream& os, const Action2Do& tc);
ostream& operator<<(ostream& os, const SetDeviceState& tc);
ostream& operator<<(ostream& os, const SetNetworkState& tc);

bool operator==(const Command& a, const Command& b);
bool operator==(const DeviceId& di1, const DeviceId& di2);
bool operator==(const CommandWithDevice& a, const CommandWithDevice& b);
bool operator==(const TaskDeviceFinished& di1, const TaskDeviceFinished& di2);
bool operator==(const ActionFinished& di1, const ActionFinished& di2);
bool operator==(const PeopleLocation& di1, const PeopleLocation& di2);
bool operator==(const Action2Do& di1, const Action2Do& di2);

bool operator!=(const DeviceId& di1, const DeviceId& di2);
bool operator!=(const Command& a, const Command& b);


}
#endif // CADMIUM_STRUCT_TYPES_HPP

