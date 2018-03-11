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


#ifndef CADMIUM_RADIO_NETWORK_HPP
#define CADMIUM_RADIO_NETWORK_HPP

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
struct NetRadio_defs{
  //input
  struct in : public in_port<Communication> {};
  struct inNetState : public in_port<SetNetworkState> {};
  //output
  struct out : public out_port<Communication> {};
};
//Model definition
template<typename TIME>
class NetRadio {
  using defs=NetRadio_defs; // putting definitions in context
  public:
    //Parameters to be overwriten when instantiating the atomic model
    TIME  procesingTime;
    int prob_interferences;
    string id;
    // default constructor
    NetRadio(string id_param, TIME procesingTime_param, int prob_interferences_param) noexcept {
      id = id_param;
      procesingTime = procesingTime_param;
      prob_interferences = prob_interferences_param;
      state.broken = false;
    }
    // state definition
    struct state_type{
      vector <Communication> msg2Transmit;
      bool                   broken;
    };
    state_type state;
    // ports definition
    using input_ports=std::tuple<typename defs::in, typename defs::inNetState>;
    using output_ports=std::tuple<typename defs::out>;
    // internal transition
    void internal_transition() {
      if(state.msg2Transmit.empty()) throw std::logic_error("The internal cannot be reached in this state");
      state.msg2Transmit.clear();
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
      if(!state.msg2Transmit.empty()) {
        cout << "WARNING. the external in the network is happening when the model is not passivated" << endl;
      }
      //inNetState Port
      if(!get_messages<typename defs::inNetState>(mbs).empty()){
        if(get_messages<typename defs::inNetState>(mbs).size() > 1) throw std::logic_error("Only one message at a time allowed");
        else{
          for(const auto &x : get_messages<typename defs::inNetState>(mbs)){
            state.broken = x.outOfOrder;
          }
        }
      }
      /** Add the new incomming messages **/
      for (auto &x : get_messages<typename defs::in>(mbs)) {                                  
        if(state.broken){
          x.msg_radio = RadioMsgType::NET_OUT_OF_ORDER;
          x.to.id = "all";
        }else if(x.msg_radio == RadioMsgType::COMMAND_TRANSMITTED){
          bool interferences = false;  
          unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // seed variable is used to ensure a different set of results for each simulation
          srand (seed);                    
          int aux = rand() %100;                    
          (aux < prob_interferences) ? interferences = true : interferences = false;                   
          if(interferences) x.msg_radio = RadioMsgType::INTERFERENCES;
        }
        state.msg2Transmit.push_back(x);
      }             
    }
    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
      internal_transition();
      external_transition(TIME(),std::move(mbs));
    }
    // output function
    typename make_message_bags<output_ports>::type output() const {
      typename make_message_bags<output_ports>::type bags;
      for(int i = 0; i<state.msg2Transmit.size(); i++){
        get_messages<typename defs::out>(bags).push_back(state.msg2Transmit[i]);
      }
      return bags;
    }
    // time_advance function
    TIME time_advance() const {
      return (state.msg2Transmit.empty() ? std::numeric_limits<TIME>::infinity() : procesingTime);
    }
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename NetRadio<TIME>::state_type& i) {
      os << " state: " << i.broken; 
      return os;
    }
};          
#endif //CADMIUM_RADIO_NETWORK_HPP