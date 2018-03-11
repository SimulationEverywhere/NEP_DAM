/**
 * Copyright (c) 2017, Cristina Ruiz Martin
 * Carleton University, Universidad de Valladolid
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CADMIUM_DEVICE_PHONE_MULTIPLE_HPP
#define CADMIUM_DEVICE_PHONE_MULTIPLE_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>
#include <iostream>

#include "../data_structures/struct_types.hpp"
#include "../data_structures/communication.hpp"
#include "../data_structures/nep_model_enum_types.hpp"

using namespace std;
using namespace cadmium;

using namespace nep_structures;
using namespace nep_model_enum_types;

//Port definition
struct DevicePhoneMultiple_defs{
  //Input ports
  struct microphoneIn : public in_port<Communication> {};
  struct networkIn : public in_port<Communication> {};
  struct setStateIn : public in_port<SetDeviceState> {};
  //output ports
  struct speakerOut : public out_port<Communication> {};
  struct networkOut : public out_port<Communication> {};         
};
//MODEL
template<typename TIME>
class DevicePhoneMultiple {
  using defs=DevicePhoneMultiple_defs; // putting definitions in context
  public:
    enum class PhoneState {IDLE, CALL_REQUESTED, INCOMING_CALL_REQUESTED, COMMUNICATION_PARTIALLY_STABLISHED, COMMUNICATION_FULLY_STABLISHED, COMMAND_TRANSMITTED, COMMUNICATION_OVER, OUT_OF_ORDER};
    //Parameters
    DeviceId id;
    TIME timeProcessing;
    TIME timeRinging;
    // default constructor
    DevicePhoneMultiple(DeviceId id_param, TIME timeProcessing_param, TIME timeRinging_param) noexcept {
      id = id_param;
      timeRinging = timeRinging_param;
      timeProcessing = timeProcessing_param;
      state.state = PhoneState::IDLE;
      state.advanceTime = std::numeric_limits<TIME>::infinity();
    }
    // state definition          
    struct state_type{
      vector <Communication>  networkOutMsg;
      vector <Communication>  speakerOutMsg;
      vector<DeviceId>        busyWithRequest;
      vector<DeviceId>        busyWithStablished;
      TIME                    advanceTime;
      TIME                    remainingTimeRinging;
      PhoneState              state;
    };
    state_type state;
    // ports definition
    using input_ports=std::tuple<typename defs::microphoneIn, typename defs::setStateIn, typename defs::networkIn>;
    using output_ports=std::tuple<typename defs::speakerOut, typename defs::networkOut>;
    // internal transition
    void internal_transition() {
      if(!state.networkOutMsg.empty() || !state.speakerOutMsg.empty()){
        state.networkOutMsg.clear();
        state.speakerOutMsg.clear();
        (state.state == PhoneState::CALL_REQUESTED || state.state == PhoneState::COMMUNICATION_PARTIALLY_STABLISHED) ? state.advanceTime = state.remainingTimeRinging : state.advanceTime = std::numeric_limits<TIME>::infinity();
      }else{ // - The time requesting is over
        if(state.state == PhoneState::CALL_REQUESTED || state.state == PhoneState::COMMUNICATION_PARTIALLY_STABLISHED){
          for(int i=0; i < state.busyWithRequest.size(); i++){
             state.networkOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id, state.busyWithRequest[i]));
          }                 
          for(int i=0; i< state.busyWithRequest.size(); i++){          
            state.speakerOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, state.busyWithRequest[i], id));
          }        
          state.advanceTime = timeProcessing;
          if(state.state == PhoneState::COMMUNICATION_PARTIALLY_STABLISHED){
            state.state = PhoneState::COMMUNICATION_FULLY_STABLISHED; 
            state.busyWithRequest.clear();
          }   
          else if (state.state == PhoneState::CALL_REQUESTED) state.state = PhoneState::COMMUNICATION_OVER;
          else assert(false && "Error programing the internal"); 
        }else{
          assert(false && "There is not internal if there are no message to send && state != CALL_REQUESTED && state != COMMUNICATION_PARTIALLY_STABLISHED"); 
        }    
      }
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
      if(!state.networkOutMsg.empty() && !state.speakerOutMsg.empty()){
        cout << "WARNING. The phone is getting new messages before sending the previous" << endl;
      } 
      if(state.state == PhoneState::CALL_REQUESTED || state.state == PhoneState::COMMUNICATION_PARTIALLY_STABLISHED){
        state.remainingTimeRinging -= e;
      }else{
        state.remainingTimeRinging = timeRinging;
      }
      //setStateIn Port
      if(!get_messages<typename defs::setStateIn>(mbs).empty()){
        if(get_messages<typename defs::setStateIn>(mbs).size() > 1) throw std::logic_error("Only one message at a time allowed");
        else{
          for(const auto &x : get_messages<typename defs::setStateIn>(mbs)){
            if(state.state == PhoneState::OUT_OF_ORDER && x.outOfOrder == false){
              state.state = PhoneState::IDLE;
            }else if(state.state != PhoneState::OUT_OF_ORDER && x.outOfOrder == true){
              state.state = PhoneState::OUT_OF_ORDER;                
              if(!state.speakerOutMsg.empty()){
                state.speakerOutMsg.clear();
                state.speakerOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::OUT_OF_ORDER, id, id));
              }
              if(!state.networkOutMsg.empty()){
                for(int i = 0; i < state.networkOutMsg.size(); i++){
                  state.networkOutMsg[i].msg_phone = PhoneMsgType::CALL_OVER;
                }
              }
            }
          }
        }
      }
      //microphoneIn
      for (const auto &x : get_messages<typename defs::microphoneIn>(mbs)){
        switch(state.state){
          case PhoneState::IDLE:
            if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
              state.state = PhoneState::CALL_REQUESTED;
              state.busyWithRequest.push_back(x.to);
              state.networkOutMsg.emplace_back(x);        
            }else if(x.msg_phone == PhoneMsgType::CALL_OVER){
              //The call is already over
            }else{
              throw std::logic_error("Message not allowed in IDLE state. External. MicrophoneIn port");
            }
            break;
          case PhoneState::CALL_REQUESTED:
            if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
              bool alreadyrequested = false;
              for(int i=0; i<state.busyWithRequest.size();i++){
                if (state.busyWithRequest[i] == x.to){
                 alreadyrequested = true;
                 break;
                }                         
              }
              if(alreadyrequested == false){
                state.busyWithRequest.push_back(x.to);
                state.networkOutMsg.emplace_back(x);
              }            
            }else if(x.msg_phone == PhoneMsgType::CALL_OVER){
              for(int i=0; i<state.busyWithRequest.size();i++){
                if (state.busyWithRequest[i] == x.to){
                  state.busyWithRequest.erase(state.busyWithRequest.begin()+i);
                  state.networkOutMsg.emplace_back(x);
                  break;
                }                         
              }
              if(state.busyWithRequest.empty()) state.state = PhoneState::IDLE;
            }else{
              throw std::logic_error("Message not allowed in CALL_REQUESTED state. External. MicrophoneIn port");
            }
            break;
          case PhoneState::INCOMING_CALL_REQUESTED:
            state.networkOutMsg.emplace_back(x);
            if (x.msg_phone == PhoneMsgType::CALL_ACCEPTED){
              state.state = PhoneState::COMMUNICATION_FULLY_STABLISHED;
              if(state.busyWithRequest.size()<=1) state.busyWithRequest.clear();
              else throw std::logic_error("INCOMING_CALL_REQUESTED state. More than one incomming call at a time");
              state.busyWithStablished.push_back(x.to);
            } 
            else if(x.msg_phone == PhoneMsgType::CALL_OVER) state.state = PhoneState::IDLE;
            else throw std::logic_error("Message not allowed in INCOMING_CALL_REQUESTED state. External. MicrophoneIn port");
            break;
          case PhoneState::COMMUNICATION_PARTIALLY_STABLISHED:
            if(x.msg_phone == PhoneMsgType::CALL_OVER){
              for(int i=0; i<state.busyWithRequest.size();i++){
                if (state.busyWithRequest[i] == x.to){
                  state.busyWithRequest.erase(state.busyWithRequest.begin()+i);
                  state.networkOutMsg.emplace_back(x);
                  break;
                }                         
              }
              for(int i=0; i<state.busyWithStablished.size();i++){
                if(state.busyWithStablished[i] == x.to){
                  state.busyWithStablished.erase(state.busyWithStablished.begin()+i);
                  state.networkOutMsg.emplace_back(x);
                  break;
                }                         
              }
              if(state.busyWithStablished.empty() && state.busyWithRequest.empty()) assert(false && "Error programing the COMMUNICATION_PARTIALLY_STABLISHED state");
              else if(state.busyWithRequest.empty()) state.state = PhoneState::COMMUNICATION_FULLY_STABLISHED;
              else if(state.busyWithStablished.empty()) state.state = PhoneState::CALL_REQUESTED;
            }else{
              throw std::logic_error("Message not allowed in COMMUNICATION_PARTIALLY_STABLISHED state. External. MicrophoneIn port");
            }
            break;
          case PhoneState::COMMUNICATION_FULLY_STABLISHED:
          case PhoneState::COMMAND_TRANSMITTED:
            if(x.msg_phone == PhoneMsgType::CALL_OVER){
              for(int i=0; i<state.busyWithStablished.size();i++){
                if(state.busyWithStablished[i] == x.to){
                  state.busyWithStablished.erase(state.busyWithStablished.begin()+i);
                  state.networkOutMsg.emplace_back(x);
                  break;
                }                         
              }
              if(state.busyWithStablished.empty()) state.state = PhoneState::IDLE;
            }else if(x.msg_phone == PhoneMsgType::COMMAND_TRANSMITTED){
              bool correct = false;
              for(int i=0; i<state.busyWithStablished.size();i++){
                if(state.busyWithStablished[i] == x.to){
                  correct = true;
                  state.networkOutMsg.emplace_back(x);
                  state.state = PhoneState::COMMAND_TRANSMITTED;
                  break;
                }
              }
              if(correct == false) assert(false && "sending a command when communication not stablished");
            }else{
              throw std::logic_error("Message not allowed in COMMUNICATION_FULLY_STABLISHED state. External. MicrophoneIn port");
            }
            break;
          case PhoneState::COMMUNICATION_OVER:
            //cout << "EXTERNAL PHONE MULTIPLE. COMMUNICATION_OVER" << x << endl;
            (x.msg_phone == PhoneMsgType::CALL_OVER) ? (state.state = PhoneState::IDLE) : throw std::logic_error("Message not allowed in COMMUNICATION_OVER state.External. MicrophoneIn port");
            break;
          case PhoneState::OUT_OF_ORDER:
            state.speakerOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::OUT_OF_ORDER,id,id));
            if(x.msg_phone != PhoneMsgType::CALL_REQUEST){
              state.networkOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::CALL_OVER,id,x.to));
            }
            state.busyWithRequest.clear();
            state.busyWithStablished.clear();
            break;         
          default: assert(false && "Not that state"); break;
        }   
      }
      //network In port         
      for (const auto &x : get_messages<typename defs::networkIn>(mbs)){ 
        switch(state.state){
          case PhoneState::IDLE:
            if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
              state.state = PhoneState::INCOMING_CALL_REQUESTED;
              state.busyWithRequest.push_back(x.from);
              state.speakerOutMsg.emplace_back(x);        
            }else if(x.msg_phone == PhoneMsgType::CALL_OVER || x.msg_phone == PhoneMsgType::NET_OUT_OF_ORDER){
              //The call is already over
            }else{
              throw std::logic_error("Message not allowed in IDLE state. External. NetworkIn port");
            }
            break;
          case PhoneState::CALL_REQUESTED:
            if(x.msg_phone == PhoneMsgType::CALL_ACCEPTED){
              bool correct = false;
              for(int i=0; i<state.busyWithRequest.size();i++){
                if (state.busyWithRequest[i] == x.from) {
                  correct = true;
                  state.busyWithRequest.erase(state.busyWithRequest.begin()+i);
                  state.busyWithStablished.push_back(x.from);
                  state.speakerOutMsg.emplace_back(x);
                  if(state.busyWithRequest.empty()) state.state = PhoneState::COMMUNICATION_FULLY_STABLISHED;
                  else if(!state.busyWithStablished.empty()) state.state = PhoneState::COMMUNICATION_PARTIALLY_STABLISHED;
                  break;
                }                  
              }
              if(correct == false) assert(false && "receiving call_accepted from someone I am not calling");                        
            }else if(x.msg_phone == PhoneMsgType::CALL_OVER || x.msg_phone == PhoneMsgType::NET_OUT_OF_ORDER){
              for(int i=0; i<state.busyWithRequest.size();i++){
                if (state.busyWithRequest[i] == x.from){
                  state.busyWithRequest.erase(state.busyWithRequest.begin()+i);
                  state.speakerOutMsg.emplace_back(x);
                  break;
                }                         
              }
              if(state.busyWithRequest.empty()) state.state = PhoneState::COMMUNICATION_OVER;
            }else if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
              state.networkOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::CALL_OVER,id,x.from)); 
            }else{
              throw std::logic_error("Message not allowed in CALL_REQUESTED state. External. NetworkIn port");
            }
            break;
          case PhoneState::INCOMING_CALL_REQUESTED:                    
            if(x.msg_phone == PhoneMsgType::CALL_OVER || x.msg_phone == PhoneMsgType::NET_OUT_OF_ORDER){
              state.state = PhoneState::IDLE;
              state.speakerOutMsg.emplace_back(x);
              if(state.busyWithRequest.size()<=1) state.busyWithRequest.clear();
              else throw std::logic_error("INCOMING_CALL_REQUESTED state. More than one incomming call at a time");
            }else if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
              state.networkOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::CALL_OVER,id,x.from)); 
            }else throw std::logic_error("Message not allowed in INCOMING_CALL_REQUESTED state. External. NetworkIn port");
            break;
          case PhoneState::COMMUNICATION_PARTIALLY_STABLISHED:
            if(x.msg_phone == PhoneMsgType::CALL_ACCEPTED){
              bool correct = false;
              for(int i=0; i<state.busyWithRequest.size();i++){
                if (state.busyWithRequest[i] == x.from) {
                  correct = true;
                  state.busyWithRequest.erase(state.busyWithRequest.begin()+i);
                  state.busyWithStablished.push_back(x.from);
                  state.speakerOutMsg.emplace_back(x);
                  break;
                }                  
              }
              if(correct == false) assert(false && "receiving call_accepted from someone I am not calling");  
            }else if(x.msg_phone == PhoneMsgType::CALL_OVER || x.msg_phone == PhoneMsgType::NET_OUT_OF_ORDER){
              for(int i=0; i<state.busyWithRequest.size();i++){
                if (state.busyWithRequest[i] == x.from){
                  state.busyWithRequest.erase(state.busyWithRequest.begin()+i);
                  state.speakerOutMsg.emplace_back(x);
                  break;
                }                         
              }
              for(int i=0; i<state.busyWithStablished.size();i++){
                if(state.busyWithStablished[i] == x.from){
                  state.busyWithStablished.erase(state.busyWithStablished.begin()+i);
                  state.speakerOutMsg.emplace_back(x);
                  break;
                }                         
              }                      
            }else if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
              state.networkOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::CALL_OVER,id,x.from)); 
            }else{
              throw std::logic_error("Message not allowed in COMMUNICATION_PARTIALLY_STABLISHED state. External. MicrophoneIn port");
            }
            if(state.busyWithStablished.empty() && state.busyWithRequest.empty()) assert(false && "Error programing the COMMUNICATION_PARTIALLY_STABLISHED state");
            else if(state.busyWithRequest.empty()) state.state = PhoneState::COMMUNICATION_FULLY_STABLISHED;
            else if(state.busyWithStablished.empty()) state.state = PhoneState::CALL_REQUESTED;
            break;
          case PhoneState::COMMUNICATION_FULLY_STABLISHED:
          case PhoneState::COMMAND_TRANSMITTED:
            if(x.msg_phone == PhoneMsgType::CALL_OVER || x.msg_phone == PhoneMsgType::NET_OUT_OF_ORDER){
              for(int i=0; i<state.busyWithStablished.size();i++){
                if(state.busyWithStablished[i] == x.from){
                  state.busyWithStablished.erase(state.busyWithStablished.begin()+i);
                  state.speakerOutMsg.emplace_back(x);
                  break;
                }                         
              }
              if(state.busyWithStablished.empty()) state.state = PhoneState::COMMUNICATION_OVER;
            }else if(x.msg_phone == PhoneMsgType::COMMAND_TRANSMITTED){
              bool correct = false;
              for(int i=0; i<state.busyWithStablished.size();i++){
                if(state.busyWithStablished[i] == x.from){
                  correct = true;
                  state.speakerOutMsg.emplace_back(x);
                  state.state = PhoneState::COMMAND_TRANSMITTED;
                  break;
                }
              }
              if(correct == false) assert(false && "sending a command when communication not stablished");
            }else if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
              state.networkOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::CALL_OVER,id,x.from)); 
            }else{
              throw std::logic_error("Message not allowed in COMMUNICATION_FULLY_STABLISHED state. External. NetworkIn port");
            }
            break;
          case PhoneState::COMMUNICATION_OVER:
            if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
              state.networkOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::CALL_OVER,id,x.from)); 
            }else if (x.msg_phone != PhoneMsgType::CALL_OVER) throw std::logic_error("Message not allowed in COMMUNICATION_OVER state.External. NetworkIn port");
            break;
          case PhoneState::OUT_OF_ORDER:
            state.networkOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::CALL_OVER,id,x.from));                    
            if(x.msg_phone != PhoneMsgType::CALL_REQUEST){
              state.speakerOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::OUT_OF_ORDER,id,id));
            }
            state.busyWithRequest.clear();
            state.busyWithStablished.clear();
            break;         
          default: assert(false && "Not that state"); break;
        }   
      }     
      //defining advance time after the external
      if(!state.speakerOutMsg.empty() || !state.networkOutMsg.empty()){
        state.advanceTime = timeProcessing;
      }else{
        if(state.state == PhoneState::CALL_REQUESTED || state.state == PhoneState::COMMUNICATION_PARTIALLY_STABLISHED) state.advanceTime = state.remainingTimeRinging;
        else state.advanceTime = std::numeric_limits<TIME>::infinity();
      }
      //cleaning busy with if phone state idle
      if(state.state == PhoneState::IDLE){
        state.busyWithStablished.clear();
        state.busyWithRequest.clear();
      }
    }
    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
      internal_transition();
      external_transition(TIME(), std::move(mbs));
    }
    // output function
    typename make_message_bags<output_ports>::type output() const {
      typename make_message_bags<output_ports>::type bags;
      for (int i = 0; i < state.networkOutMsg.size(); i++){
        get_messages<typename defs::networkOut>(bags).push_back(state.networkOutMsg[i]);      
      }   
      for (int i = 0; i < state.speakerOutMsg.size(); i++){
        get_messages<typename defs::speakerOut>(bags).push_back(state.speakerOutMsg[i]);      
      }
      return bags;
    }
    // time_advance function
    TIME time_advance() const {
      return state.advanceTime;
    }
    // << operator for the state
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename DevicePhoneMultiple<TIME>::state_type& i) {
      os << "next time: " << i.advanceTime << " busyWithRequestSize: " << i.busyWithRequest.size() << " busyWithStablishedSize: " << i.busyWithStablished.size() << " networkOutMsg: " << i.networkOutMsg.size() << " speakerOutMsg: " << i.speakerOutMsg.size(); 
      return os;
    }
};
#endif //CADMIUM_DEVICE_PHONE_MULTIPLE_HPP

