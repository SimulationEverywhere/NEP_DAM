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

#ifndef CADMIUM_SWITCH_FACE2FACE_HPP
#define CADMIUM_SWITCH_FACE2FACE_HPP

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
    struct switchFace2Face_defs{
        struct sendOut : public out_port<Communication> {
        };
        struct answerOut : public out_port<Communication> {
        };
        struct decideOut : public out_port<Communication> {
        };
        struct communicationIn : public in_port<Communication> {
        };
        struct setAnswerIn : public in_port<Communication> {
        };
        struct setDecideIn : public in_port<TaskDeviceFinished> {
        };
        struct setSendIn : public in_port<CommandWithDevice> {
        };
    };
//This is a meta-model, it should be overloaded for declaring the Id parameter
//Communication typename must have a field DeviceId "to"
    template<typename TIME>
    class switchFace2Face {
        using defs=switchFace2Face_defs; // putting definitions in context
        public:
            //Parameters
            DeviceId id;
            // state definition
            enum SwitchState{ANSWER, SEND, DECIDE};
            struct state_type{
                vector <Communication> outCommunication;
                SwitchState  state;
                DeviceId     busyWith;
            }; 
            state_type state;
            // ports definition
            using input_ports=std::tuple<typename defs::setAnswerIn, typename defs::setSendIn, typename defs::setDecideIn, typename defs::communicationIn>;
            using output_ports=std::tuple<typename defs::sendOut, typename defs::answerOut, typename defs::decideOut>;
            // default constructor
            switchFace2Face(DeviceId Id) noexcept {
                    id = Id;
                    state.outCommunication.clear();
                    state.state = SwitchState::DECIDE;
            }
            // internal transition
            void internal_transition() {
                state.outCommunication.clear();
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {      
                int bug = 0;
                for (const auto &x : get_messages<typename defs::setAnswerIn>(mbs)) {
                    state.state = SwitchState::ANSWER;
                    state.busyWith = x.from;
                    bug += 1;
                }
                for (const auto &x : get_messages<typename defs::setSendIn>(mbs)) {
                    state.state = SwitchState::SEND;
                    state.busyWith = x.to;
                    bug += 1;
                } 
                for (const auto &x : get_messages<typename defs::setDecideIn>(mbs)){
                    state.state = SwitchState::DECIDE; 
                    bug += 1;
                }
                for (const auto &x : get_messages<typename defs::communicationIn>(mbs)) {
                    //cout << "EXTERNAL: switch Face2Face. communicationIn: " << x << endl;
                    if(x.to == id) state.outCommunication.push_back(x); //I only take the messages that are for me --> if (x.to == id)
                }        
                //Cannot receive setAnswer, setSend or setDecide at the same time or more than one at the same time                
                if (bug > 1) throw std::logic_error("switch Face2Face - Contradiction to set states");
                       
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
                    case SwitchState::ANSWER:
                        for (int i = 0; i < (state.outCommunication.size()); i++){
                          if(state.busyWith == state.outCommunication[i].from) {
                            get_messages<typename defs::answerOut>(bags).push_back(state.outCommunication[i]);
                            //cout << "OUTPUT: switch in person. answer port: " << state.outCommunication[i] << endl;
                          }       
                          else get_messages<typename defs::decideOut>(bags).push_back(state.outCommunication[i]);
                           
                        }
                    break;
                    case SwitchState::SEND:
                        for (int i = 0; i < (state.outCommunication.size()); i++){
                            if(state.busyWith == state.outCommunication[i].from) {
                                get_messages<typename defs::sendOut>(bags).push_back(state.outCommunication[i]);
                                //cout << "OUTPUT: switch in person. send port: " << state.outCommunication[i] << endl;
                            }
                            else get_messages<typename defs::decideOut>(bags).push_back(state.outCommunication[i]);
                             
                        }
                    break;
                    case SwitchState::DECIDE:
                        for (int i = 0; i < (state.outCommunication.size()); i++){
                          get_messages<typename defs::decideOut>(bags).push_back(state.outCommunication[i]);
                          //cout << "OUTPUT: switch in person. decide port: " << state.outCommunication[i] << endl;
                        }
                    break;
                    default: assert(false && "not that state"); break;
                } 
                return bags;
            }

            // time_advance function
            TIME time_advance() const {
                return (state.outCommunication.empty() ? std::numeric_limits<TIME>::infinity() : TIME("00:00:00:001"));
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename switchFace2Face<TIME>::state_type& i) {
            os << "state: " << i.state << " busyWith:" << i.busyWith; 
          return os;
          }
        };  

        

#endif //CADMIUM_SWITCH_FACE2FACE_HPP