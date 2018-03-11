#ifndef NEP_MODEL_ENUM_TYPES_HPP
#define NEP_MODEL_ENUM_TYPES_HPP

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

using namespace std;

namespace nep_model_enum_types{

  enum class MessageType {PHONE_MESSAGE, TEXT_MESSAGE, RADIO_MESSAGE};

  enum class PhoneMsgType {CALL_REQUEST, CALL_ACCEPTED, COMMAND_TRANSMITTED, CALL_OVER, OUT_OF_ORDER, NET_OUT_OF_ORDER};
  enum class TextMsgType {NEW, READ, OUT_OF_ORDER, NET_OUT_OF_ORDER};
  enum class RadioMsgType {REQUEST, COMMAND_TRANSMITTED, OUT_OF_ORDER, INTERFERENCES, NET_OUT_OF_ORDER};

  enum class DeviceType {IDLE, MOBILEPHONE, LANDLINEPHONE, PRIVATELINEPHONE, SATELLITE_PHONE, RADIOLOGICAL_GROUP_DEVICE, BEEPER, IN_PERSON, RADIO_REMAN, RADIO_REMER, EMAIL , FAX, TRANKI_GC, TRANKI_E};

  enum class TaskType {ANSWER, SEND, ACTION};

  enum class NetworkId {PHONE_NET, PRIVATELINEPHONE_NET, SATELLITE_NET, RADIOLOGICAL_GROUP_NET, BEEPER_NET, RADIO_REMAN_NET, RADIO_REMER_NET, INTERNET , FAX_NET, TRANKI_GC_NET, TRANKI_E_NET};
  

  /*******************************************/
  /**************** OPERATORS ****************/
  /*******************************************/

  /****************** Input stream ************************/


  istream& operator>> (istream& is, MessageType& msg);
  istream& operator>> (istream& is, PhoneMsgType& pmt);
  istream& operator>> (istream& is, TextMsgType& pmt);
  istream& operator>> (istream& is, RadioMsgType& pmt);

  istream& operator>> (istream& is, DeviceType& dt);
  
  istream& operator>> (istream& is, TaskType& dt);

  istream& operator>> (istream& is, NetworkId& dt);
  
  /***************** Output stream ***********************/
  

  ostream& operator<<(ostream& os, const MessageType& msg);
  
  ostream& operator<<(ostream& os, const PhoneMsgType& pmt);
  ostream& operator<<(ostream& os, const TextMsgType& pmt);
  ostream& operator<<(ostream& os, const RadioMsgType& pmt);

  ostream& operator<<(ostream& os, const DeviceType& dt);
  
  ostream& operator<<(ostream& os, const TaskType& dt);
  ostream& operator<<(ostream& os, const NetworkId& dt);

  /********* To string ********************************/

    const char* ToString(DeviceType v);
    

}

#endif // NEP_MODEL_ENUM_TYPES_HPP