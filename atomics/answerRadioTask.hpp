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
 
#ifndef CADMIUM_ANSWER_RADIO_TASK_HPP
#define CADMIUM_ANSWER_RADIO_TASK_HPP

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
  struct answerRadioTask_defs{
    struct taskFinisedAcknow : public out_port<TaskDeviceFinished> {
    };
    struct fromSpeaker : public in_port<Communication> {
    };
    struct task : public in_port<Communication> {
    };
  };
//This is a meta-model, it should be overloaded for declaring the Id, timeSend2Device, timeWritting and timeUnable2Send parameters
  template<typename TIME>
  class answerRadioTask {
    using defs=answerRadioTask_defs; // putting definitions in context
    public:
      // state definition
      enum class TaskState {IDLE, WAITING_COMMAND, COMMUNICATION_OVER};      //Parameters to be overwriten when instantiating the atomic model
      DeviceId        id;
      TIME            timeWaitingCommand; 
      TIME            timeHangUpCallOver;  
      // default constructor
      answerRadioTask(DeviceId id_param, TIME timeWaitingCommand_param, TIME timeHangUpCallOver_param) noexcept {
        id = id_param;
        timeWaitingCommand = timeWaitingCommand_param;
        timeHangUpCallOver = timeHangUpCallOver_param;
        state.state = TaskState::IDLE;
      }
      
      struct state_type{
        vector <TaskDeviceFinished>  taskFinishedMsg;
        TaskState          state;
      };
      state_type state;
      // ports definition
      using input_ports=std::tuple<typename defs::task, typename defs::fromSpeaker>;
      using output_ports=std::tuple<typename defs::taskFinisedAcknow>;

      // internal transition
   void internal_transition() {
    state.taskFinishedMsg.clear();
    if(state.state == TaskState::COMMUNICATION_OVER) state.state = TaskState::IDLE;
    else if(state.state == TaskState::WAITING_COMMAND) state.state = TaskState::IDLE;
    else assert(false && "Internal cannot happen");
  }

  void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
    //from Speaker
    for (const auto &x : get_messages<typename defs::fromSpeaker>(mbs)) {
        switch(state.state){
          case TaskState::IDLE: assert(false && "External. No message allow in IDLE from fromSpeaker port");break;
          case TaskState::WAITING_COMMAND:            
            switch(x.msg_radio){                              
              case RadioMsgType::COMMAND_TRANSMITTED:
                state.state = TaskState::COMMUNICATION_OVER;                
                state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::ANSWER, id, x.from, false, x.from.id, id.id, x.command.command));            
                break;
              case RadioMsgType::INTERFERENCES:
                state.state = TaskState::COMMUNICATION_OVER;  
                state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::ANSWER, id, x.from, false, x.from.id, id.id, string("-")));       
                break; 
              case RadioMsgType::REQUEST:
                state.state = TaskState::WAITING_COMMAND;  
                break;       
              default: assert(false && "External. This message type is not accepted in WAITING_COMMAND state");break;
            }
            break;
          case TaskState::COMMUNICATION_OVER:
            switch(x.msg_radio){                              
              case RadioMsgType::COMMAND_TRANSMITTED:                
                state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::ANSWER, id, x.from, false, x.from.id, id.id, x.command.command));            
                break;
              case RadioMsgType::INTERFERENCES:
                state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::ANSWER, id, x.from, false, x.from.id, id.id, string("-")));       
                break;
              case RadioMsgType::REQUEST:
                state.state = TaskState::WAITING_COMMAND;  
                break;                         
              default: assert(false && "External. This message type is not accepted in COMMUNICATION_OVER state");break;
            }
            break;
          default: throw std::logic_error("External. This state does not exists"); break;
        } 
      }
    //task port
    for (const auto &x : get_messages<typename defs::task>(mbs)) {
        if(state.state == TaskState::IDLE) state.state = TaskState::WAITING_COMMAND;
        else assert(false && "No external from answer task if state is not IDLE");
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
    if(!state.taskFinishedMsg.empty()){
      for (int i = 0; i < (state.taskFinishedMsg.size()); i++){
        get_messages<typename defs::taskFinisedAcknow>(bags).push_back(state.taskFinishedMsg[i]);        
      }
    }else{
      get_messages<typename defs::taskFinisedAcknow>(bags).push_back(TaskDeviceFinished(TaskType::ANSWER, id, DeviceId(id.type, string("-")), false, string("-"), string("-"), string("-")));
    }        
    return bags;
  }

  // time_advance function
  TIME time_advance() const{
    TIME next_internal;
    switch(state.state){
      case TaskState::IDLE: next_internal = std::numeric_limits<TIME>::infinity(); break;
      case TaskState::WAITING_COMMAND: next_internal = timeWaitingCommand; break;
      case TaskState::COMMUNICATION_OVER: next_internal = timeHangUpCallOver; break;
      default: assert(false && "Advance. The state does not exists");break;
    }
    return next_internal;
  }
  //operator << for the state
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename answerRadioTask<TIME>::state_type& i) {
        os << " taskFinishedMsg: " << i.taskFinishedMsg.size(); 
        return os;
    }
};

#endif // CADMIUM_ANSWER_RADIO_TASK_HPP