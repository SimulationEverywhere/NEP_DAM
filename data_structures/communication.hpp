#ifndef CADMIUM_COMMUNICATION_HPP
#define CADMIUM_COMMUNICATION_HPP

#include <assert.h>
#include <iostream>
#include <string>

#include "nep_model_enum_types.hpp"
#include "struct_types.hpp"

using namespace std;
using namespace nep_model_enum_types;
using namespace nep_structures;

struct Communication{
  //default
  Communication(){}
  //PHONE_MESSAGE WITHOUT COMMAND
  Communication(MessageType i_type, PhoneMsgType i_msg_phone, DeviceId i_from, DeviceId i_to)
   :type(i_type), from(i_from), to(i_to), msg_phone(i_msg_phone)  {
  }
  //PHONE_MESSAGE WITH COMMAND
  Communication(MessageType i_type, PhoneMsgType i_msg_phone, DeviceId i_from, DeviceId i_to, Command i_command)
   :type(i_type), from(i_from), to(i_to), msg_phone(i_msg_phone), command(i_command)  {
  }
  //RADIO_MESSAGE WITHOUT COMMAND
  Communication(MessageType i_type, RadioMsgType i_msg_radio, DeviceId i_from,  DeviceId i_to)
   :type(i_type), from(i_from), to(i_to), msg_radio(i_msg_radio)  {
  }
  //RADIO_MESSAGE WITH COMMAND
  Communication(MessageType i_type, RadioMsgType i_msg_radio, DeviceId i_from,  DeviceId i_to, Command i_command)
   :type(i_type), from(i_from), to(i_to), msg_radio(i_msg_radio), command(i_command)  {
  }
  //TEXT_MESSAGE WITHOUT COMMAND
  Communication(MessageType i_type, TextMsgType i_msg_text, DeviceId i_from, DeviceId i_to)
   :type(i_type), from(i_from), to(i_to), msg_text(i_msg_text)  {
  }
  //TEXT_MESSAGE WITH COMMAND
  Communication(MessageType i_type, TextMsgType i_msg_text, DeviceId i_from, DeviceId i_to, Command i_command)
   :type(i_type), from(i_from), to(i_to), msg_text(i_msg_text), command(i_command)  {
  }
  
  MessageType         type;
  DeviceId            from;
  DeviceId            to;
  Command             command;
  PhoneMsgType        msg_phone;
  TextMsgType         msg_text;
  RadioMsgType        msg_radio;   
 
  void clear() {
   Communication(); 
  }
};

istream& operator>> (istream& is, Communication& msg);

ostream& operator<<(ostream& os, const Communication& msg);

bool operator==(const Communication& a, const Communication& b);


#endif //CADMIUM_COMMUNICATION_HPP