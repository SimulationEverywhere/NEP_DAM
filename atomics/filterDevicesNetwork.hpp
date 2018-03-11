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

#ifndef CADMIUM_FILTER_DEVICES_NETWORK_HPP
#define CADMIUM_FILTER_DEVICES_NETWORK_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>

#include "../data_structures/nep_model_enum_types.hpp"
#include "../data_structures/communication.hpp"
#include "../data_structures/struct_types.hpp"

using namespace cadmium;
using namespace std;
using namespace nep_model_enum_types;
using namespace nep_structures;

//Port definition
    struct filterDevicesNetwork_defs{
        struct in : public in_port<Communication> {
        };
        struct outMOBILEPHONE : public out_port<Communication> {
        };
        struct outLANDLINEPHONE : public out_port<Communication> {
        };
        struct outPRIVATELINEPHONE : public out_port<Communication> {
        };
        struct outSATELLITE_PHONE : public out_port<Communication> {
        };
        struct outRADIOLOGICAL_GROUP_DEVICE : public out_port<Communication> {
        };
        struct outBEEPER : public out_port<Communication> {
        };
        struct outRADIO_REMAN : public out_port<Communication> {
        };
        struct outRADIO_REMER : public out_port<Communication> {
        };
        struct outEMAIL : public out_port<Communication> {
        };
        struct outFAX : public out_port<Communication> {
        }; 
        struct outTRANKI_GC : public out_port<Communication> {
        };
        struct outTRANKI_E : public out_port<Communication> {
        };  
    };

    template<typename TIME>
    class filterDevicesNetwork {
        using defs=filterDevicesNetwork_defs; // putting definitions in context
        public:
            //Model parameters
            string id;           
            // default constructor
            filterDevicesNetwork(string id_param) noexcept {
             id = id_param;
            }
            // state definition
            struct state_type{
                vector<Communication> msgToMOBILEPHONE;
                vector<Communication> msgToLANDLINEPHONE;
                vector<Communication> msgToPRIVATELINEPHONE;
                vector<Communication> msgToSATELLITE_PHONE;
                vector<Communication> msgToRADIOLOGICAL_GROUP_DEVICE;
                vector<Communication> msgToBEEPER;
                vector<Communication> msgToRADIO_REMAN;
                vector<Communication> msgToRADIO_REMER;
                vector<Communication> msgToEMAIL;
                vector<Communication> msgToFAX;
                vector<Communication> msgToTRANKI_GC;
                vector<Communication> msgToTRANKI_E;
                TIME                  advance;
            }; 
            state_type state;
            // ports definition
            using input_ports=std::tuple<typename defs::in>;
            using output_ports=std::tuple<typename defs::outMOBILEPHONE, typename defs::outLANDLINEPHONE, typename defs::outPRIVATELINEPHONE, typename defs::outSATELLITE_PHONE,
                    typename defs::outRADIOLOGICAL_GROUP_DEVICE, typename defs::outBEEPER, typename defs::outRADIO_REMAN, typename defs::outRADIO_REMER,
                    typename defs::outEMAIL, typename defs::outFAX, typename defs::outTRANKI_GC, typename defs::outTRANKI_E>;

            // internal transition
            void internal_transition() {
                state.msgToMOBILEPHONE.clear();
                state.msgToLANDLINEPHONE.clear();
                state.msgToPRIVATELINEPHONE.clear();
                state.msgToSATELLITE_PHONE.clear();
                state.msgToRADIOLOGICAL_GROUP_DEVICE.clear();
                state.msgToBEEPER.clear();
                state.msgToRADIO_REMAN.clear();
                state.msgToRADIO_REMER.clear();
                state.msgToEMAIL.clear();
                state.msgToFAX.clear();
                state.msgToTRANKI_GC.clear();
                state.msgToTRANKI_E.clear();
                state.advance = std::numeric_limits<TIME>::infinity();
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {                       
                for (const auto &x : get_messages<typename defs::in>(mbs)) {
                    switch(x.to.type){
                        case DeviceType::MOBILEPHONE: 
                            if(x.to.id == id) state.msgToMOBILEPHONE.emplace_back(x);
                            break;
                        case DeviceType::LANDLINEPHONE: 
                            if(x.to.id == id) state.msgToLANDLINEPHONE.emplace_back(x);
                            break;
                        case DeviceType::PRIVATELINEPHONE: 
                            if(x.to.id == id) state.msgToPRIVATELINEPHONE.emplace_back(x);
                            break;
                        case DeviceType::SATELLITE_PHONE: 
                            if(x.to.id == id) state.msgToSATELLITE_PHONE.emplace_back(x);
                            break;
                        case DeviceType::RADIOLOGICAL_GROUP_DEVICE: 
                            if(x.to.id == id) state.msgToRADIOLOGICAL_GROUP_DEVICE.emplace_back(x);
                            break;
                        case DeviceType::BEEPER: 
                            if(x.to.id == id) state.msgToBEEPER.emplace_back(x);
                            break;
                        case DeviceType::RADIO_REMAN: 
                            if(x.from.id != id) state.msgToRADIO_REMAN.emplace_back(x);
                            break;
                        case DeviceType::RADIO_REMER: 
                            if(x.from.id != id) state.msgToRADIO_REMER.emplace_back(x);
                            break;
                        case DeviceType::EMAIL: 
                            if(x.to.id == id) state.msgToEMAIL.emplace_back(x);
                            break;
                        case DeviceType::FAX: 
                            if(x.to.id == id) state.msgToFAX.emplace_back(x);
                            break;
                        case DeviceType::TRANKI_GC: 
                            if(x.to.id == id) state.msgToTRANKI_GC.emplace_back(x);
                            break;
                        case DeviceType::TRANKI_E: 
                            if(x.to.id == id) state.msgToTRANKI_E.emplace_back(x);
                            break;
                        default: assert(false && "External. Cannot get a message from that DeviceType");break;
                    }                   
                }
                state.advance = TIME("00:00:00:001");               
            }

            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
                internal_transition();
                external_transition(TIME(), std::move(mbs));
            }

            // output function
            typename make_message_bags<output_ports>::type output() const {
                typename make_message_bags<output_ports>::type bags;
                for (int i = 0; i < (state.msgToMOBILEPHONE.size()); i++){
                  get_messages<typename defs::outMOBILEPHONE>(bags).push_back(state.msgToMOBILEPHONE[i]);        
                }
                for (int i = 0; i < (state.msgToLANDLINEPHONE.size()); i++){
                  get_messages<typename defs::outLANDLINEPHONE>(bags).push_back(state.msgToLANDLINEPHONE[i]);        
                }
                 for (int i = 0; i < (state.msgToPRIVATELINEPHONE.size()); i++){
                  get_messages<typename defs::outPRIVATELINEPHONE>(bags).push_back(state.msgToPRIVATELINEPHONE[i]);        
                }
                 for (int i = 0; i < (state.msgToSATELLITE_PHONE.size()); i++){
                  get_messages<typename defs::outSATELLITE_PHONE>(bags).push_back(state.msgToSATELLITE_PHONE[i]);        
                }
                 for (int i = 0; i < (state.msgToRADIOLOGICAL_GROUP_DEVICE.size()); i++){
                  get_messages<typename defs::outRADIOLOGICAL_GROUP_DEVICE>(bags).push_back(state.msgToRADIOLOGICAL_GROUP_DEVICE[i]);        
                }
                 for (int i = 0; i < (state.msgToBEEPER.size()); i++){
                  get_messages<typename defs::outBEEPER>(bags).push_back(state.msgToBEEPER[i]);        
                }
                for (int i = 0; i < (state.msgToRADIO_REMAN.size()); i++){
                  get_messages<typename defs::outRADIO_REMAN>(bags).push_back(state.msgToRADIO_REMAN[i]);        
                }
                 for (int i = 0; i < (state.msgToRADIO_REMER.size()); i++){
                  get_messages<typename defs::outRADIO_REMER>(bags).push_back(state.msgToRADIO_REMER[i]);        
                }
                 for (int i = 0; i < (state.msgToEMAIL.size()); i++){
                  get_messages<typename defs::outEMAIL>(bags).push_back(state.msgToEMAIL[i]);        
                }
                 for (int i = 0; i < (state.msgToFAX.size()); i++){
                  get_messages<typename defs::outFAX>(bags).push_back(state.msgToFAX[i]);        
                }
                 for (int i = 0; i < (state.msgToTRANKI_GC.size()); i++){
                  get_messages<typename defs::outTRANKI_GC>(bags).push_back(state.msgToTRANKI_GC[i]);        
                }
                 for (int i = 0; i < (state.msgToTRANKI_E.size()); i++){
                  get_messages<typename defs::outTRANKI_E>(bags).push_back(state.msgToTRANKI_E[i]);        
                }
                return bags;
            }

            // time_advance function
            TIME time_advance() const {
              return state.advance;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename filterDevicesNetwork<TIME>::state_type& i) {
                os << "advance: " << i.advance; 
            return os;
            }
        };    
            
#endif //CADMIUM_FILTER_DEVICES_NETWORK_HPP