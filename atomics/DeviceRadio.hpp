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

#ifndef CADMIUM_DEVICE_RADIO_HPP
#define CADMIUM_DEVICE_RADIO_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>

#include "../data_structures/struct_types.hpp"
#include "../data_structures/communication.hpp"
#include "../data_structures/nep_model_enum_types.hpp"

using namespace std;
using namespace cadmium;

using namespace nep_structures;
using namespace nep_model_enum_types;

//Port definition
struct DeviceRadio_defs{
  //INPUT PORTS
  struct microphoneIn : public in_port<Communication> {};
  struct networkIn : public in_port<Communication> {};
  struct setStateIn : public in_port<SetDeviceState> {};
  //OUTPUT PORTS
  struct speakerOut : public out_port<Communication> {};
  struct networkOut : public out_port<Communication> {};            
};
//MODEL
template<typename TIME>
class DeviceRadio {
  using defs=DeviceRadio_defs; // putting definitions in context
  public:
    enum class RadioState {IDLE, BUSY_START, BUSY, OVER, OUT_OF_ORDER};
    //Parameters to be overwritten when instantiating the atomic
    DeviceId id;
    TIME timeProcessing;
    // default constructor
    DeviceRadio(DeviceId id_param, TIME timeProcessing_param) noexcept {
      id = id_param;
      timeProcessing = timeProcessing_param;
      state.state = RadioState::IDLE;
    }
    // state definition            
    struct state_type{
        vector <Communication>      networkOutMsg;
        vector <Communication>      speakerOutMsg;
        RadioState        state;
        vector<DeviceId>  busyWith;    
    };
    state_type state;
    // ports definition
    using input_ports=std::tuple<typename defs::microphoneIn, typename defs::setStateIn, typename defs::networkIn>;
    using output_ports=std::tuple<typename defs::speakerOut, typename defs::networkOut>;
    // internal transition
    void internal_transition() {
      state.networkOutMsg.clear();
      state.speakerOutMsg.clear();
      switch(state.state){
        case RadioState::IDLE: break;
        case RadioState::BUSY_START: state.state = RadioState::BUSY;break;
        case RadioState::BUSY: break;
        case RadioState::OVER: state.state = RadioState::IDLE; break;
        case RadioState::OUT_OF_ORDER: break;
        default: assert(false && "Not that state");
      }
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
      //setStateIn Port
      if(!get_messages<typename defs::setStateIn>(mbs).empty()){
        if(get_messages<typename defs::setStateIn>(mbs).size() > 1) throw std::logic_error("Only one message at a time allowed");
        else{
          for(const auto &x : get_messages<typename defs::setStateIn>(mbs)){
            state.state = RadioState::OUT_OF_ORDER;
            state.speakerOutMsg.clear();
            if(!state.networkOutMsg.empty()){
              state.speakerOutMsg.emplace_back(Communication(MessageType::RADIO_MESSAGE, RadioMsgType::OUT_OF_ORDER, id, id));
              state.networkOutMsg.clear();
            }
          }
        }
      }
      //MicrophoneIn port
      for (auto &x : get_messages<typename defs::microphoneIn>(mbs)){  //fromMicrophonePort
        switch(state.state){
          case RadioState::IDLE:
          case RadioState::BUSY_START:
            if(x.msg_radio == RadioMsgType::REQUEST){
              state.state = RadioState::BUSY_START;
              state.busyWith.push_back(x.from);
              state.networkOutMsg.emplace_back(x);          
            }else{
              throw std::logic_error("Message not allowed in IDLE/BUSY_START state. External fromMicrophonePort");
            }
            break;
          case RadioState::BUSY:
            if(x.msg_radio == RadioMsgType::REQUEST){
              state.busyWith.push_back(x.from);
              state.networkOutMsg.emplace_back(x);          
            }else if(x.msg_radio == RadioMsgType::COMMAND_TRANSMITTED || x.msg_radio == RadioMsgType::INTERFERENCES){
              if(state.busyWith.size()!=1) x.msg_radio = RadioMsgType::INTERFERENCES;
              state.networkOutMsg.emplace_back(x);              
            }else{
              throw std::logic_error("Message not allowed in BUSY state. External fromMicrophonePort");
            }
            break;
          case RadioState::OVER: 
            if(x.msg_radio == RadioMsgType::REQUEST){
              state.state = RadioState::BUSY_START;
              state.busyWith.push_back(x.from);
              state.networkOutMsg.emplace_back(x);          
            }else{
              throw std::logic_error("Message not allowed in BUSY state. External fromMicrophonePort");
            }
            break;
          case RadioState::OUT_OF_ORDER:
            state.speakerOutMsg.emplace_back(Communication(MessageType::RADIO_MESSAGE, RadioMsgType::OUT_OF_ORDER, id, id));      
            break;
          default: assert(false && "Not that state"); break;
        }            
      }
      //NetworkIn port
      for (auto &x : get_messages<typename defs::networkIn>(mbs)){         
        switch(state.state){
          case RadioState::IDLE:
          case RadioState::BUSY_START:
            if(x.msg_radio == RadioMsgType::REQUEST){
              state.state = RadioState::BUSY_START;
              state.busyWith.push_back(x.from);
              state.speakerOutMsg.emplace_back(x);          
            }else if(x.msg_radio == RadioMsgType::NET_OUT_OF_ORDER){
              state.speakerOutMsg.emplace_back(x);    
            }else{
              throw std::logic_error("Message not allowed in IDLE/BUSY_START state. External networkIn port");
            }
            break;
          case RadioState::BUSY:
            if(x.msg_radio == RadioMsgType::REQUEST){
              state.busyWith.push_back(x.from);
              state.speakerOutMsg.emplace_back(x);          
            }else if(x.msg_radio == RadioMsgType::COMMAND_TRANSMITTED || x.msg_radio == RadioMsgType::INTERFERENCES){
              if(state.busyWith.size()!=1) x.msg_radio = RadioMsgType::INTERFERENCES;
              state.speakerOutMsg.emplace_back(x);              
            }else if(x.msg_radio == RadioMsgType::NET_OUT_OF_ORDER){
              state.speakerOutMsg.emplace_back(x);    
            }else{
              throw std::logic_error("Message not allowed in BUSY state. External networkIn port");
            }
            break;
          case RadioState::OVER: 
            if(x.msg_radio == RadioMsgType::REQUEST){
              state.state = RadioState::BUSY_START;
                state.busyWith.push_back(x.from);
                state.speakerOutMsg.emplace_back(x);          
            }else if(x.msg_radio == RadioMsgType::NET_OUT_OF_ORDER){
              state.speakerOutMsg.emplace_back(x);    
            }else{
              throw std::logic_error("Message not allowed in BUSY state. External netwrokIn port");
            }
            break;
          case RadioState::OUT_OF_ORDER: state.busyWith.clear(); break;
          default: assert(false && "Not that state"); break;
        }       
      }
      //Update busywith  
      for(int i = 0; i < state.networkOutMsg.size(); i++){
        if(state.networkOutMsg[i].msg_radio == RadioMsgType::COMMAND_TRANSMITTED || state.networkOutMsg[i].msg_radio == RadioMsgType::INTERFERENCES){
          for(int j = 0; j < state.busyWith.size(); j++){
            if(state.busyWith[j] == state.networkOutMsg[i].from){
              state.busyWith.erase(state.busyWith.begin()+j);
              break;
            }
          }
        }      
      }
      for(int i = 0; i < state.speakerOutMsg.size(); i++){
        if(state.speakerOutMsg[i].msg_radio == RadioMsgType::COMMAND_TRANSMITTED || state.speakerOutMsg[i].msg_radio == RadioMsgType::INTERFERENCES || state.speakerOutMsg[i].msg_radio == RadioMsgType::NET_OUT_OF_ORDER ){
          for(int j = 0; j < state.busyWith.size(); j++){
            if(state.busyWith[j] == state.speakerOutMsg[i].from){
              state.busyWith.erase(state.busyWith.begin()+j);
              break;
            }
          }
        }      
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
      return ((!state.networkOutMsg.empty() || !state.speakerOutMsg.empty()) ? timeProcessing : std::numeric_limits<TIME>::infinity());
    }
    // << operator for the state
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename DeviceRadio<TIME>::state_type& i) {
      os << " networkOutMsg: " << i.networkOutMsg.size() << " speakerOutMsg:" << i.speakerOutMsg.size(); 
      return os;
    }
};
#endif //CADMIUM_DEVICE_RADIO_HPP