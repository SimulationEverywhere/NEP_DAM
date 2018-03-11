#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "decision_maker_behaviour_enum_types.hpp"

using namespace std;

namespace decision_maker_behaviour_enum_types{

  /***************************************************/
  /************* Output stream ***********************/
  /***************************************************/

  ostream& operator<<(ostream& os, const TaskPriorityType& tpt){
    switch(tpt){
      case TaskPriorityType::ANSWER: os << "ANSWER"; break;
      case TaskPriorityType::SEND: os << "SEND"; break;
      case TaskPriorityType::DO_ACTION: os << "DO_ACTION"; break;
      default: assert(false && "The TaskPriorityType do not have the operator <<"); break;
    }
    return os;
  }
  ostream& operator<<(ostream& os, const AnswerPriorityType& apt){
    switch(apt){
      case AnswerPriorityType::DEVICE_PRIORITY: os << "DEVICE_PRIORITY"; break;
      case AnswerPriorityType::PERSON_PRIORITY: os << "PERSON_PRIORITY"; break;
      default: assert(false && "The AnswerPriorityType do not have the operator <<"); break;
    }
    return os;
  }
  ostream& operator<<(ostream& os, const SendPriorityType& spt){
    switch(spt){
      case SendPriorityType::RECEPTION_ORDER: os << "RECEPTION_ORDER"; break;
      case SendPriorityType::INVERSE_RECEPTION_ORDER: os << "INVERSE_RECEPTION_ORDER"; break;
      case SendPriorityType::PRIORITY_LIST: os << "PRIORITY_LIST"; break;
      default: assert(false && "The SendPriorityType do not have the operator <<"); break;
    }
    return os;
  }   
    
  /***************************************************/
  /************* Input stream ************************/
  /***************************************************/  
  
  istream& operator>> (istream& is, TaskPriorityType& tpt){
    string isT;
    is >> isT;
    if (isT == "ANSWER")                  tpt = TaskPriorityType::ANSWER;
    else if (isT == "SEND")               tpt = TaskPriorityType::SEND;  
    else if (isT == "DO_ACTION")          tpt = TaskPriorityType::DO_ACTION;
    else assert(false && "wrong TaskPriorityType.");
    return is;
  }

  istream& operator>> (istream& is, AnswerPriorityType& apt){
    string isT;
    is >> isT;
    if (isT == "DEVICE_PRIORITY")          apt = AnswerPriorityType::DEVICE_PRIORITY;
    else if (isT == "PERSON_PRIORITY")     apt = AnswerPriorityType::PERSON_PRIORITY;  
    else assert(false && "wrong AnswerPriorityType.");
    return is;
  }

  istream& operator>> (istream& is, SendPriorityType& spt){
    string isT;
    is >> isT;
    if (isT == "RECEPTION_ORDER")               spt = SendPriorityType::RECEPTION_ORDER;
    else if (isT == "INVERSE_RECEPTION_ORDER")  spt = SendPriorityType::INVERSE_RECEPTION_ORDER;  
    else if (isT == "PRIORITY_LIST")            spt = SendPriorityType::PRIORITY_LIST;  
    else assert(false && "wrong SendPriorityType.");
    return is;
  }

  /**************************************************/
  /************   TO STRING   ***********************/
  /**************************************************/

  const char* ToString(TaskPriorityType v){
    switch (v){
      case TaskPriorityType::ANSWER:   return "ANSWER";
      case TaskPriorityType::SEND:   return "SEND";
      case TaskPriorityType::DO_ACTION: return "DO_ACTION";
      default: return "[Unknown type]";
    }
   }
  const char* ToString(AnswerPriorityType v){
    switch (v){
      case AnswerPriorityType::DEVICE_PRIORITY:   return "DEVICE_PRIORITY";
      case AnswerPriorityType::PERSON_PRIORITY:   return "PERSON_PRIORITY";
      default: return "[Unknown type]";
    }
  }
  const char* ToString(SendPriorityType v){
    switch (v){
      case SendPriorityType::RECEPTION_ORDER:   return "RECEPTION_ORDER";
      case SendPriorityType::INVERSE_RECEPTION_ORDER:   return "INVERSE_RECEPTION_ORDER";
      case SendPriorityType::PRIORITY_LIST: return "PRIORITY_LIST";
      default: return "[Unknown type]";
    }
  }
  

} //closing namespace

