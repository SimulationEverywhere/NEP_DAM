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

#ifndef CADMIUM_FILTER_NETWORKS_STATE_HPP
#define CADMIUM_FILTER_NETWORKS_STATE_HPP

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

//Port declaration
struct filterNetworksState_defs{
  //input
  struct in : public in_port<SetNetworkState> {};
  //output
  struct outPhoneNet : public out_port<SetNetworkState> {};
  struct outInternetNet : public out_port<SetNetworkState> {};
  struct outFaxNet : public out_port<SetNetworkState> {};
  struct outBeeperNet : public out_port<SetNetworkState> {};
  struct outRadioRemanNet : public out_port<SetNetworkState> {};
  struct outRadioRemerNet : public out_port<SetNetworkState> {};
  struct outRadiologicalGroupNet : public out_port<SetNetworkState> {};
  struct outCivilGuardNet : public out_port<SetNetworkState> {};
  struct outAutonomousPoliceNet : public out_port<SetNetworkState> {};
  struct outSatelliteNet : public out_port<SetNetworkState> {}; 
  struct outSpecialPhoneNet : public out_port<SetNetworkState> {}; 
};
//Model definition
template<typename TIME>
class filterNetworksState {
  using defs=filterNetworksState_defs; // putting definitions in context
  public:           
    // default constructor
    filterNetworksState() noexcept { }
    // state definition
    struct state_type{
      vector<SetNetworkState> msgToPhoneNet;
      vector<SetNetworkState> msgToInternetNet;
      vector<SetNetworkState> msgToFaxNet;
      vector<SetNetworkState> msgToBeeperNet;
      vector<SetNetworkState> msgToRadioRemanNet;
      vector<SetNetworkState> msgToRadioRemerNet;
      vector<SetNetworkState> msgToRadiologicalGroupNet;
      vector<SetNetworkState> msgToCivilGuardNet;
      vector<SetNetworkState> msgToAutonomousPoliceNet;
      vector<SetNetworkState> msgToSatelliteNet;
      vector<SetNetworkState> msgToSpecialPhoneNet;
      TIME                    advance;
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
        switch(x.to){
          case NetworkId::PHONE_NET: state.msgToPhoneNet.emplace_back(x); break;
          case NetworkId::PRIVATELINEPHONE_NET: state.msgToSpecialPhoneNet.emplace_back(x); break;
          case NetworkId::SATELLITE_NET: state.msgToSatelliteNet.emplace_back(x); break;
          case NetworkId::RADIOLOGICAL_GROUP_NET: state.msgToRadiologicalGroupNet.emplace_back(x); break;
          case NetworkId::BEEPER_NET: state.msgToBeeperNet.emplace_back(x); break;
          case NetworkId::RADIO_REMAN_NET: state.msgToRadioRemanNet.emplace_back(x); break;
          case NetworkId::RADIO_REMER_NET:  state.msgToRadioRemerNet.emplace_back(x); break;
          case NetworkId::INTERNET: state.msgToInternetNet.emplace_back(x); break;
          case NetworkId::FAX_NET: state.msgToFaxNet.emplace_back(x); break;
          case NetworkId::TRANKI_GC_NET: state.msgToCivilGuardNet.emplace_back(x); break;
          case NetworkId::TRANKI_E_NET: state.msgToAutonomousPoliceNet.emplace_back(x); break;
          default: assert(false && "External. Cannot get a message from that NetworkId"); break;
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
    //<< operator for the state
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename filterNetworksState<TIME>::state_type& i) {
      os << "advance: " << i.advance; 
      return os;
    }
};           
#endif //CADMIUM_FILTER_NETWORKS_STATE_HPP