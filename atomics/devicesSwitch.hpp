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

#ifndef CADMIUM_DEVICES_SWITCH_HPP
#define CADMIUM_DEVICES_SWITCH_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>

#include "../data_structures/struct_types.hpp"
#include "../data_structures/nep_model_enum_types.hpp"
#include "../data_structures/communication.hpp"

using namespace std;
using namespace cadmium;

using namespace nep_structures;
using namespace nep_model_enum_types;

//Port definition
    struct devicesSwitch_defs{
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
    template<typename TIME>
    class devicesSwitch {
        using defs=devicesSwitch_defs; // putting definitions in context
        public:
            // state definition
            enum SwitchState{ANSWER, SEND, DECIDE};
            struct state_type{
                vector <Communication> outMsg;
                SwitchState stateMOBILEPHONE;
                SwitchState stateLANDLINEPHONE;
                SwitchState statePRIVATELINEPHONE;
                SwitchState stateSATELLITE_PHONE;
                SwitchState stateRADIOLOGICAL_GROUP_DEVICE;
                SwitchState stateBEEPER;
                SwitchState stateRADIO_REMAN;
                SwitchState stateRADIO_REMER;
                SwitchState stateEMAIL;
                SwitchState stateFAX; 
                SwitchState stateTRANKI_GC;
                SwitchState stateTRANKI_E;  
            }; 
            state_type state;
            // default constructor
                //Device type must be overwritten
            devicesSwitch() noexcept {
                state.outMsg.clear();
                state.stateMOBILEPHONE = SwitchState::DECIDE;
                state.stateLANDLINEPHONE = SwitchState::DECIDE;
                state.statePRIVATELINEPHONE = SwitchState::DECIDE;
                state.stateSATELLITE_PHONE = SwitchState::DECIDE;
                state.stateRADIOLOGICAL_GROUP_DEVICE = SwitchState::DECIDE;
                state.stateBEEPER = SwitchState::DECIDE;
                state.stateRADIO_REMAN = SwitchState::DECIDE;
                state.stateRADIO_REMER = SwitchState::DECIDE;
                state.stateEMAIL = SwitchState::DECIDE;
                state.stateFAX = SwitchState::DECIDE;
                state.stateTRANKI_GC = SwitchState::DECIDE;
                state.stateTRANKI_E = SwitchState::DECIDE;
            }
            // ports definition
            using input_ports=std::tuple<typename defs::setAnswerIn, typename defs::setSendIn, typename defs::setDecideIn, typename defs::communicationIn>;
            using output_ports=std::tuple<typename defs::sendOut, typename defs::answerOut, typename defs::decideOut>;
            // internal transition
            void internal_transition() {
                state.outMsg.clear();
            }
            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
                int bugMOBILEPHONE = 0;
                int bugLANDLINEPHONE = 0;
                int bugPRIVATELINEPHONE = 0;
                int bugSATELLITE_PHONE = 0;
                int bugRADIOLOGICAL_GROUP_DEVICE = 0;
                int bugBEEPER = 0;
                int bugRADIO_REMAN = 0;
                int bugRADIO_REMER = 0;
                int bugEMAIL = 0;
                int bugFAX = 0;
                int bugTRANKI_GC = 0;
                int bugTRANKI_E = 0;
                for (const auto &x : get_messages<typename defs::setAnswerIn>(mbs)) {
                    switch(x.to.type){
                        case DeviceType::MOBILEPHONE:
                            state.stateMOBILEPHONE = SwitchState::ANSWER;
                            bugMOBILEPHONE +=1;
                            break;
                        case DeviceType::LANDLINEPHONE:
                            state.stateLANDLINEPHONE = SwitchState::ANSWER;
                            bugLANDLINEPHONE +=1;
                            break;
                        case DeviceType::PRIVATELINEPHONE:
                            state.statePRIVATELINEPHONE = SwitchState::ANSWER;
                            bugPRIVATELINEPHONE +=1;
                            break;
                        case DeviceType::SATELLITE_PHONE:
                            state.stateSATELLITE_PHONE = SwitchState::ANSWER;
                            bugSATELLITE_PHONE +=1;
                            break;
                        case DeviceType::RADIOLOGICAL_GROUP_DEVICE:
                            state.stateRADIOLOGICAL_GROUP_DEVICE = SwitchState::ANSWER;
                            bugRADIOLOGICAL_GROUP_DEVICE +=1;
                            break;
                        case DeviceType::BEEPER:
                            state.stateBEEPER = SwitchState::ANSWER;
                            bugBEEPER +=1;
                            break;
                        case DeviceType::RADIO_REMAN:
                            state.stateRADIO_REMAN = SwitchState::ANSWER;
                            bugRADIO_REMAN +=1;
                            break;
                        case DeviceType::RADIO_REMER:
                            state.stateRADIO_REMER = SwitchState::ANSWER;
                            bugRADIO_REMER +=1;
                            break;
                        case DeviceType::EMAIL:
                            state.stateEMAIL = SwitchState::ANSWER;
                            bugEMAIL +=1;
                            break;
                        case DeviceType::FAX:
                            state.stateFAX = SwitchState::ANSWER;
                            bugFAX +=1;
                            break;
                        case DeviceType::TRANKI_GC:
                            state.stateTRANKI_GC = SwitchState::ANSWER;
                            bugTRANKI_GC +=1;
                            break;
                        case DeviceType::TRANKI_E:
                            state.stateTRANKI_E = SwitchState::ANSWER;
                            bugTRANKI_E +=1;
                            break;
                        default: assert(false && "External. Cannot get a message from that DeviceType");break;
                    }       
                }
                for (const auto &x : get_messages<typename defs::setSendIn>(mbs)) {
                    switch(x.from.type){
                        case DeviceType::MOBILEPHONE:
                            state.stateMOBILEPHONE = SwitchState::SEND;
                            bugMOBILEPHONE +=1;
                            break;
                        case DeviceType::LANDLINEPHONE:
                            state.stateLANDLINEPHONE = SwitchState::SEND;
                            bugLANDLINEPHONE +=1;
                            break;
                        case DeviceType::PRIVATELINEPHONE:
                            state.statePRIVATELINEPHONE = SwitchState::SEND;
                            bugPRIVATELINEPHONE +=1;
                            break;
                        case DeviceType::SATELLITE_PHONE:
                            state.stateSATELLITE_PHONE = SwitchState::SEND;
                            bugSATELLITE_PHONE +=1;
                            break;
                        case DeviceType::RADIOLOGICAL_GROUP_DEVICE:
                            state.stateRADIOLOGICAL_GROUP_DEVICE = SwitchState::SEND;
                            bugRADIOLOGICAL_GROUP_DEVICE +=1;
                            break;
                        case DeviceType::BEEPER:
                            state.stateBEEPER = SwitchState::SEND;
                            bugBEEPER +=1;
                            break;
                        case DeviceType::RADIO_REMAN:
                            state.stateRADIO_REMAN = SwitchState::SEND;
                            bugRADIO_REMAN +=1;
                            break;
                        case DeviceType::RADIO_REMER:
                            state.stateRADIO_REMER = SwitchState::SEND;
                            bugRADIO_REMER +=1;
                            break;
                        case DeviceType::EMAIL:
                            state.stateEMAIL = SwitchState::SEND;
                            bugEMAIL +=1;
                            break;
                        case DeviceType::FAX:
                            state.stateFAX = SwitchState::SEND;
                            bugFAX +=1;
                            break;
                        case DeviceType::TRANKI_GC:
                            state.stateTRANKI_GC = SwitchState::SEND;
                            bugTRANKI_GC +=1;
                            break;
                        case DeviceType::TRANKI_E:
                            state.stateTRANKI_E = SwitchState::SEND;
                            bugTRANKI_E +=1;
                            break;
                        default: assert(false && "External. Cannot get a message from that DeviceType");break;
                    }    
                }
                for (const auto &x : get_messages<typename defs::setDecideIn>(mbs)) {
                    switch(x.my_id.type){
                        case DeviceType::MOBILEPHONE:
                            state.stateMOBILEPHONE = SwitchState::DECIDE;
                            bugMOBILEPHONE +=1;
                            break;
                        case DeviceType::LANDLINEPHONE:
                            state.stateLANDLINEPHONE = SwitchState::DECIDE;
                            bugLANDLINEPHONE +=1;
                            break;
                        case DeviceType::PRIVATELINEPHONE:
                            state.statePRIVATELINEPHONE = SwitchState::DECIDE;
                            bugPRIVATELINEPHONE +=1;
                            break;
                        case DeviceType::SATELLITE_PHONE:
                            state.stateSATELLITE_PHONE = SwitchState::DECIDE;
                            bugSATELLITE_PHONE +=1;
                            break;
                        case DeviceType::RADIOLOGICAL_GROUP_DEVICE:
                            state.stateRADIOLOGICAL_GROUP_DEVICE = SwitchState::DECIDE;
                            bugRADIOLOGICAL_GROUP_DEVICE +=1;
                            break;
                        case DeviceType::BEEPER:
                            state.stateBEEPER = SwitchState::DECIDE;
                            bugBEEPER +=1;
                            break;
                        case DeviceType::RADIO_REMAN:
                            state.stateRADIO_REMAN = SwitchState::DECIDE;
                            bugRADIO_REMAN +=1;
                            break;
                        case DeviceType::RADIO_REMER:
                            state.stateRADIO_REMER = SwitchState::DECIDE;
                            bugRADIO_REMER +=1;
                            break;
                        case DeviceType::EMAIL:
                            state.stateEMAIL = SwitchState::DECIDE;
                            bugEMAIL +=1;
                            break;
                        case DeviceType::FAX:
                            state.stateFAX = SwitchState::DECIDE;
                            bugFAX +=1;
                            break;
                        case DeviceType::TRANKI_GC:
                            state.stateTRANKI_GC = SwitchState::DECIDE;
                            bugTRANKI_GC +=1;
                            break;
                        case DeviceType::TRANKI_E:
                            state.stateTRANKI_E = SwitchState::DECIDE;
                            bugTRANKI_E +=1;
                            break;
                        default: assert(false && "External. Cannot get a message from that DeviceType");break;
                    }    
                }
                for (const auto &x : get_messages<typename defs::communicationIn>(mbs)) {
                    state.outMsg.push_back(x);
                } 
                //Cannot receive setAnswer, setSend or setDecide at the same time                 
                if (bugMOBILEPHONE > 1) throw std::logic_error("devicesSwitch - bugMOBILEPHONE - Contradiction to set states");
                if (bugLANDLINEPHONE > 1) throw std::logic_error("devicesSwitch - bugLANDLINEPHONE - Contradiction to set states");
                if (bugPRIVATELINEPHONE > 1) throw std::logic_error("devicesSwitch - bugPRIVATELINEPHONE - Contradiction to set states");
                if (bugSATELLITE_PHONE > 1) throw std::logic_error("devicesSwitch - bugSATELLITE_PHONE - Contradiction to set states");
                if (bugRADIOLOGICAL_GROUP_DEVICE > 1) throw std::logic_error("devicesSwitch - bugRADIOLOGICAL_GROUP_DEVICE - Contradiction to set states");
                if (bugBEEPER > 1) throw std::logic_error("devicesSwitch - bugBEEPER - Contradiction to set states");
                if (bugRADIO_REMAN > 1) throw std::logic_error("devicesSwitch - bugRADIO_REMAN - Contradiction to set states");
                if (bugRADIO_REMER > 1) throw std::logic_error("devicesSwitch - bugRADIO_REMER - Contradiction to set states");
                if (bugEMAIL > 1) throw std::logic_error("devicesSwitch - bugEMAIL - Contradiction to set states");
                if (bugFAX > 1) throw std::logic_error("devicesSwitch - bugFAX - Contradiction to set states");
                if (bugTRANKI_GC > 1) throw std::logic_error("devicesSwitch - bugTRANKI_GC - Contradiction to set states");
                if (bugTRANKI_E > 1) throw std::logic_error("devicesSwitch - bugTRANKI_E - Contradiction to set states");
            }
            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
                internal_transition();
                external_transition(TIME(), std::move(mbs));
            }
            // output function
            typename make_message_bags<output_ports>::type output() const {
                typename make_message_bags<output_ports>::type bags;
                for(int i = 0; i< state.outMsg.size(); i++){
                    switch(state.outMsg[i].to.type){
                        case DeviceType::MOBILEPHONE:
                            switch(state.stateMOBILEPHONE){
                                case SwitchState::ANSWER: get_messages<typename defs::answerOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::SEND: get_messages<typename defs::sendOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::DECIDE: get_messages<typename defs::decideOut>(bags).push_back(state.outMsg[i]); break;
                                default: assert(false && "not that state"); break;
                            } 
                            break;
                        case DeviceType::LANDLINEPHONE:
                            switch(state.stateLANDLINEPHONE){
                                case SwitchState::ANSWER: get_messages<typename defs::answerOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::SEND: get_messages<typename defs::sendOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::DECIDE: get_messages<typename defs::decideOut>(bags).push_back(state.outMsg[i]); break;
                                default: assert(false && "not that state"); break;
                            } 
                            break;
                        case DeviceType::PRIVATELINEPHONE:
                            switch(state.statePRIVATELINEPHONE){
                                case SwitchState::ANSWER: get_messages<typename defs::answerOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::SEND: get_messages<typename defs::sendOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::DECIDE: get_messages<typename defs::decideOut>(bags).push_back(state.outMsg[i]); break;
                                default: assert(false && "not that state"); break;
                            } 
                            break;
                        case DeviceType::SATELLITE_PHONE:
                           switch(state.stateSATELLITE_PHONE){
                                case SwitchState::ANSWER: get_messages<typename defs::answerOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::SEND: get_messages<typename defs::sendOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::DECIDE: get_messages<typename defs::decideOut>(bags).push_back(state.outMsg[i]); break;
                                default: assert(false && "not that state"); break;
                            } 
                            break;
                        case DeviceType::RADIOLOGICAL_GROUP_DEVICE:
                            switch(state.stateRADIOLOGICAL_GROUP_DEVICE){
                                case SwitchState::ANSWER: get_messages<typename defs::answerOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::SEND: get_messages<typename defs::sendOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::DECIDE: get_messages<typename defs::decideOut>(bags).push_back(state.outMsg[i]); break;
                                default: assert(false && "not that state"); break;
                            } 
                            break;
                        case DeviceType::BEEPER:
                            switch(state.stateBEEPER){
                                case SwitchState::ANSWER: get_messages<typename defs::answerOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::SEND: get_messages<typename defs::sendOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::DECIDE: get_messages<typename defs::decideOut>(bags).push_back(state.outMsg[i]); break;
                                default: assert(false && "not that state"); break;
                            } 
                            break;
                        case DeviceType::RADIO_REMAN:
                            switch(state.stateRADIO_REMAN){
                                case SwitchState::ANSWER: get_messages<typename defs::answerOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::SEND: get_messages<typename defs::sendOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::DECIDE: get_messages<typename defs::decideOut>(bags).push_back(state.outMsg[i]); break;
                                default: assert(false && "not that state"); break;
                            } 
                            break;
                        case DeviceType::RADIO_REMER:
                            switch(state.stateRADIO_REMER){
                                case SwitchState::ANSWER: get_messages<typename defs::answerOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::SEND: get_messages<typename defs::sendOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::DECIDE: get_messages<typename defs::decideOut>(bags).push_back(state.outMsg[i]); break;
                                default: assert(false && "not that state"); break;
                            } 
                            break;
                        case DeviceType::EMAIL:
                           switch(state.stateEMAIL){
                                case SwitchState::ANSWER: get_messages<typename defs::answerOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::SEND: get_messages<typename defs::sendOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::DECIDE: get_messages<typename defs::decideOut>(bags).push_back(state.outMsg[i]); break;
                                default: assert(false && "not that state"); break;
                            } 
                            break;
                        case DeviceType::FAX:
                            switch(state.stateFAX){
                                case SwitchState::ANSWER: get_messages<typename defs::answerOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::SEND: get_messages<typename defs::sendOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::DECIDE: get_messages<typename defs::decideOut>(bags).push_back(state.outMsg[i]); break;
                                default: assert(false && "not that state"); break;
                            } 
                            break;
                        case DeviceType::TRANKI_GC:
                            switch(state.stateTRANKI_GC){
                                case SwitchState::ANSWER: get_messages<typename defs::answerOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::SEND: get_messages<typename defs::sendOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::DECIDE: get_messages<typename defs::decideOut>(bags).push_back(state.outMsg[i]); break;
                                default: assert(false && "not that state"); break;
                            } 
                            break;
                        case DeviceType::TRANKI_E:
                            switch(state.stateTRANKI_E){
                                case SwitchState::ANSWER: get_messages<typename defs::answerOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::SEND: get_messages<typename defs::sendOut>(bags).push_back(state.outMsg[i]); break;
                                case SwitchState::DECIDE: get_messages<typename defs::decideOut>(bags).push_back(state.outMsg[i]); break;
                                default: assert(false && "not that state"); break;
                            } 
                            break;
                        default: assert(false && "External. Cannot get a message from that DeviceType");break;
                    }  
                }
                return bags;
            }
            // time_advance function
            TIME time_advance() const {
                return (state.outMsg.empty() ? std::numeric_limits<TIME>::infinity() : TIME("00:00:00:001"));
            }
            //operator << for the state
            friend std::ostringstream& operator<<(std::ostringstream& os, const typename devicesSwitch<TIME>::state_type& i) {
                os << " outSize:" << i.outMsg.size(); 
                return os;
            }
        };    
            
#endif //CADMIUM_DEVICES_SWITCH_HPP