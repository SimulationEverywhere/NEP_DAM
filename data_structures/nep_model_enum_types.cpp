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

#include "nep_model_enum_types.hpp"

using namespace std;

namespace nep_model_enum_types{
  
  /***************************************************/
  /************* Output stream ***********************/
  /***************************************************/

  ostream& operator<<(ostream& os, const MessageType& msg){
  switch(msg){
    case MessageType::PHONE_MESSAGE: os << "PHONE_MESSAGE"; break;
    case MessageType::TEXT_MESSAGE: os << "TEXT_MESSAGE"; break;
    case MessageType::RADIO_MESSAGE: os << "RADIO_MESSAGE"; break;
    default: assert(false && "The MessageType do not have the operator <<"); break;
  }
  return os;
}
  
  ostream& operator<< (ostream& os, const PhoneMsgType& pmt){
    switch(pmt){
      case PhoneMsgType::CALL_REQUEST: os << "CALL_REQUEST"; break;
      case PhoneMsgType::CALL_ACCEPTED: os << "CALL_ACCEPTED"; break;
      case PhoneMsgType::COMMAND_TRANSMITTED: os << "COMMAND_TRANSMITTED"; break;
      case PhoneMsgType::CALL_OVER: os << "CALL_OVER"; break;      
      case PhoneMsgType::OUT_OF_ORDER: os << "OUT_OF_ORDER"; break;
      case PhoneMsgType::NET_OUT_OF_ORDER: os << "NET_OUT_OF_ORDER"; break;
      default: assert(false && "The PhoneMsgType do not have the operator <<"); break;
    }
    return os;
  }

  ostream& operator<< (ostream& os, const TextMsgType& pmt){
    switch(pmt){
      case TextMsgType::NEW: os << "NEW"; break;
      case TextMsgType::READ: os << "READ"; break;
      case TextMsgType::OUT_OF_ORDER: os << "OUT_OF_ORDER"; break;
      case TextMsgType::NET_OUT_OF_ORDER: os << "NET_OUT_OF_ORDER"; break;
      default: assert(false && "The TextMsgType do not have the operator <<"); break;
    }
    return os;
  } 

  ostream& operator<< (ostream& os, const RadioMsgType& pmt){
    switch(pmt){
      case RadioMsgType::REQUEST: os << "REQUEST"; break;
      case RadioMsgType::COMMAND_TRANSMITTED: os << "COMMAND_TRANSMITTED"; break;
      case RadioMsgType::INTERFERENCES: os << "INTERFERENCES"; break;      
      case RadioMsgType::OUT_OF_ORDER: os << "OUT_OF_ORDER"; break;
      case RadioMsgType::NET_OUT_OF_ORDER: os << "NET_OUT_OF_ORDER"; break;
      default: assert(false && "The RadioMsgType do not have the operator <<"); break;
    }
    return os;
  }

  ostream& operator<< (ostream& os, const DeviceType& dt){
    switch(dt) {
      case DeviceType::EMAIL: os << "EMAIL"; break;
      case DeviceType::LANDLINEPHONE: os << "LANDLINEPHONE"; break;
      case DeviceType::BEEPER: os << "BEEPER"; break;
      case DeviceType::IN_PERSON: os << "IN_PERSON"; break;
      case DeviceType::MOBILEPHONE: os << "MOBILEPHONE"; break;
      case DeviceType::SATELLITE_PHONE: os << "SATELLITE_PHONE"; break;
      case DeviceType::RADIOLOGICAL_GROUP_DEVICE: os << "RADIOLOGICAL_GROUP_DEVICE"; break;
      case DeviceType::IDLE: os << "IDLE"; break;
      case DeviceType::RADIO_REMAN: os << "RADIO_REMAN"; break;
      case DeviceType::RADIO_REMER: os << "RADIO_REMER"; break;
      case DeviceType::FAX: os << "FAX"; break;
      case DeviceType::TRANKI_E: os << "TRANKI_E"; break;
      case DeviceType::TRANKI_GC: os << "TRANKI_GC"; break;
          case DeviceType::PRIVATELINEPHONE: os << "PRIVATELINEPHONE"; break;
      default: assert(false && "The DeviceType do not have the operator <<"); break;
    }
    return os;
  }

  ostream& operator<< (ostream& os, const NetworkId& dt){
    switch(dt) {
      case NetworkId::PHONE_NET: os << "PHONE_NET"; break;
      case NetworkId::PRIVATELINEPHONE_NET: os << "PRIVATELINEPHONE_NET"; break;
      case NetworkId::SATELLITE_NET: os << "SATELLITE_NET"; break;
      case NetworkId::RADIOLOGICAL_GROUP_NET: os << "RADIOLOGICAL_GROUP_NET"; break;
      case NetworkId::BEEPER_NET: os << "BEEPER_NET"; break;
      case NetworkId::RADIO_REMAN_NET: os << "RADIO_REMAN_NET"; break;
      case NetworkId::RADIO_REMER_NET: os << "RADIO_REMER_NET"; break;
      case NetworkId::INTERNET: os << "INTERNET"; break;
      case NetworkId::FAX_NET: os << "FAX_NET"; break;
      case NetworkId::TRANKI_GC_NET: os << "TRANKI_GC_NET"; break;
      case NetworkId::TRANKI_E_NET: os << "TRANKI_E_NET"; break;
      default: assert(false && "The NetworkId do not have the operator <<"); break;
    }
    return os;
  }

 
  ostream& operator<<(ostream& os, const TaskType& dt){
    switch(dt) {
        case TaskType::ANSWER: os << "ANSWER"; break;
        case TaskType::SEND: os << "SEND"; break;
        case TaskType::ACTION: os << "ACTION"; break;
        default: assert(false && "The TaskType do not have the operator <<"); break;
      }
      return os;
  }



  /***************************************************/
  /************* Input stream ************************/
  /***************************************************/

  istream& operator>> (istream& is, MessageType& msg){
  string isT;
  is >> isT;
  if (isT == "PHONE_MESSAGE")                 msg = MessageType::PHONE_MESSAGE;
  else if (isT == "TEXT_MESSAGE")             msg = MessageType::TEXT_MESSAGE;
  else if (isT == "RADIO_MESSAGE")            msg = MessageType::RADIO_MESSAGE;
  else assert(false && "wrong MessageType.");
  return is;  
}


  istream& operator>>(istream& is, PhoneMsgType& pmt){
    string isT;
    is >> isT;    
    if (isT == "CALL_REQUEST")              pmt = PhoneMsgType::CALL_REQUEST;  
    else if (isT == "CALL_ACCEPTED")        pmt = PhoneMsgType::CALL_ACCEPTED;
    else if (isT == "COMMAND_TRANSMITTED")   pmt = PhoneMsgType::COMMAND_TRANSMITTED;
    else if (isT == "CALL_OVER")            pmt = PhoneMsgType::CALL_OVER;
    else if (isT == "OUT_OF_ORDER")         pmt = PhoneMsgType::OUT_OF_ORDER;
    else if (isT == "NET_OUT_OF_ORDER")         pmt = PhoneMsgType::NET_OUT_OF_ORDER;
    else assert(false && "wrong PhoneMsgType.");
    return is;
  }

  istream& operator>>(istream& is, TextMsgType& pmt){
    string isT;
    is >> isT;
    if (isT == "NEW")                  pmt = TextMsgType::NEW;
    else if (isT == "READ")            pmt = TextMsgType::READ;
    else if (isT == "OUT_OF_ORDER")    pmt = TextMsgType::OUT_OF_ORDER; 
    else if (isT == "NET_OUT_OF_ORDER")    pmt = TextMsgType::NET_OUT_OF_ORDER;     
    else assert(false && "wrong TextMsgType.");
    return is;
  }

  istream& operator>>(istream& is, RadioMsgType& pmt){
    string isT;
    is >> isT;    
    if (isT == "REQUEST")              pmt = RadioMsgType::REQUEST;
    else if (isT == "COMMAND_TRANSMITTED")   pmt = RadioMsgType::COMMAND_TRANSMITTED;
    else if (isT == "INTERFERENCES")            pmt = RadioMsgType::INTERFERENCES;
    else if (isT == "OUT_OF_ORDER")         pmt = RadioMsgType::OUT_OF_ORDER;
    else if (isT == "NET_OUT_OF_ORDER")         pmt = RadioMsgType::NET_OUT_OF_ORDER;
    else assert(false && "Operator >> wrong RadioMsgType.");
    return is;
  }

  istream& operator>>(istream& is, DeviceType& dt){
    string isT;
    is >> isT;
    if (isT == "EMAIL")                   dt = DeviceType::EMAIL;
    else if (isT == "LANDLINEPHONE")      dt = DeviceType::LANDLINEPHONE; 
    else if (isT == "BEEPER")             dt = DeviceType::BEEPER;
    else if (isT == "IN_PERSON")          dt = DeviceType::IN_PERSON;
    else if (isT == "MOBILEPHONE")        dt = DeviceType::MOBILEPHONE;
    else if (isT == "IDLE")               dt = DeviceType::IDLE;
    else if (isT == "RADIO_REMAN")              dt = DeviceType::RADIO_REMAN;
    else if (isT == "RADIO_REMER")              dt = DeviceType::RADIO_REMER;
    else if (isT == "SATELLITE_PHONE")     dt = DeviceType::SATELLITE_PHONE;
    else if (isT == "RADIOLOGICAL_GROUP_DEVICE")     dt = DeviceType::RADIOLOGICAL_GROUP_DEVICE;
    else if (isT == "FAX")                dt = DeviceType::FAX;
    else if (isT == "TRANKI_GC")                dt = DeviceType::TRANKI_GC;
    else if (isT == "TRANKI_E")                dt = DeviceType::TRANKI_E;
     else if (isT == "PRIVATELINEPHONE")                dt = DeviceType::PRIVATELINEPHONE;
    else assert(false && "wrgon DeviceType.");
    return is;
  }

  istream& operator>>(istream& is, NetworkId& dt){
    string isT;
    is >> isT;
    if (isT == "PHONE_NET")                   dt = NetworkId::PHONE_NET;
    else if (isT == "PRIVATELINEPHONE_NET")      dt = NetworkId::PRIVATELINEPHONE_NET; 
    else if (isT == "SATELLITE_NET")             dt = NetworkId::SATELLITE_NET;
    else if (isT == "RADIOLOGICAL_GROUP_NET")          dt = NetworkId::RADIOLOGICAL_GROUP_NET;
    else if (isT == "BEEPER_NET")        dt = NetworkId::BEEPER_NET;
    else if (isT == "RADIO_REMAN_NET")               dt = NetworkId::RADIO_REMAN_NET;
    else if (isT == "RADIO_REMER_NET")              dt = NetworkId::RADIO_REMER_NET;
    else if (isT == "INTERNET")              dt = NetworkId::INTERNET;
    else if (isT == "FAX_NET")     dt = NetworkId::FAX_NET;
    else if (isT == "TRANKI_GC_NET")     dt = NetworkId::TRANKI_GC_NET;
    else if (isT == "TRANKI_E_NET")                dt = NetworkId::TRANKI_E_NET;
    return is;
  }

  istream& operator>> (istream& is, TaskType& dt){
    string isT;
    is >> isT;
    if (isT == "SEND")                    dt = TaskType::SEND;
    else if (isT == "ANSWER")             dt = TaskType::ANSWER; 
    else if (isT == "ACTION")             dt = TaskType::ACTION;
    else assert(false && "wrgon TaskType.");
    return is;    
  }  

  /*************************************************/
  /************* To string  ************************/
  /*************************************************/

  const char* ToString(DeviceType v){
    switch (v){
      case DeviceType::EMAIL: return "EMAIL";
      case DeviceType::LANDLINEPHONE: return "LANDLINEPHONE";
      case DeviceType::BEEPER: return "BEEPER";
      case DeviceType::IN_PERSON: return "IN_PERSON";
      case DeviceType::MOBILEPHONE: return "MOBILEPHONE";
      case DeviceType::IDLE: return "IDLE";
      case DeviceType::RADIO_REMER: return "RADIO_REMER";
      case DeviceType::RADIO_REMAN: return "RADIO_REMAN";
      case DeviceType::SATELLITE_PHONE: return "SATELLITE_PHONE";
      case DeviceType::RADIOLOGICAL_GROUP_DEVICE: return "RADIOLOGICAL_GROUP_DEVICE";
      case DeviceType::FAX: return "FAX";
      case DeviceType::TRANKI_GC: return "TRANKI_GC";
      case DeviceType::TRANKI_E: return "TRANKI_E";
      case DeviceType::PRIVATELINEPHONE: return "PRIVATELINEPHONE";
      default: assert(false && "wrgon DeviceType."); return "[Unknown type]";
    }
  }

}