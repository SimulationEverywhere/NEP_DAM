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
 
#ifndef CADMIUM_CALL_BY_MULTIPLEPHONE_TASK_HPP
#define CADMIUM_CALL_BY_MULTIPLEPHONE_TASK_HPP

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
  struct callByMultiplePhoneTask_defs{
    struct toMicrophone : public out_port<Communication> {
    };
    struct taskFinisedAcknow : public out_port<TaskDeviceFinished> {
    };
    struct fromSpeaker : public in_port<Communication> {
    };
    struct task : public in_port<CommandWithDevice> {
    };
  };
//This is a meta-model, it should be overloaded for declaring the Id, timeSend2Device, timeWritting and timeUnable2Send parameters
  template<typename TIME>
  class callByMultiplePhoneTask {
    using defs=callByMultiplePhoneTask_defs; // putting definitions in context
    public:
      // state definition
      enum class TaskState {IDLE, PICKING_PHONE, WAITING_ANSWER, COMMUNICATION_STABLISHED_PARTIAL, COMMUNICATION_STABLISHED, COMMAND_TRANSMITTED, COMMUNICATION_OVER};
      //Parameters to be overwriten when instantiating the atomic model
      DeviceId        id;
      TIME            timeWaitingPartial; 
      TIME            timeTransmitCommand;
      TIME            timeHangUpCommandTransmited; 
      TIME            timeHangUpCallOver;  
      // default constructor
      callByMultiplePhoneTask(DeviceId id_param, TIME timeWaitingPartial_param ,TIME timeTransmitCommand_param, TIME timeHangUpCommandTransmited_param, TIME timeHangUpCallOver_param) noexcept {
        id = id_param;
        timeWaitingPartial = timeWaitingPartial_param;
        timeTransmitCommand = timeHangUpCommandTransmited_param;
        timeHangUpCommandTransmited = timeHangUpCommandTransmited_param;
        timeHangUpCallOver = timeHangUpCallOver_param;
        state.state = TaskState::IDLE;
        state.advanceTime = std::numeric_limits<TIME>::infinity(); 
      }
      
      struct state_type{
        vector <TaskDeviceFinished>  taskFinishedMsg;
        vector <Communication>       toMicrophoneMsg;
        TaskState          state;
        Command            command2Transmit;
        vector<DeviceId>   command2Whom;
        vector<DeviceId>   callAcepted;
        vector<DeviceId>   callNotAcepted;
        TIME               advanceTime;
      };
      state_type state;
      // ports definition
      using input_ports=std::tuple<typename defs::task, typename defs::fromSpeaker>;
      using output_ports=std::tuple<typename defs::toMicrophone, typename defs::taskFinisedAcknow>;

      // internal transition
   void internal_transition() {
    switch(state.state){
      case TaskState::IDLE: assert(false && "Internal cannot happen in IDLE"); break;
      case TaskState::PICKING_PHONE:
        state.state = TaskState::WAITING_ANSWER;
        state.taskFinishedMsg.clear();
        state.toMicrophoneMsg.clear();
        break;
      case TaskState::WAITING_ANSWER: assert(false && "Internal cannot happen in WAITING_ANSWER"); break;
      case TaskState::COMMUNICATION_STABLISHED_PARTIAL:
        for(int i = 0; state.toMicrophoneMsg.size(); i++){
          if(state.toMicrophoneMsg[i].msg_phone == PhoneMsgType::CALL_OVER) state.callNotAcepted.push_back(state.toMicrophoneMsg[i].to);
          else assert(false && "Internal. COMMUNICATION_STABLISHED_PARTIAL. The wrong message has been sent");    
        }
        state.taskFinishedMsg.clear();
        state.toMicrophoneMsg.clear();
        if(!(state.command2Whom.size() == (state.callAcepted.size()+state.callNotAcepted.size()))) assert(false && "Internal. COMMUNICATION_STABLISHED_PARTIAL. Bug programming the model");   
        for(int i = 0; i < state.callAcepted.size(); i++){
          state.toMicrophoneMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::COMMAND_TRANSMITTED, id, state.callAcepted[i], state.command2Transmit));
        }        
        state.state = TaskState::COMMUNICATION_STABLISHED;
        break;
      case TaskState::COMMUNICATION_STABLISHED:
        state.state = TaskState::COMMAND_TRANSMITTED;
        for(int i = 0; i < state.toMicrophoneMsg.size(); i++){
          state.toMicrophoneMsg[i].msg_phone = PhoneMsgType::CALL_OVER;
        }        
        for(int i=0; i<state.callAcepted.size(); i++){
          state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, state.callAcepted[i], false, state.command2Transmit.from, state.callAcepted[i].id, state.command2Transmit.command));
        }
        for(int i=0; i<state.callNotAcepted.size(); i++){
          state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, state.callNotAcepted[i], false, state.command2Transmit.from, state.callNotAcepted[i].id, string("-")));
        }          
        break;
      case TaskState::COMMAND_TRANSMITTED:
      case TaskState::COMMUNICATION_OVER:
        state.state = TaskState::IDLE;
        state.taskFinishedMsg.clear();
        state.toMicrophoneMsg.clear();
        state.callAcepted.clear();
        state.callNotAcepted.clear();
        state.command2Whom.clear();
        state.command2Transmit.clear();
        break;
      default: assert(false && "Internal. The state does not exists");break;
    }
    switch(state.state){
      case TaskState::IDLE: state.advanceTime = std::numeric_limits<TIME>::infinity();  break;
      case TaskState::PICKING_PHONE: assert(false && "not reachable from internal"); break;
      case TaskState::WAITING_ANSWER: state.advanceTime = std::numeric_limits<TIME>::infinity();  break;
      case TaskState::COMMUNICATION_STABLISHED_PARTIAL: assert(false && "not reachable from internal"); break;
      case TaskState::COMMUNICATION_STABLISHED: state.advanceTime = timeTransmitCommand; break;
      case TaskState::COMMAND_TRANSMITTED: state.advanceTime = timeHangUpCommandTransmited; break;
      case TaskState::COMMUNICATION_OVER: assert(false && "not reachable from internal"); break;
      default: assert(false && "Seting time advance internal. The state does not exists");break;
    }     
  }

  void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
    TaskState aux_previosState = state.state;
    //from Speaker
    for (const auto &x : get_messages<typename defs::fromSpeaker>(mbs)) {
        switch(state.state){
          case TaskState::IDLE: assert(false && "External. No message allow in IDLE from fromSpeaker port");break;
          case TaskState::PICKING_PHONE: assert(false && "External. No message allow in PICKING_PHONE from fromSpeaker port");break;
          case TaskState::WAITING_ANSWER:
          case TaskState::COMMUNICATION_STABLISHED_PARTIAL:
            switch(x.msg_phone){
              case PhoneMsgType::CALL_ACCEPTED: state.callAcepted.push_back(x.from); break;
              case PhoneMsgType::CALL_OVER:
                for(int i = 0; i < state.command2Whom.size(); i++){
                  if(state.command2Whom[i] == x.from){
                    state.callNotAcepted.push_back(x.from);
                    break;
                  }
                }
                for(int i = 0; i < state.callAcepted.size(); i++){
                  if(state.callAcepted[i] == x.from){
                    state.callAcepted.erase(state.callAcepted.begin()+i);
                    state.callNotAcepted.push_back(x.from);
                    break;
                  }
                }        
                break;
              case PhoneMsgType::OUT_OF_ORDER:
                state.state = TaskState::COMMUNICATION_OVER;
                for(int i =0; i < state.command2Whom.size(); i++){
                  state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, x.from, true, x.from.id, state.command2Whom[i].id,string("-")));
                }
                state.command2Whom.clear();
                state.taskFinishedMsg.clear();
                state.toMicrophoneMsg.clear();    
                for (int i = 0; i < state.callAcepted.size(); i++){
                  state.toMicrophoneMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id, state.callAcepted[i]));
                }
                state.callAcepted.clear();
                state.callNotAcepted.clear();
                break;
              default: assert(false && "External. This message type is not accepted in WAITING_ANSWER/COMMUNICATION_STABLISHED_PARTIAL state");break;
            }
            if(x.msg_phone != PhoneMsgType::OUT_OF_ORDER) {
              state.toMicrophoneMsg.clear();
              state.taskFinishedMsg.clear();
              if (state.command2Whom.size() == (state.callAcepted.size() + state.callNotAcepted.size()) &&
                  !state.callAcepted.empty()) { //All call have being either acepted or not. At least one call accepted
                state.state = TaskState::COMMUNICATION_STABLISHED;
                for (int i = 0; i < state.callAcepted.size(); i++) {
                  state.toMicrophoneMsg.emplace_back(
                          Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::COMMAND_TRANSMITTED, id,
                                        state.callAcepted[i], state.command2Transmit));
                }
              } else if (!state.callAcepted.empty()) { //Pending responses but at least one call acepted
                //Generate the requestes to Hang up the call that have not been acepted
                state.state = TaskState::COMMUNICATION_STABLISHED_PARTIAL;
                for (int i = 0; i < state.command2Whom.size(); i++) {
                  bool accepted = false;
                  for (int j = 0; j < state.callAcepted.size(); j++) {
                    if (state.callAcepted[j] == state.command2Whom[i]) {
                      accepted = true;
                      break;
                    }
                  }
                  if (!accepted) state.toMicrophoneMsg.emplace_back(
                            Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id,
                                          state.command2Whom[i]));
                }
              } else if (state.callAcepted.empty()) { //No call acepted yet
                if (state.command2Whom.size() == state.callNotAcepted.size()) { //Al requests have not been acepted
                  state.state = TaskState::COMMUNICATION_OVER;
                  for (int i = 0; i < state.callNotAcepted.size(); i++) {
                    state.taskFinishedMsg.emplace_back(
                            TaskDeviceFinished(TaskType::SEND, id, state.callNotAcepted[i], false, id.id,
                                               state.callNotAcepted[i].id, string("-")));
                  }
                  state.toMicrophoneMsg.emplace_back(
                          Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id,
                                        DeviceId(id.type, string("multiple"))));
                } else {
                  state.state = TaskState::WAITING_ANSWER;
                }
              }
            }
            break;
          case TaskState::COMMUNICATION_STABLISHED:
            switch(x.msg_phone){
              case PhoneMsgType::CALL_OVER:
                for(int j = 0; j < state.callAcepted.size(); j++){
                  if(state.callAcepted[j] == x.from){
                    state.callAcepted.erase(state.callAcepted.begin()+j);
                    state.callNotAcepted.push_back(x.from);
                    break;
                  }
                }
                if(state.callAcepted.empty()&&(state.command2Whom.size() == state.callNotAcepted.size())){
                  state.state = TaskState::COMMUNICATION_OVER;
                  state.toMicrophoneMsg.clear();
                  state.taskFinishedMsg.clear();
                  for(int i=0; i<state.callNotAcepted.size(); i++){
                    state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, state.callNotAcepted[i], false, id.id, state.callNotAcepted[i].id, string("-")));
                  }      
                  state.toMicrophoneMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id, DeviceId(id.type, string("multiple")))); 
                }            
                break;
              case PhoneMsgType::OUT_OF_ORDER:
                state.state = TaskState::COMMUNICATION_OVER;
                state.taskFinishedMsg.clear();
                state.toMicrophoneMsg.clear();
                for(int i=0; i<state.callNotAcepted.size(); i++){
                  state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, state.callNotAcepted[i], false, id.id, state.callNotAcepted[i].id, string("-")));
                }
                for(int i=0; i<state.callAcepted.size(); i++){
                  state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, state.callAcepted[i], false, id.id, state.callAcepted[i].id, string("-")));
                }        
                for(int i = 0; i<state.callAcepted.size(); i++){
                  state.toMicrophoneMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id, state.callAcepted[i]));
                }
                break;
              default: assert(false && "External. This message type is not accepted in COMMUNICATION_STABLISHED state");break;
            }
            break;
          case TaskState::COMMAND_TRANSMITTED:
            switch(x.msg_phone){
              case PhoneMsgType::CALL_OVER:
                for(int j = 0; j < state.callAcepted.size(); j++){
                  if(state.callAcepted[j] == x.from){
                    state.callAcepted.erase(state.callAcepted.begin()+j);
                    break;
                  }
                }
                if(state.callAcepted.empty()){
                  state.state = TaskState::COMMUNICATION_OVER;
                  state.toMicrophoneMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id, DeviceId(id.type, string("multiple")))); 
                } 
                break;
              case PhoneMsgType::OUT_OF_ORDER:
                state.state = TaskState::COMMUNICATION_OVER;
                for(int k = 0; k<state.taskFinishedMsg.size(); k++){
                  state.taskFinishedMsg[k].out_of_order = true;
                }                
                break;
              default: assert(false && "This message type is not accepted in COMMAND_TRANSMITTED state");break;
            }
            break;
          case TaskState::COMMUNICATION_OVER: assert(false && "External. No message allow in COMMUNICATION_OVER");break;
          default: throw std::logic_error("External. This state does not exists"); break;
        } 
      }
      //task port
      for (const auto &x : get_messages<typename defs::task>(mbs)) {
        if(state.state == TaskState::IDLE){
            state.state = TaskState::PICKING_PHONE;
            state.command2Transmit = x.command;
            state.command2Transmit.to = string("multiple");
            state.command2Whom.push_back(x.to);          
            state.toMicrophoneMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_REQUEST, id, x.to));
        }else if(state.state == TaskState::PICKING_PHONE && state.command2Transmit.command == x.command.command){
            state.command2Whom.push_back(x.to);          
            state.toMicrophoneMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_REQUEST, id, x.to));
        }else assert(false && "External from calltaskport cannot happen"); 
      }   
    
    switch(state.state){
      case TaskState::WAITING_ANSWER: state.advanceTime = state.advanceTime - e;break;
      case TaskState::PICKING_PHONE: state.advanceTime = TIME("00:00:00:001"); break;
      case TaskState::COMMUNICATION_STABLISHED_PARTIAL:
        if(aux_previosState == TaskState::WAITING_ANSWER) state.advanceTime = timeWaitingPartial;
        else if(aux_previosState == TaskState::COMMUNICATION_STABLISHED_PARTIAL) state.advanceTime = state.advanceTime - e;
        else assert(false && "not reachable from external"); 
        break;
      case TaskState::COMMUNICATION_STABLISHED:
        if(aux_previosState == TaskState::WAITING_ANSWER) state.advanceTime = timeTransmitCommand;
        else if(aux_previosState == TaskState::COMMUNICATION_STABLISHED_PARTIAL) state.advanceTime = timeTransmitCommand;
        else if(aux_previosState == TaskState::COMMUNICATION_STABLISHED ) state.advanceTime = state.advanceTime - e;
        else assert(false && "not reachable from external"); 
        break;
      case TaskState::COMMUNICATION_OVER: state.advanceTime = timeHangUpCallOver; break;
      default: assert(false && "Advance. not reachable from external");break;
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
      for (int i = 0; i < (state.toMicrophoneMsg.size()); i++){
        get_messages<typename defs::toMicrophone>(bags).push_back(state.toMicrophoneMsg[i]);        
      }       
      for (int i = 0; i < (state.taskFinishedMsg.size()); i++){
        get_messages<typename defs::taskFinisedAcknow>(bags).push_back(state.taskFinishedMsg[i]);        
      }
      return bags; 
    }

 // time_advance function
  TIME time_advance() const{
    return state.advanceTime;
  }
  //operator << for the state
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename callByMultiplePhoneTask<TIME>::state_type& i) {
        os << " command2Whom: " << i.command2Whom.size() << " command2Transmit: " << i.command2Transmit; 
        return os;
    }
};

#endif // CADMIUM_CALL_BY_MULTIPLEPHONE_TASK_HPP