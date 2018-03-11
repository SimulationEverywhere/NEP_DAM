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
 
#ifndef CADMIUM_CALL_BY_PHONE_TASK_HPP
#define CADMIUM_CALL_BY_PHONE_TASK_HPP

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
    struct callByPhoneTask_defs{
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
    class callByPhoneTask {
        using defs=callByPhoneTask_defs; // putting definitions in context
        public:
            // state definition
            enum class TaskState {IDLE, PICKING_PHONE, WAITING_ANSWER, COMMUNICATION_STABLISHED, COMMAND_TRANSMITTED, COMMUNICATION_OVER};
            //Parameters to be overwriten when instantiating the atomic model
            DeviceId        id;
            TIME            timeTransmitCommand;
            TIME            timeHangUpCommandTransmited; 
            TIME            timeHangUpCallOver;  
            // default constructor
            callByPhoneTask(DeviceId id_param, TIME timeTransmitCommand_param, TIME timeHangUpCommandTransmited_param, TIME timeHangUpCallOver_param) noexcept {
              id = id_param;
              timeTransmitCommand = timeHangUpCommandTransmited_param;
              timeHangUpCommandTransmited = timeHangUpCommandTransmited_param;
              timeHangUpCallOver = timeHangUpCallOver_param;
              state.state = TaskState::IDLE;
            }
            
            struct state_type{
              vector <TaskDeviceFinished>       taskFinishedMsg;
              vector <Communication>       toMicrophoneMsg;
              TaskState          state;
              Command            command2Transmit;
              DeviceId           to;
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
                case TaskState::COMMUNICATION_STABLISHED:
                  state.state = TaskState::COMMAND_TRANSMITTED;
                  state.toMicrophoneMsg[0].msg_phone = PhoneMsgType::CALL_OVER;
                  state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, state.toMicrophoneMsg[0].to, false, state.command2Transmit));
                  state.command2Transmit.clear();
                  break;
                case TaskState::COMMAND_TRANSMITTED:
                case TaskState::COMMUNICATION_OVER:
                  state.state = TaskState::IDLE;
                  state.taskFinishedMsg.clear();
                  state.toMicrophoneMsg.clear();
                  break;
                default: assert(false && "Internal. The state does not exists");break;
              } 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              for (const auto &x : get_messages<typename defs::fromSpeaker>(mbs)) {
                switch(state.state){
                  case TaskState::IDLE: assert(false && "External. No message allow in IDLE from fromSpeaker port");break;
                  case TaskState::PICKING_PHONE: assert(false && "External. No message allow in PICKING_PHONE from fromSpeaker port");break;
                  case TaskState::WAITING_ANSWER:
                    switch(x.msg_phone){
                      case PhoneMsgType::CALL_ACCEPTED:
                        state.state = TaskState::COMMUNICATION_STABLISHED;
                        state.toMicrophoneMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::COMMAND_TRANSMITTED, id, x.from, state.command2Transmit));
                      break;
                    case PhoneMsgType::CALL_OVER:
                      state.state = TaskState::COMMUNICATION_OVER;
                      state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, x.from, false, x.from.id, x.to.id, string("-")));
                      state.toMicrophoneMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id, x.from));            
                      break;
                    case PhoneMsgType::OUT_OF_ORDER:
                      state.state = TaskState::COMMUNICATION_OVER;
                      state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, state.to, true, id.id, state.to.id, string("-")));       
                      break;
                    default: assert(false && "External. This message type is not accepted in WAITING_ANSWER state");break;
                    }
                    break;
                  case TaskState::COMMUNICATION_STABLISHED:
                    state.state = TaskState::COMMUNICATION_OVER;
                    state.toMicrophoneMsg.clear();
                    switch(x.msg_phone){
                      case PhoneMsgType::CALL_OVER:                
                        state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, x.from, false, x.from.id, x.to.id, string("-")));         
                        state.toMicrophoneMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id, x.from));            
                        break;
                      case PhoneMsgType::OUT_OF_ORDER:
                        state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, state.to, true, id.id, state.to.id, string("-")));       
                        break;
                      default: assert(false && "External. This message type is not accepted in COMMUNICATION_STABLISHED state");break;
                    }
                    break;
                  case TaskState::COMMAND_TRANSMITTED:
                    state.state = TaskState::COMMUNICATION_OVER;
                    switch(x.msg_phone){
                      case PhoneMsgType::CALL_OVER: break;
                      case PhoneMsgType::OUT_OF_ORDER:                
                        state.taskFinishedMsg[0].out_of_order = true;
                        break;
                      default: assert(false && "This message type is not accepted in COMMAND_TRANSMITTED state");break;
                    }
                    break;
                  case TaskState::COMMUNICATION_OVER: assert(false && "External. No message allow in COMMUNICATION_OVER");break;
                  default: throw std::logic_error("External. This state does not exists"); break;
                } 
              }
              for (const auto &x : get_messages<typename defs::task>(mbs)) {
                if(state.state == TaskState::IDLE){
                  state.state = TaskState::PICKING_PHONE;
                  state.command2Transmit = x.command;
                  state.to = x.to;            
                  state.toMicrophoneMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_REQUEST, id, x.to));
                  state.taskFinishedMsg.clear();
                }else{
                  assert(false && "External. task Port. No message allow in a state different than IDLE");
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
                for (int i = 0; i < (state.toMicrophoneMsg.size()); i++){
                  get_messages<typename defs::toMicrophone>(bags).push_back(state.toMicrophoneMsg[i]);        
                }       
                for (int i = 0; i < (state.taskFinishedMsg.size()); i++){
                  get_messages<typename defs::taskFinisedAcknow>(bags).push_back(state.taskFinishedMsg[i]);        
                }
                return bags;    
            }

            // time_advance function
            TIME time_advance() const {
              TIME next_internal;
              switch(state.state){
                case TaskState::IDLE: next_internal = std::numeric_limits<TIME>::infinity(); break;
                case TaskState::PICKING_PHONE: next_internal = TIME("00:00:00:001"); break;
                case TaskState::WAITING_ANSWER: next_internal = std::numeric_limits<TIME>::infinity(); break;
                case TaskState::COMMUNICATION_STABLISHED: next_internal =timeTransmitCommand; break;
                case TaskState::COMMAND_TRANSMITTED: next_internal = timeHangUpCommandTransmited; break;
                case TaskState::COMMUNICATION_OVER: next_internal = timeHangUpCallOver; break;
                default: assert(false && "Advance. The state does not exists");break;
              }
              return next_internal;
            }
            //operator << for the state
            friend std::ostringstream& operator<<(std::ostringstream& os, const typename callByPhoneTask<TIME>::state_type& i) {
                os << " to:" << i.to << " command2Transmit: " << i.command2Transmit; 
                return os;
            }
        };    
            
#endif //CADMIUM_CALL_BY_PHONE_TASK_HPP 