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

#ifndef CADMIUM_DEVICE_TEXT_OUTBOX_HPP
#define CADMIUM_DEVICE_TEXT_OUTBOX_HPP

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
struct DeviceTextOutbox_defs{
  //input ports
  struct newIn : public in_port<Communication> {};
  struct brokenIn : public in_port<Communication> {};
  struct setStateIn : public in_port<SetDeviceState> {};
  //output Ports
  struct displayOut : public out_port<Communication> {};
  struct networkOut : public out_port<Communication> {};             
};
//MODEL
template<typename TIME>
class DeviceTextOutbox {
  using defs=DeviceTextOutbox_defs; // putting definitions in context
  public:
    //Parameters
    DeviceId id;
    TIME     sendingPeriod;
    TIME     outOfOrderAcknowledgementTime;
    // default constructor
    DeviceTextOutbox(DeviceId id_param, TIME sendingPeriod_param, TIME outOfOrderAcknowledgementTime_param) noexcept {
      id = id_param;
      sendingPeriod = sendingPeriod_param;
      outOfOrderAcknowledgementTime = outOfOrderAcknowledgementTime_param;
      state.outOfOrder = false;
      state.advanceTime = std::numeric_limits<TIME>::infinity();      
    }            
    // state definition
    struct state_type{
      vector <Communication>         DeviceTextOutboxState;
      vector <Communication>         outOfOrderMsg;
      bool                           outOfOrder;
      TIME                           advanceTime;
    };
    state_type state;
    // ports definition
    using input_ports=std::tuple<typename defs::newIn, typename defs::setStateIn, typename defs::brokenIn>;
    using output_ports=std::tuple<typename defs::displayOut, typename defs::networkOut>;
    // internal transition
    void internal_transition() {
      if(state.outOfOrder){
        state.outOfOrderMsg.clear();
        state.advanceTime = std::numeric_limits<TIME>::infinity();
      }else{
        if(!state.DeviceTextOutboxState.empty()){
          state.DeviceTextOutboxState.erase(state.DeviceTextOutboxState.begin());
          (!state.DeviceTextOutboxState.empty()) ? state.advanceTime = sendingPeriod : state.advanceTime = std::numeric_limits<TIME>::infinity();
        }else{
          throw std::logic_error("Error coding, there is not internal if state.DeviceTextOutboxState is empty and the device is working");     
        }
      }    
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        //  cout << "EXTERNAL. DeviceTextOutbox - ";
      //setStateIn Port
      if(!get_messages<typename defs::setStateIn>(mbs).empty()){
        if(get_messages<typename defs::setStateIn>(mbs).size() > 1) throw std::logic_error("Only one message at a time allowed");
        else{
          for(const auto &x : get_messages<typename defs::setStateIn>(mbs)){
            cout << "setStateIn: " << x << " | ";
            state.outOfOrder = x.outOfOrder;
          }
        }
      }
      //newIn Port
      for (const auto &x : get_messages<typename defs::newIn>(mbs)){
        cout << "newIn: " << x << " | ";
        if(state.outOfOrder) state.outOfOrderMsg.push_back(Communication(MessageType::TEXT_MESSAGE, TextMsgType::OUT_OF_ORDER, x.from, x.to));
        else state.DeviceTextOutboxState.push_back(x);    
      }
      //brokenIn Port
      for (const auto &x : get_messages<typename defs::brokenIn>(mbs)){
        //cout << "brokenIn: " << x << " | ";
        Communication aux = Communication(x.type, TextMsgType::NEW, x.to, x.from, x.command);
        state.DeviceTextOutboxState.push_back(aux);    
      }     
     // cout << endl;
      //SetTime Advance
      if(state.outOfOrder){
        if(state.outOfOrderMsg.empty()) state.advanceTime = std::numeric_limits<TIME>::infinity();
        else state.advanceTime = outOfOrderAcknowledgementTime;
      }else{
        if(state.advanceTime != std::numeric_limits<TIME>::infinity()){
          ((state.advanceTime -e)>TIME())? state.advanceTime -= e : state.advanceTime = TIME("00:00:00:001");
        }else{
          (!state.DeviceTextOutboxState.empty()) ? state.advanceTime = sendingPeriod : state.advanceTime = std::numeric_limits<TIME>::infinity();
        }
      }
    }
    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
      internal_transition();
      external_transition(TIME(), std::move(mbs));
    }            // output function
    typename make_message_bags<output_ports>::type output() const {
      typename make_message_bags<output_ports>::type bags;
      //cout << "OUTPUT. DeviceTextOutbox - ";
      if(state.outOfOrder){
        for(int i = 0; i<state.outOfOrderMsg.size();i++){
          get_messages<typename defs::displayOut>(bags).push_back(state.outOfOrderMsg[i]);   
         // cout << "displayOut: " << state.outOfOrderMsg[i] << " | ";   
        }
      }else{
        get_messages<typename defs::networkOut>(bags).push_back(state.DeviceTextOutboxState[0]);
        //cout << "networkOut: " << state.DeviceTextOutboxState[0] << " | ";  
      }    
      //cout << endl;
      return bags;
    }
    // time_advance function
    TIME time_advance() const {
      return state.advanceTime;
    }
    //<< operator for the state
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename DeviceTextOutbox<TIME>::state_type& i) {
      os << " state: " << i.DeviceTextOutboxState.size(); 
      return os;
    }
};
#endif //CADMIUM_DEVICE_TEXT_OUTBOX_HPP