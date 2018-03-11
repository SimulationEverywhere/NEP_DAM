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
 
#ifndef CADMIUM_ANSWER_PERSON_TASK_HPP
#define CADMIUM_ANSWER_PERSON_TASK_HPP

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
    struct answerPersonTask_defs{
        struct toPerson : public out_port<Communication> {
        };
        struct taskFinisedAcknow : public out_port<TaskDeviceFinished> {
        };
        struct fromPerson : public in_port<Communication> {
        };
        struct task : public in_port<Communication> {
        };
    };
//This is a meta-model, it should be overloaded for declaring the Id, timeSend2Device, timeWritting and timeUnable2Send parameters
    template<typename TIME>
    class answerPersonTask {
        using defs=answerPersonTask_defs; // putting definitions in context
        public:
            // state definition
            enum class TaskState {IDLE, REQUEST_CONVERSATION, COMMUNICATION_STABLISHED, COMMAND_RECEIVED, COMMUNICATION_OVER};
            //Parameters to be overwriten when instantiating the atomic model
            DeviceId        id;
            TIME            timeAcceptConversation;  
            TIME            timeConversationOver;
            TIME            timeHangUpCommandReceived;
            // default constructor
            answerPersonTask(DeviceId id_param, TIME timeAcceptConversation_param, TIME timeConversationOver_param, TIME timeHangUpCommandReceived_param) noexcept {
              id = id_param;
              timeAcceptConversation = timeAcceptConversation_param;
              timeConversationOver = timeConversationOver_param;
              timeHangUpCommandReceived = timeHangUpCommandReceived_param;
              state.state = TaskState::IDLE;
              state.taskFinishedMsg.clear();
              state.toPersonMsg.clear();
            }
           
            struct state_type{
              vector <TaskDeviceFinished>  taskFinishedMsg;
              vector <Communication>       toPersonMsg;
              TaskState          state;
            };
            state_type state;
            // ports definition
            using input_ports=std::tuple<typename defs::task, typename defs::fromPerson>;
            using output_ports=std::tuple<typename defs::toPerson, typename defs::taskFinisedAcknow>;

            // internal transition
            void internal_transition() {
              state.taskFinishedMsg.clear();
              state.toPersonMsg.clear();
              switch(state.state){
                case TaskState::IDLE: assert(false && "Internal cannot happen in IDLE"); break;
                case TaskState::REQUEST_CONVERSATION: state.state = TaskState::COMMUNICATION_STABLISHED; break;
                case TaskState::COMMUNICATION_STABLISHED: assert(false && "Internal cannot happen in COMMUNICATION_STABLISHED"); break;
                case TaskState::COMMAND_RECEIVED: state.state = TaskState::IDLE; break;
                case TaskState::COMMUNICATION_OVER: state.state = TaskState::IDLE; break;
                default: assert(false && "Internal. The state does not exists");break;
              } 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              for (const auto &x : get_messages<typename defs::fromPerson>(mbs)) {
                //cout << "EXTERNAL. AnswerPerson" << id << "fromPerson port: " << x << endl;
                state.toPersonMsg.clear();
                switch(state.state){
                  case TaskState::IDLE: assert(false && "External. No message allow in IDLE from fromPerson port");break;
                  case TaskState::REQUEST_CONVERSATION:
                    if (x.msg_phone == PhoneMsgType::CALL_OVER){
                      state.taskFinishedMsg.clear();
                      state.state = TaskState::COMMUNICATION_OVER;
                      state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::ANSWER, id, x.from, false, x.from.id, x.to.id, string("-")));
                    } else{
                        cout << "ANSWER PERSON " << id << "message received ****" << x << "*****" << endl;
                      assert(false && "This message type is not accepted in REQUEST_CONVERSATION state");
                    } 
                    break;  
                  case TaskState::COMMUNICATION_STABLISHED:            
                    state.taskFinishedMsg.clear();
                    switch(x.msg_phone){  
                      case PhoneMsgType::COMMAND_TRANSMITTED:                
                        state.state = TaskState::COMMAND_RECEIVED;
                        state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::ANSWER, id, x.from, false, x.command));
                        state.toPersonMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id, x.from));
                        break;
                      case PhoneMsgType::CALL_OVER:
                        state.state = TaskState::COMMUNICATION_OVER;
                        state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::ANSWER, id, x.from, false, x.from.id, x.to.id, string("-")));         
                        break;
                       default: assert(false && "External. This message type is not accepted in COMMUNICATION_STABLISHED state");break;
                      }
                    break;
                  case TaskState::COMMAND_RECEIVED:
                    state.taskFinishedMsg.clear();
                    if (x.msg_phone == PhoneMsgType::CALL_OVER){
                      state.state = TaskState::COMMUNICATION_OVER;
                      state.taskFinishedMsg.emplace_back(TaskDeviceFinished(TaskType::ANSWER, id, x.from, false, x.command));
                    } else{
                      assert(false && "This message type is not accepted in COMMAND_RECEIVED state");
                    } 
                    break;
                  case TaskState::COMMUNICATION_OVER:
                      if (x.msg_phone != PhoneMsgType::CALL_OVER) {
                        cout << "ANSWER PERSON " << id << "message received ****" << x << "*****" << endl;
                        assert(false && "External. Message allow in COMMUNICATION_OVER");
                      }
                      break;
                  default: throw std::logic_error("External. This state does not exists"); break;
                }
              }
              for (const auto &x : get_messages<typename defs::task>(mbs)) {
                //cout << "EXTERNAL. AnswerPerson" << id << "task port: " << x << endl;
                if(state.state == TaskState::IDLE){
                  state.state = TaskState::REQUEST_CONVERSATION;
                  state.toPersonMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_ACCEPTED, id, x.from));
                }else{
                  assert(false && "External. task Port. No message allow in a state different than IDLE");
                }          
              }
            }

            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              cout << "********** CONFLUENCE: ANSWER IN PERSON " << id << endl;
                internal_transition();
                external_transition(TIME(), std::move(mbs));
            }

            // output function
            typename make_message_bags<output_ports>::type output() const {
                typename make_message_bags<output_ports>::type bags;
                for (int i = 0; i < (state.toPersonMsg.size()); i++){
                  get_messages<typename defs::toPerson>(bags).push_back(state.toPersonMsg[i]);
                  //cout << "OUTPUT. AnswerPerson" << id << "toPerson port: " << state.toPersonMsg[i] << endl;      
                }       
                for (int i = 0; i < (state.taskFinishedMsg.size()); i++){
                  get_messages<typename defs::taskFinisedAcknow>(bags).push_back(state.taskFinishedMsg[i]);
                  //cout << "OUTPUT. AnswerPerson" << id << "taskFinisedAcknow port: " << state.toPersonMsg[i] << endl;         
                }
                //cout << "----" << endl;
                return bags;    
            }

            // time_advance function
            TIME time_advance() const {
              TIME next_internal;
              switch(state.state){
                case TaskState::IDLE: next_internal = std::numeric_limits<TIME>::infinity(); break;
                case TaskState::REQUEST_CONVERSATION: next_internal = timeAcceptConversation; break;
                case TaskState::COMMUNICATION_STABLISHED: next_internal = std::numeric_limits<TIME>::infinity(); break;
                case TaskState::COMMAND_RECEIVED: next_internal = timeHangUpCommandReceived; break;
                case TaskState::COMMUNICATION_OVER: next_internal = timeConversationOver; break;
                default: assert(false && "Advance. The state does not exists");break;
              }
              return next_internal;
            }

          //operator << for the state
          friend std::ostringstream& operator<<(std::ostringstream& os, const typename answerPersonTask<TIME>::state_type& i) {
            os << " taskFinishedMsg: " << i.taskFinishedMsg.size() << " toPersonMsg: " << i.toPersonMsg.size(); 
            return os;
          }
        };    
            
#endif //CADMIUM_ANSWER_PERSON_TASK_HPP 