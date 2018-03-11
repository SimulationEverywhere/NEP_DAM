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


#ifndef CADMIUM_TEXT_MSG_NETWORK_HPP
#define CADMIUM_TEXT_MSG_NETWORK_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>

#include "../data_structures/struct_types.hpp"
#include "../data_structures/communication.hpp"
#include "../data_structures/nep_model_enum_types.hpp"

using namespace std;
using namespace cadmium;

//Port declaration
struct NetText_defs{
  //input
  struct in : public in_port<Communication> {};
  struct inNetState : public in_port<SetNetworkState> {};
  //output
  struct out : public out_port<Communication> {};
};
//Model definition
template<typename TIME>
class NetText {
  using defs=NetText_defs; // putting definitions in context
  public:
    //Parameters to be overwriten when instantiating the atomic model
    TIME  procesingTime;
    string id;
    // default constructor
    NetText(string id_param, TIME ProcesingTime_param) noexcept {
      id = id_param;
      procesingTime = ProcesingTime_param;
      state.broken =  false;
      state.advanceTime = std::numeric_limits<TIME>::infinity();
    }
    // state definition
    struct state_type{
      bool                               broken;
      vector<tuple<TIME,Communication>>  msg2Transmit;
      TIME                               advanceTime;
    };
    state_type state;
    // ports definition
    using input_ports=std::tuple<typename defs::in, typename defs::inNetState>;
    using output_ports=std::tuple<typename defs::out>;
    // internal transition
    void internal_transition() {
      /** Update the time of the message to leave the network **/
      for(int i = 0; i<state.msg2Transmit.size(); i++){
        get<TIME>(state.msg2Transmit[i]) = get<TIME>(state.msg2Transmit[i]) - state.advanceTime;
      }
      /**Delete the message that already left the network**/
      while(true){
        if(!state.msg2Transmit.empty()){
          if(get<TIME>(state.msg2Transmit[0])==TIME()) state.msg2Transmit.erase(state.msg2Transmit.begin());
          else break;
        }else{
          break;
        }      
      }
      /**Update time advance*/
      if(!state.msg2Transmit.empty()) state.advanceTime = get<TIME>(state.msg2Transmit[0]);
      else state.advanceTime = std::numeric_limits<TIME>::infinity();
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
      //inNetState Port
      if(!get_messages<typename defs::inNetState>(mbs).empty()){
        if(get_messages<typename defs::inNetState>(mbs).size() > 1) throw std::logic_error("Only one message at a time allowed");
        else{
          for(const auto &x : get_messages<typename defs::inNetState>(mbs)){
            state.broken = x.outOfOrder;
          }
        }
      }
      /**Update the time of the message to leave the network**/
      for(int i = 0; i<state.msg2Transmit.size(); i++){
        get<TIME>(state.msg2Transmit[i]) = get<TIME>(state.msg2Transmit[i]) - e;
      }
      /**Add the new incomming messages **/
      for (const auto &x : get_messages<typename defs::in>(mbs)) {
        state.msg2Transmit.push_back(make_tuple(procesingTime, x));
      }
      /**If the network is broken, return an acknoledgement when the first message was suposed to leave the network**/
      if(state.broken){
        for(int i = 0; i < state.msg2Transmit.size(); i++){
          Communication aux = get<Communication>(state.msg2Transmit[i]);
          get<Communication>(state.msg2Transmit[i])=Communication(aux.type, TextMsgType::NET_OUT_OF_ORDER, aux.to, aux.from, aux.command);
          get<TIME>(state.msg2Transmit[i]) = get<TIME>(state.msg2Transmit[0]);
        }
      }   
      /**Update time advance**/
      if(!state.msg2Transmit.empty()) state.advanceTime = get<TIME>(state.msg2Transmit[0]);
      else state.advanceTime = std::numeric_limits<TIME>::infinity();
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
        if(get<TIME>(state.msg2Transmit[i])==state.advanceTime) get_messages<typename defs::out>(bags).emplace_back(get<Communication>(state.msg2Transmit[i]));
        else break;
      }
      return bags;
    }
    // time_advance function
    TIME time_advance() const {
        return state.advanceTime;
    }
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename NetText<TIME>::state_type& i) {
      os << "next time: " << i.advanceTime << " broken:" << i.broken << " msg2transmit: " << i.msg2Transmit.size();
        for(int j = 0; j<i.msg2Transmit.size(); j++ ){
            os << " " << get<Communication>(i.msg2Transmit[j]);
        }
      return os;
    }
};                
#endif //CADMIUM_TEXT_MSG_NETWORK_HPP