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


#ifndef CADMIUM_NET_PHONE_HPP
#define CADMIUM_NET_PHONE_HPP

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

//Port declaration
struct NetPhone_defs{
  //input
  struct in : public in_port<Communication> {};
  struct inNetState : public in_port<SetNetworkState> {};
  //output
  struct out : public out_port<Communication> {};
};
//Model definition
template<typename TIME>
class NetPhone {
  using defs=NetPhone_defs; // putting definitions in context
  public:
    //Parameters to be overwriten when instantiating the atomic model
    TIME  procesingTime;
    int prob_busy;
    string id;
    // default constructor
    NetPhone(string id_param, TIME procesingTime_param, int prob_busy_param) noexcept {
      id = id_param;
      procesingTime = procesingTime_param;
      prob_busy = prob_busy_param;
      state.broken = false;
    }
    // state definition
    struct state_type{
      bool broken;
      vector <Communication> msg2Transmit;
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
        cout << "WARNING. External in network " << id << " is happening when the model is not passivated" << endl;
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
      //in Port
      for (const auto &x : get_messages<typename defs::in>(mbs)) {
        //cout << "EXTERNAL. NetPhone "<< id <<". in port: " << x << endl;
        bool busy = false;
        if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
          unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // seed variable is used to ensure a different set of results for each simulation
          srand (seed);    
          int aux = rand() %100;
          (aux < prob_busy) ? busy = true : busy = false;
          if(state.broken) state.msg2Transmit.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::NET_OUT_OF_ORDER, x.to, x.from));
          else if(busy) state.msg2Transmit.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, x.to, x.from));
          else state.msg2Transmit.emplace_back(x);
        }else{
          if(state.broken){
            state.msg2Transmit.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::NET_OUT_OF_ORDER, x.to, x.from));
            state.msg2Transmit.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, x.from, x.to));
          }else{
            state.msg2Transmit.emplace_back(x);
          }
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
      for(int i = 0; i<state.msg2Transmit.size(); i++){
        get_messages<typename defs::out>(bags).push_back(state.msg2Transmit[i]);
        //cout << "OUTPUT. NetPhone "<< id <<": " << state.msg2Transmit[i] << endl;
      }
        return bags;
    }
    // time_advance function
    TIME time_advance() const {
      return (state.msg2Transmit.empty() ? std::numeric_limits<TIME>::infinity() : procesingTime);
    }
    // << operator for the state
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename NetPhone<TIME>::state_type& i) {
      os << " state: " << i.broken; 
      return os;
    }
};          
#endif //CADMIUM_NET_PHONE_HPP