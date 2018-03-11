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


#ifndef CADMIUM_SEND_COMMAND_IN_PERSON_HPP
#define CADMIUM_SEND_COMMAND_IN_PERSON_HPP

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
    struct sendCommandInPerson_defs{
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
    class sendCommandInPerson {
        using defs=sendCommandInPerson_defs; // putting definitions in context
        public:
            enum class TaskState {IDLE, REQUESTING_CONVERSATION, WAITING_ANSWER, COMMUNICATION_STABLISHED, COMMAND_TRANSMITTED, COMMUNICATION_OVER};
            //Parameters to be overwriten when instantiating the atomic model
            DeviceId           id;
            TIME               timeWaitingAnswer;
            TIME               timeTransmitCommand;
            TIME               timeHangUpCommandTransmited; 
            TIME               timeHangUpConversationOver;
            // default constructor
            sendCommandInPerson(DeviceId id_param, TIME timeWaitingAnswer_param, TIME timeTransmitCommand_param, TIME timeHangUpCommandTransmited_param, TIME timeHangUpConversationOver_param) noexcept {
              id =id_param;
              timeWaitingAnswer = timeWaitingAnswer_param;
              timeTransmitCommand = timeTransmitCommand_param;
              timeHangUpCommandTransmited = timeHangUpCommandTransmited_param;
              timeHangUpConversationOver = timeHangUpConversationOver_param;
              state.state = TaskState::IDLE;
            }
            // state definition
            struct state_type{
              vector <TaskDeviceFinished>       taskFinishedMsg;
              vector <Communication>            toPersonMsg;
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
              state.toPersonMsg.clear();
              switch(state.state){
              case TaskState::IDLE: assert(false && "Internal cannot happen in IDLE"); break;
              case TaskState::REQUESTING_CONVERSATION:
                state.state = TaskState::WAITING_ANSWER;
                state.toPersonMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id, DeviceId(DeviceType::IN_PERSON, state.command2Transmit.to)));
                break;
              case TaskState::WAITING_ANSWER:
                  state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, state.toPersonMsg[0].to, false, state.command2Transmit.from, state.command2Transmit.to, string("-")));
                  state.state = TaskState::COMMUNICATION_OVER;
                break;
              case TaskState::COMMUNICATION_STABLISHED:
                state.state = TaskState::COMMAND_TRANSMITTED;
                state.toPersonMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id, DeviceId(DeviceType::IN_PERSON, state.command2Transmit.to)));
                state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, state.toPersonMsg[0].to, false, state.command2Transmit));
                state.command2Transmit.clear();
                break;
              case TaskState::COMMAND_TRANSMITTED: state.state = TaskState::IDLE; break;
              case TaskState::COMMUNICATION_OVER: state.state = TaskState::IDLE; break;
              default: assert(false && "Internal. The state does not exists");break;
              } 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {

               for (const auto &x : get_messages<typename defs::communicationIn>(mbs)) {
                //cout << "EXTERNAL - sendCommandInPerson "<< id << "communicationIn port - " << x << endl;
                  state.taskFinishedMsg.clear();
                  state.toPersonMsg.clear(); 
                  switch(state.state){
                    case TaskState::IDLE: assert(false && "External. No message allow in IDLE from communicationIn");break;
                    case TaskState::REQUESTING_CONVERSATION: assert(false && "External. No message allow in REQUESTING_CONVERSATION from communicationIn");break;
                    case TaskState::WAITING_ANSWER:
                      switch(x.msg_phone){
                        case PhoneMsgType::CALL_ACCEPTED:
                          state.state = TaskState::COMMUNICATION_STABLISHED;
                          state.toPersonMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::COMMAND_TRANSMITTED, id, x.from, state.command2Transmit));
                          break;
                        case PhoneMsgType::CALL_OVER:
                          state.state = TaskState::COMMUNICATION_OVER;
                          state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, x.from, false, x.from.id, x.to.id, string("-")));            
                          break;
                        default: assert(false && "External. This message type is not accepted in REQUESTING_CONVERSATION state");break;
                      }
                      break;
                    case TaskState::COMMUNICATION_STABLISHED:
                      if(x.msg_phone == PhoneMsgType::CALL_OVER) {
                          state.state = TaskState::COMMUNICATION_OVER;
                          state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, x.from, false, x.from.id, x.to.id,string("-")));
                      } else assert(false && "External. This message type is not accepted in COMMUNICATION_STABLISHED state");
                      break;
                    case TaskState::COMMAND_TRANSMITTED:
                      if(x.msg_phone == PhoneMsgType::CALL_OVER){
                        state.state = TaskState::COMMUNICATION_OVER;
                        state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, x.from, false, state.command2Transmit));
                      }else assert(false && "External. This message type is not accepted in COMMAND_TRANSMITTED state");
                      break;  
                    case TaskState::COMMUNICATION_OVER:
                        if(x.msg_phone == PhoneMsgType::CALL_ACCEPTED || x.msg_phone == PhoneMsgType::CALL_OVER ){
                            state.toPersonMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id, DeviceId(DeviceType::IN_PERSON, state.command2Transmit.to)));
                            state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::SEND, id, state.toPersonMsg[0].to, false, state.command2Transmit.from, state.command2Transmit.to, string("-")));
                        }else {
                            assert(false && "External. Message not allow in COMMUNICATION_OVER");
                        }
                          break;
                    default: assert(false && "External. This state does not exists"); break;
                  } 
                }   

              for (const auto &x : get_messages<typename defs::task>(mbs)){
                //cout << "EXTERNAL - sendCommandInPerson "<< id << "task port - " << x << endl;
                if(state.state == TaskState::IDLE){
                  state.state = TaskState::REQUESTING_CONVERSATION;
                  state.command2Transmit = x.command;            
                  state.toPersonMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_REQUEST, id, x.to));
                }else assert(false && "External. Message not allow in this state task port");
              }       
            }

            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
                cout << "********** CONFLUENCE: SEND IN PERSON " << id << endl;
                internal_transition();
                external_transition(TIME(), std::move(mbs));
            }

            // output function
            typename make_message_bags<output_ports>::type output() const {
                typename make_message_bags<output_ports>::type bags;
                
                for (int i = 0; i < (state.taskFinishedMsg.size()); i++){
                  get_messages<typename defs::taskFinisedAcknow>(bags).push_back(state.taskFinishedMsg[i]);
                  //cout << "OUTPUT. sendCommandInPerson "<< id << "taskFinisedAcknow - port" << state.taskFinishedMsg[i] << endl;          
                }
                for (int i = 0; i < (state.toPersonMsg.size()); i++){
                  get_messages<typename defs::communicationOut>(bags).push_back(state.toPersonMsg[i]);
                  //cout << "OUTPUT. sendCommandInPerson "<< id << "communicationOut - port" << state.toPersonMsg[i] << endl;        
                }
                  //cout << "----" << endl;  
                return bags;
            }

            // time_advance function
            TIME time_advance() const {
               TIME next_internal;
                  switch(state.state){
                    case TaskState::IDLE: next_internal = std::numeric_limits<TIME>::infinity(); break;
                    case TaskState::REQUESTING_CONVERSATION: next_internal = TIME("00:00:00:001"); break;
                    case TaskState::WAITING_ANSWER: next_internal = timeWaitingAnswer; break;
                    case TaskState::COMMUNICATION_STABLISHED: next_internal = timeTransmitCommand; break;
                    case TaskState::COMMAND_TRANSMITTED: next_internal = timeHangUpCommandTransmited; break;
                    case TaskState::COMMUNICATION_OVER: next_internal = timeHangUpConversationOver; break;
                    default: assert(false && "Advance. The state does not exists");break;
                  }
                return next_internal;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename sendCommandInPerson<TIME>::state_type& i) {
                os << " taskFinishedMsg:" << i.taskFinishedMsg.size() << "toPersonMsg: " << i.toPersonMsg.size(); 
                return os;
            }
        };    
            
#endif //CADMIUM_SEND_COMMAND_IN_PERSON_HPP