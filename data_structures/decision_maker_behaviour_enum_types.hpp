#ifndef CADMIUM_DECISION_MAKER_BEHAVIOUR_ENUM_TYPES_HPP
#define CADMIUM_DECISION_MAKER_BEHAVIOUR_ENUM_TYPES_HPP

#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

namespace decision_maker_behaviour_enum_types{
  enum class TaskPriorityType {ANSWER, SEND, DO_ACTION};
  enum class AnswerPriorityType {DEVICE_PRIORITY, PERSON_PRIORITY};
  enum class SendPriorityType {RECEPTION_ORDER, INVERSE_RECEPTION_ORDER, PRIORITY_LIST};

  /*******************************************/
  /**************** OPERATORS ****************/
  /*******************************************/
  
  /************* Input stream ************************/
  istream& operator>> (istream& is, TaskPriorityType& tpt);
  istream& operator>> (istream& is, AnswerPriorityType& apt);
  istream& operator>> (istream& is, SendPriorityType& spt);

  /************* Output stream ***********************/
  ostream& operator<<(ostream& os, const TaskPriorityType& tpt);
  ostream& operator<<(ostream& os, const AnswerPriorityType& apt);
  ostream& operator<<(ostream& os, const SendPriorityType& spt);
  /***************To string *****************************/
  const char* ToString(TaskPriorityType v);
  const char* ToString(AnswerPriorityType v);
  const char* ToString(SendPriorityType v);
}
#endif // CADMIUM_DECISION_MAKER_BEHAVIOUR_ENUM_TYPES_HPP