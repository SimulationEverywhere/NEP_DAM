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


#ifndef CADMIUM_SEND_COMMAND_BY_RADIO_HPP
#define CADMIUM_SEND_COMMAND_BY_RADIO_HPP

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
    struct sendCommandByRadio_defs{
        struct communicationOut : public out_port<Communication> {
        };
        struct taskFinisedAcknow : public out_port<TaskDeviceFinished> {
        };
        struct communicationIn : public in_port<Communication> {
        };
        struct task : public in_port<CommandWithDevice> {
        };
    };
//This is a meta-model, it should be overloaded for declaring the Id parameter
    template<typename TIME>
    class sendCommandByRadio {
        using defs=sendCommandByRadio_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            DeviceId           id;
            TIME               timeTransmitCommand; 
            TIME               timeHangUpCallOver;
            // default constructor
            sendCommandByRadio(DeviceId id_param, TIME timeTransmitCommand_param, TIME timeHangUpCallOver_param) noexcept {
              id = id_param;
              timeTransmitCommand = timeTransmitCommand_param;
              timeHangUpCallOver = timeHangUpCallOver_param;
              state.state = TaskState::IDLE;
            }
            // state definition
            enum class TaskState {IDLE, COMMAND2SEND, CONNECTION_REQUESTED, COMMAND_TRANSMITTED, COMMAND_NOT_TRANSMITTED, COMMUNICATION_OVER};
            struct state_type{
              vector <TaskDeviceFinished>       taskFinishedMsg;
              vector <Communication>            toMicrophoneMsg;
              TaskState                         state;
              Command                           command2Transmit;    
            };

            state_type state;
            // ports definition
            using input_ports=std::tuple<typename defs::communicationIn, typename defs::task>;
            using output_ports=std::tuple<typename defs::communicationOut, typename defs::taskFinisedAcknow>;

            // internal transition
            void internal_transition() {
              state.taskFinishedMsg.clear();
              state.toMicrophoneMsg.clear();
              if(state.state == TaskState::COMMAND2SEND) {
                state.state = TaskState::CONNECTION_REQUESTED;
                state.toMicrophoneMsg.emplace_back(Communication(MessageType::RADIO_MESSAGE, RadioMsgType::COMMAND_TRANSMITTED, id, DeviceId(id.type, "all"), state.command2Transmit));
              }else if(state.state == TaskState::CONNECTION_REQUESTED){
                state.state = TaskState::COMMAND_TRANSMITTED;
                state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, DeviceId(id.type, "multiple"), false, state.command2Transmit)); 
              }else if(state.state == TaskState::COMMAND_TRANSMITTED) state.state = TaskState::IDLE;
              else if(state.state == TaskState::COMMAND_NOT_TRANSMITTED) state.state = TaskState::IDLE;
              else assert(false && "Internal cannot happen");
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              for (const auto &x : get_messages<typename defs::communicationIn>(mbs)) {
                state.taskFinishedMsg.clear();
                state.toMicrophoneMsg.clear();
                switch(state.state){
                  case TaskState::IDLE: assert(false && "External. No message allow in IDLE from communicationIn port");break;
                  case TaskState::COMMAND2SEND:
                    state.state = TaskState::COMMAND_NOT_TRANSMITTED;                    
                    state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, DeviceId(id.type, "multiple"), false, id.id, string("multiple"), string("-")));       
                    break;
                  case TaskState::CONNECTION_REQUESTED:
                    state.state = TaskState::COMMAND_NOT_TRANSMITTED;         
                    switch(x.msg_radio){                              
                      case RadioMsgType::COMMAND_TRANSMITTED:
                      case RadioMsgType::INTERFERENCES:
                      case RadioMsgType::REQUEST:                
                        state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, DeviceId(id.type, "multiple"), false, id.id, string("multiple"), string("-")));       
                        state.toMicrophoneMsg.emplace_back(Communication(MessageType::RADIO_MESSAGE, RadioMsgType::INTERFERENCES, id, DeviceId(id.type, "all"), state.command2Transmit));
                        break;
                      case RadioMsgType::OUT_OF_ORDER:
                        state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, DeviceId(id.type, "multiple"), true, id.id, string("multiple"), string("-")));       
                        break;
                      default: assert(false && "External. This message type is not accepted in CONNECTION_REQUESTED state");break;
                    }
                    break;
                  case TaskState::COMMUNICATION_OVER:
                  case TaskState::COMMAND_NOT_TRANSMITTED:
                  case TaskState::COMMAND_TRANSMITTED:
                    if(x.msg_radio == RadioMsgType::OUT_OF_ORDER){
                      state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, DeviceId(id.type, "multiple"), true, id.id, string("multiple"), string("-")));       
                    }else if(x.msg_radio == RadioMsgType::INTERFERENCES){              
                      state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, DeviceId(id.type, "multiple"), false, id.id, string("multiple"), string("-")));       
                    }
                    break;
                  default: throw std::logic_error("External. This state does not exists"); break;        } 
                }   

              for (const auto &x : get_messages<typename defs::task>(mbs)){
                if(state.state == TaskState::IDLE){
                  state.state = TaskState::COMMAND2SEND;
                  state.command2Transmit = x.command;
                  state.toMicrophoneMsg.emplace_back(Communication(MessageType::RADIO_MESSAGE, RadioMsgType::REQUEST, id, DeviceId(id.type, "all)")));
                } 
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
                for (int i = 0; i < (state.taskFinishedMsg.size()); i++){
                  get_messages<typename defs::taskFinisedAcknow>(bags).push_back(state.taskFinishedMsg[i]);        
                }
                for (int i = 0; i < (state.toMicrophoneMsg.size()); i++){
                  get_messages<typename defs::communicationOut>(bags).push_back(state.toMicrophoneMsg[i]);        
                }
                    
                return bags;
            }

            // time_advance function
            TIME time_advance() const {
               TIME next_internal;
                switch(state.state){
                  case TaskState::IDLE: next_internal = std::numeric_limits<TIME>::infinity(); break;
                  case TaskState::COMMAND2SEND: next_internal=TIME("00:00:00:001"); break;
                  case TaskState::CONNECTION_REQUESTED: next_internal = timeTransmitCommand; break;
                  case TaskState::COMMAND_TRANSMITTED: next_internal = timeHangUpCallOver; break;
                  case TaskState::COMMAND_NOT_TRANSMITTED: next_internal = timeHangUpCallOver; break;
                  default: assert(false && "Advance. The state does not exists");break;
                }
                return next_internal;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename sendCommandByRadio<TIME>::state_type& i) {
                os << " taskFinishedMsg:" << i.taskFinishedMsg.size() << "toMicrophoneMsg: " << i.toMicrophoneMsg.size(); 
                return os;
            }
        };    
            
#endif //CADMIUM_SEND_COMMAND_BY_RADIO_HPP