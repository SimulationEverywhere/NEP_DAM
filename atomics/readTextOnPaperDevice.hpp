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
 
#ifndef CADMIUM_READ_TEXT_MSG_ON_PAPER_DEVICE_HPP
#define CADMIUM_READ_TEXT_MSG_ON_PAPER_DEVICE_HPP

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
    struct readTextOnPaperDevice_defs{
        struct toKeyBoard : public out_port<Communication> {
        };
        struct taskFinisedAcknow : public out_port<TaskDeviceFinished> {
        };
        struct fromScreen : public in_port<Communication> {
        };
        struct task : public in_port<Communication> {
        };
    };
//This is a meta-model, it should be overloaded for declaring the Id, timeSend2Device, timeWritting and timeUnable2Send parameters
    template<typename TIME>
    class readTextOnPaperDevice {
        using defs=readTextOnPaperDevice_defs; // putting definitions in context
        public:
            // state definition
            enum class TaskState {IDLE, STARTING_READING, READING};
            //Parameters to be overwriten when instantiating the atomic model
            DeviceId        id;
            TIME            timeStartingReading;
            TIME            timeReading;
            // default constructor
            readTextOnPaperDevice(DeviceId id_param, TIME timeStartingReading_param, TIME timeReading_param) noexcept {
              id = id_param;
              timeReading = timeReading_param;
              timeStartingReading = timeStartingReading_param;
              state.state = TaskState::IDLE;
              state.outOfOrder = false;
            }
            struct state_type{
              TaskState          state;
              bool               outOfOrder;
              Communication      text2read;
              TIME               elapsed; 
            };
            state_type state;
            // ports definition
            using input_ports=std::tuple<typename defs::task, typename defs::fromScreen>;
            using output_ports=std::tuple<typename defs::toKeyBoard, typename defs::taskFinisedAcknow>;

            // internal transition
            void internal_transition() {
              state.elapsed = TIME();
              switch(state.state){
                case TaskState::IDLE: assert(false && "Internal cannot happen in IDLE"); break;
                case TaskState::STARTING_READING: state.state = TaskState::READING; break;
                case TaskState::READING:
                  state.state = TaskState::IDLE;
                  state.outOfOrder = false;
                  break;
                default: assert(false && "Internal. The state does not exists");break;
              } 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              for (const auto &x : get_messages<typename defs::fromScreen>(mbs)) {
                switch(state.state){
                  case TaskState::IDLE: assert(false && "External. No message allow in IDLE from fromScreen port");break;
                  case TaskState::STARTING_READING: assert(false && "External. No message allow in STARTING_READING from fromScreen port");break;
                  case TaskState::READING:
                    if(x.msg_text == TextMsgType::OUT_OF_ORDER) state.outOfOrder=true;
                    else assert(false && "External. Message not allowed in READING from fromScreen port");
                    state.elapsed = e;
                    break;
                  default: throw std::logic_error("External. This state does not exists"); break;
                } 
              }
              for (const auto &x : get_messages<typename defs::task>(mbs)) {
                switch(state.state){
                  case TaskState::IDLE:
                    state.state = TaskState::STARTING_READING;            
                    state.text2read = x;
                    break;
                  case TaskState::STARTING_READING: assert(false && "External from task Port cannot happen in STARTING_READING");  break;
                  case TaskState::READING: assert(false && "External from task Port cannot happen in READING"); break;
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
                Communication aux;
                TaskDeviceFinished finished;
                switch(state.state){
                  case TaskState::IDLE: assert(false && "Out. No output in IDLE");break;
                  case TaskState::STARTING_READING:
                    aux = state.text2read;
                    aux.msg_text = TextMsgType::READ;
                    get_messages<typename defs::toKeyBoard>(bags).push_back(aux);
                    break;
                  case TaskState::READING:
                    finished = TaskDeviceFinished(TaskType::ANSWER, id, state.text2read.from, state.outOfOrder, state.text2read.command);
                    get_messages<typename defs::taskFinisedAcknow>(bags).push_back(finished);
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
                case TaskState::STARTING_READING: next_internal = timeStartingReading; break;
                case TaskState::READING: next_internal = timeReading - state.elapsed; break;
                default: assert(false && "Advance. The state does not exists");break;
              }
              return next_internal;              
            }

             //operator << for the state
            friend std::ostringstream& operator<<(std::ostringstream& os, const typename readTextOnPaperDevice<TIME>::state_type& i) {
                os << " text2read:" << i.text2read; 
                return os;
            }
        };    
            
#endif //CADMIUM_READ_TEXT_MSG_ON_PAPER_DEVICE_HPP