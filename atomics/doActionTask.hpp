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


#ifndef CADMIUM_DO_ACTION_TASK_HPP
#define CADMIUM_DO_ACTION_TASK_HPP

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
    struct doActionTask_defs{
        struct locationOut : public out_port<PeopleLocation> {
        };
        struct taskFinisedAcknow : public out_port<ActionFinished> {
        };
        struct task : public in_port<Action2Do> {
        };
    };
//This is a meta-model, it should be overloaded for declaring the Id parameter
    template<typename TIME>
    class doActionTask {
        using defs=doActionTask_defs; // putting definitions in context
        public:
            // state definition
            enum class TaskState {INITIAL, IDLE, DOING_ACTION};
            //Parameters to be overwriten when instantiating the atomic model
            string           id;
            string           initialLocation;
            // default constructor
            doActionTask(string id_param, string initialLocation_param) noexcept {
              id = id_param;
              initialLocation = initialLocation_param;
              state.location = initialLocation;
              state.state = TaskState::INITIAL;
            }
             struct state_type{
              TaskState          state;
              string             location;
              Action2Do          actionInProgress;
            };
            state_type state;
            // ports definition
            using input_ports=std::tuple<typename defs::task>;
            using output_ports=std::tuple<typename defs::locationOut, typename defs::taskFinisedAcknow>;

            // internal transition
            void internal_transition() {
              switch(state.state){
                case TaskState::IDLE: assert(false && "Internal cannot happen in IDLE"); break;
                case TaskState::INITIAL: state.state = TaskState::IDLE; break;
                case TaskState::DOING_ACTION:
                  state.location = state.actionInProgress.location;
                  state.state = TaskState::IDLE;
                  break;
                default: assert(false && "Internal. The state does not exists");break;
              } 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              for (const auto &x : get_messages<typename defs::task>(mbs)) {
                switch(state.state){
                  case TaskState::INITIAL: assert(false && "External. No message allow in INITIAL");break;
                  case TaskState::IDLE:
                    state.actionInProgress = x;
                    state.state = TaskState::DOING_ACTION;
                    break;
                  case TaskState::DOING_ACTION: assert(false && "External. No message allow in DOING_ACTION");break;
                  default: assert(false && "The taskState is not implemented"); break;
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
                  case TaskState::IDLE: assert(false && "Output cannot happen in IDLE"); break;
                  case TaskState::INITIAL:
                    get_messages<typename defs::locationOut>(bags).push_back(PeopleLocation(state.location, id));    
                    break;
                  case TaskState::DOING_ACTION:
                    if(state.location != state.actionInProgress.location){
                      get_messages<typename defs::locationOut>(bags).push_back(PeopleLocation(state.actionInProgress.location, id)); 
                    }
                    get_messages<typename defs::taskFinisedAcknow>(bags).push_back(ActionFinished(id, state.actionInProgress.action_id, state.actionInProgress.location));  
                    break;
                  default: assert(false && "Output. The state does not exists");break;
                } 
                return bags;
            }

            // time_advance function
            TIME time_advance() const {
              TIME next_internal;
              switch(state.state){
                case TaskState::IDLE: next_internal = std::numeric_limits<TIME>::infinity(); break;
                case TaskState::INITIAL: next_internal = TIME("00:00:00:001"); break;
                case TaskState::DOING_ACTION: next_internal = TIME(state.actionInProgress.executionTime); break;
                default: assert(false && "Advance. The state does not exists");break;
              }
              return next_internal;
            }
            //operator << for the state
            friend std::ostringstream& operator<<(std::ostringstream& os, const typename doActionTask<TIME>::state_type& i) {
                os << " actionInProgress:" << i.actionInProgress; 
                return os;
            }
        };    
            
#endif //CADMIUM_DO_ACTION_TASK_HPP