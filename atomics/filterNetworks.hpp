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

#ifndef CADMIUM_FILTER_NETWORKS_HPP
#define CADMIUM_FILTER_NETWORKS_HPP

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
    struct filterNetworks_defs{
        struct in : public in_port<Communication> {
        };
        struct outPhoneNet : public out_port<Communication> {
        };
        struct outInternetNet : public out_port<Communication> {
        };
        struct outFaxNet : public out_port<Communication> {
        };
        struct outBeeperNet : public out_port<Communication> {
        };
        struct outRadioRemanNet : public out_port<Communication> {
        };
        struct outRadioRemerNet : public out_port<Communication> {
        };
        struct outRadiologicalGroupNet : public out_port<Communication> {
        };
        struct outCivilGuardNet : public out_port<Communication> {
        };
        struct outAutonomousPoliceNet : public out_port<Communication> {
        };
        struct outSatelliteNet : public out_port<Communication> {
        }; 
        struct outSpecialPhoneNet : public out_port<Communication> {
        }; 
    };

    template<typename TIME>
    class filterNetworks {
        using defs=filterNetworks_defs; // putting definitions in context
        public:           
            // default constructor
            filterNetworks() noexcept {
            }
            // state definition
            struct state_type{
                vector<Communication> msgToPhoneNet;
                vector<Communication> msgToInternetNet;
                vector<Communication> msgToFaxNet;
                vector<Communication> msgToBeeperNet;
                vector<Communication> msgToRadioRemanNet;
                vector<Communication> msgToRadioRemerNet;
                vector<Communication> msgToRadiologicalGroupNet;
                vector<Communication> msgToCivilGuardNet;
                vector<Communication> msgToAutonomousPoliceNet;
                vector<Communication> msgToSatelliteNet;
                vector<Communication> msgToSpecialPhoneNet;
                TIME                  advance;
            }; 
            state_type state;
            // ports definition
            using input_ports=std::tuple<typename defs::in>;
            using output_ports=std::tuple<typename defs::outPhoneNet, typename defs::outInternetNet, typename defs::outFaxNet, typename defs::outBeeperNet,
                    typename defs::outRadioRemanNet, typename defs::outRadioRemerNet, typename defs::outRadiologicalGroupNet, typename defs::outCivilGuardNet,
                    typename defs::outAutonomousPoliceNet, typename defs::outSatelliteNet, typename defs::outSpecialPhoneNet>;

            // internal transition
            void internal_transition() {
                state.msgToPhoneNet.clear();
                state.msgToInternetNet.clear();
                state.msgToFaxNet.clear();
                state.msgToBeeperNet.clear();
                state.msgToRadioRemanNet.clear();
                state.msgToRadioRemerNet.clear();
                state.msgToRadiologicalGroupNet.clear();
                state.msgToCivilGuardNet.clear();
                state.msgToAutonomousPoliceNet.clear();
                state.msgToSatelliteNet.clear();
                state.msgToSpecialPhoneNet.clear();
                state.advance = std::numeric_limits<TIME>::infinity();
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {                       
                for (const auto &x : get_messages<typename defs::in>(mbs)) {
                    switch(x.from.type){
                        case DeviceType::MOBILEPHONE: state.msgToPhoneNet.emplace_back(x);break;
                        case DeviceType::LANDLINEPHONE: state.msgToPhoneNet.emplace_back(x);break;
                        case DeviceType::PRIVATELINEPHONE: state.msgToSpecialPhoneNet.emplace_back(x);break;
                        case DeviceType::SATELLITE_PHONE: state.msgToSatelliteNet.emplace_back(x);break;
                        case DeviceType::RADIOLOGICAL_GROUP_DEVICE: state.msgToRadiologicalGroupNet.emplace_back(x);break;
                        case DeviceType::BEEPER: state.msgToBeeperNet.emplace_back(x);break;
                        case DeviceType::RADIO_REMAN: state.msgToRadioRemanNet.emplace_back(x);break;
                        case DeviceType::RADIO_REMER: state.msgToRadioRemerNet.emplace_back(x);break;
                        case DeviceType::EMAIL: state.msgToInternetNet.emplace_back(x);break;
                        case DeviceType::FAX: state.msgToFaxNet.emplace_back(x);break;
                        case DeviceType::TRANKI_GC: state.msgToCivilGuardNet.emplace_back(x);break;
                        case DeviceType::TRANKI_E: state.msgToAutonomousPoliceNet.emplace_back(x);break;
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
                for (int i = 0; i < (state.msgToPhoneNet.size()); i++){
                  get_messages<typename defs::outPhoneNet>(bags).push_back(state.msgToPhoneNet[i]);        
                }
                 for (int i = 0; i < (state.msgToInternetNet.size()); i++){
                  get_messages<typename defs::outInternetNet>(bags).push_back(state.msgToInternetNet[i]);        
                }
                 for (int i = 0; i < (state.msgToFaxNet.size()); i++){
                  get_messages<typename defs::outFaxNet>(bags).push_back(state.msgToFaxNet[i]);        
                }
                 for (int i = 0; i < (state.msgToBeeperNet.size()); i++){
                  get_messages<typename defs::outBeeperNet>(bags).push_back(state.msgToBeeperNet[i]);        
                }
                 for (int i = 0; i < (state.msgToRadioRemanNet.size()); i++){
                  get_messages<typename defs::outRadioRemanNet>(bags).push_back(state.msgToRadioRemanNet[i]);        
                }
                for (int i = 0; i < (state.msgToRadioRemerNet.size()); i++){
                  get_messages<typename defs::outRadioRemerNet>(bags).push_back(state.msgToRadioRemerNet[i]);        
                }
                 for (int i = 0; i < (state.msgToRadiologicalGroupNet.size()); i++){
                  get_messages<typename defs::outRadiologicalGroupNet>(bags).push_back(state.msgToRadiologicalGroupNet[i]);        
                }
                 for (int i = 0; i < (state.msgToCivilGuardNet.size()); i++){
                  get_messages<typename defs::outCivilGuardNet>(bags).push_back(state.msgToCivilGuardNet[i]);        
                }
                 for (int i = 0; i < (state.msgToAutonomousPoliceNet.size()); i++){
                  get_messages<typename defs::outAutonomousPoliceNet>(bags).push_back(state.msgToAutonomousPoliceNet[i]);        
                }
                 for (int i = 0; i < (state.msgToSatelliteNet.size()); i++){
                  get_messages<typename defs::outSatelliteNet>(bags).push_back(state.msgToSatelliteNet[i]);        
                }
                 for (int i = 0; i < (state.msgToSpecialPhoneNet.size()); i++){
                  get_messages<typename defs::outSpecialPhoneNet>(bags).push_back(state.msgToSpecialPhoneNet[i]);        
                }
                return bags;
            }

            // time_advance function
            TIME time_advance() const {
              return state.advance;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename filterNetworks<TIME>::state_type& i) {
                os << "advance: " << i.advance; 
            return os;
            }
        };    
            
#endif //CADMIUM_FILTER_NETWORKS_HPP