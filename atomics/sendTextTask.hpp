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
 
#ifndef CADMIUM_SEND_TEXT_HPP
#define CADMIUM_SEND_TEXT_HPP

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
    struct sendTextTask_defs{
        struct toKeyBoard : public out_port<Communication> {
        };
        struct taskFinisedAcknow : public out_port<TaskDeviceFinished> {
        };
        struct fromScreen : public in_port<Communication> {
        };
        struct task : public in_port<CommandWithDevice> {
        };
    };

//This is a meta-model, it should be overloaded for declaring the Id, timeSent2Device, timeWritting and timeUnable2Send parameters
    template<typename TIME>
    class sendTextTask {
        using defs=sendTextTask_defs; // putting definitions in context
        public:
          enum class TaskState {IDLE, WRITTING, SENT2DEVICE, UNABLE2SEND};
            //Parameters to be overwriten when instantiating the atomic model
            DeviceId        id;
            TIME            timeSent2Device;
            TIME            timeWritting;
            TIME            timeUnable2Send;   
            // default constructor
            sendTextTask(DeviceId id_param, TIME timeSent2Device_param, TIME timeWritting_param, TIME timeUnable2Send_param) noexcept {
              id = id_param;
              timeSent2Device = timeSent2Device_param;
              timeWritting = timeWritting_param;
              timeUnable2Send = timeUnable2Send_param;
              state.state = TaskState::IDLE;
            }
            // state definition            
            struct state_type{
              TaskState          state;
              CommandWithDevice  text2send;
            };
            state_type state;
            // ports definition
            using input_ports=std::tuple<typename defs::task, typename defs::fromScreen>;
            using output_ports=std::tuple<typename defs::toKeyBoard, typename defs::taskFinisedAcknow>;

            // internal transition
            void internal_transition() {
              switch(state.state){
                case TaskState::IDLE: assert(false && "Internal cannot happen in IDLE"); break;
                case TaskState::WRITTING: state.state = TaskState::SENT2DEVICE; break;
                case TaskState::SENT2DEVICE: state.state = TaskState::IDLE; break;
                case TaskState::UNABLE2SEND: state.state = TaskState::IDLE; break;
                default: assert(false && "Internal. The state does not exists");break;
              } 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              for (const auto &x : get_messages<typename defs::fromScreen>(mbs)) {
                switch(state.state){
                  case TaskState::IDLE: assert(false && "External. No message allow in IDLE from fromScreen port");break;
                  case TaskState::WRITTING: assert(false && "External. No message allow in WRITTING from fromScreen port");break;
                  case TaskState::SENT2DEVICE:
                    if(x.msg_text != TextMsgType::OUT_OF_ORDER) assert(false && "External. Message not allowed in SENT2DEVICE from fromScreen port");
                    state.state = TaskState::UNABLE2SEND;
                    break;
                  case TaskState::UNABLE2SEND: assert(false && "External. No message allow in UNABLE2SEND from fromScreen port");break;
                  default: throw std::logic_error("External. This state does not exists"); break;
                }
              }
              for (const auto &x : get_messages<typename defs::task>(mbs)) {
                switch(state.state){
                  case TaskState::IDLE:
                    state.state = TaskState::WRITTING;            
                    state.text2send = x;
                    break;
                  case TaskState::WRITTING: assert(false && "External from sendTaskPort cannot happen in WRITTING");  break;
                  case TaskState::SENT2DEVICE: assert(false && "External from sendTaskPort cannot happen in SENT2DEVICE"); break;
                  case TaskState::UNABLE2SEND: assert(false && "External from sendTaskPort cannot happen in UNABLE2SEND");  break;
                  default: assert(false && "External. The state does not exists");break;
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
                switch(state.state){
                    case TaskState::IDLE: assert(false && "Out. No output in IDLE");break;
                    case TaskState::WRITTING:
                      get_messages<typename defs::toKeyBoard>(bags).push_back(Communication(MessageType::TEXT_MESSAGE, TextMsgType::NEW, id, state.text2send.to, state.text2send.command));
                      break;
                    case TaskState::SENT2DEVICE:
                      get_messages<typename defs::taskFinisedAcknow>(bags).push_back(TaskDeviceFinished(TaskType::SEND, id, state.text2send.to, 0, state.text2send.command));
                      break;
                    case TaskState::UNABLE2SEND:
                      get_messages<typename defs::taskFinisedAcknow>(bags).push_back(TaskDeviceFinished(TaskType::SEND, id, state.text2send.to, 1, state.text2send.command.from, state.text2send.command.to, string("-")));
                      break;
                    default: assert(false && "Out. The state does not exists");break;
                }
                return bags;    
            }

            // time_advance function
            TIME time_advance() const {
              TIME next_internal;
              switch(state.state){
                case TaskState::IDLE: next_internal = std::numeric_limits<TIME>::infinity(); break;
                case TaskState::WRITTING: next_internal = timeWritting; break;
                case TaskState::SENT2DEVICE: next_internal = timeSent2Device; break;
                case TaskState::UNABLE2SEND: next_internal = timeUnable2Send; break;
                default: assert(false && "Advance. The state does not exists");break;
              }
              return next_internal;
            }
            //operator << for the state
            friend std::ostringstream& operator<<(std::ostringstream& os, const typename sendTextTask<TIME>::state_type& i) {
                os << " text2send:" << i.text2send; 
                return os;
            }
        };    
            
#endif //CADMIUM_SEND_TEXT_HPP
