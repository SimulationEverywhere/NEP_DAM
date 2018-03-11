#include <math.h> 
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "communication.hpp"
#include "nep_model_enum_types.hpp"
#include "struct_types.hpp"

using namespace std;
using namespace nep_model_enum_types;
using namespace nep_structures;


/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator<<(ostream& os, const Communication& msg) {

  os << msg.type << " ";

  switch(msg.type){
    case MessageType::PHONE_MESSAGE: os << msg.msg_phone << " "; break;
    case MessageType::TEXT_MESSAGE: os << msg.msg_text << " "; break;
    case MessageType::RADIO_MESSAGE: os << msg.msg_radio << " "; break;
    default: assert(false && "Not implemented");
  }

  os << msg.from << " " << msg.to << " " << msg.command << " "; 
  return os;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator>> (istream& is, Communication& msg) {
  is >> msg.type;
  switch(msg.type){
    case MessageType::PHONE_MESSAGE:
      is >> msg.msg_phone;      
      break;
    case MessageType::RADIO_MESSAGE:
      is >> msg.msg_radio;
      break;
    case MessageType::TEXT_MESSAGE:
      is >> msg.msg_text;
      break;
    default: assert(false && "Not implemented");break;
  }
  is >> msg.from;
  is >> msg.to;
  is >> msg.command;
  return is;
}

/*******************************************************/
/******************** == *******************************/
/*******************************************************/

bool operator==(const Communication& a, const Communication& b) {
  bool res = true;
  res = res && (a.type == b.type);
  res = res && (a.from == b.from);
  res = res && (a.to == b.to);
  res = res && (a.msg_phone == b.msg_phone);
  res = res && (a.msg_text == b.msg_text);
  res = res && (a.msg_radio == b.msg_radio);
  res = res && (a.command == b.command);
  return res;    
}
