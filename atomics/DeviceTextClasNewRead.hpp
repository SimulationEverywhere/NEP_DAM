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


#ifndef CADMIUM_DEVICE_TEXT_CLAS_NEW_READ_HPP
#define CADMIUM_DEVICE_TEXT_CLAS_NEW_READ_HPP

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
struct DeviceTextClasNewRead_defs{
    //Input ports
    struct in : public in_port<Communication> {};
    //Output ports
    struct newOut : public out_port<Communication> {};
    struct readOut : public out_port<Communication> {};
};
//modelDEL
template<typename TIME>
class DeviceTextClasNewRead {
    using defs=DeviceTextClasNewRead_defs; // putting definitions in context
    public:
        //Parameters 
        TIME         processingTime; // Processing time
        // default constructor
        DeviceTextClasNewRead(TIME processingTime_param) noexcept {
            processingTime = processingTime_param;
        }
        // state definition
        struct state_type{
            vector <Communication> newMsg;
            vector <Communication> readMsg;
        }; 
        state_type state;
        // ports definition
        using input_ports=std::tuple<typename defs::in>;
        using output_ports=std::tuple<typename defs::newOut, typename defs::readOut>;
        // internal transition
        void internal_transition() {
          state.newMsg.clear();
          state.readMsg.clear();
        }
        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
            //cout << "EXTERNAL. DeviceTextClasNewRead - ";
          for (const auto &x : get_messages<typename defs::in>(mbs)){
            //cout << x << " | ";
            switch(x.type){
              case MessageType::TEXT_MESSAGE:
                switch (x.msg_text){
                  case TextMsgType::NEW: state.newMsg.push_back(x); break;
                  case TextMsgType::READ: state.readMsg.push_back(x); break;
                  default:assert(false&&"This TextMsgType is not allowed in this model"); break;
                }
                break;
              default: assert(false && "MessageType not allowed in this model"); break;
            }
          }
           //cout << endl;
        }
        // confluence transition
        void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
            internal_transition();
            external_transition(TIME(), std::move(mbs));
        }
        // output function
        typename make_message_bags<output_ports>::type output() const {
            //cout << "OUTPUT. DeviceTextClasNewRead - ";
            typename make_message_bags<output_ports>::type bags;
            for (int i = 0; i < (state.newMsg.size()); i++){
               // cout << "newOut: " << state.newMsg[i] << " | ";
              get_messages<typename defs::newOut>(bags).push_back(state.newMsg[i]); 
            }
            for (int i = 0; i < (state.readMsg.size()); i++){
               // cout << "readOut: " << state.readMsg[i] << " | ";
              get_messages<typename defs::readOut>(bags).push_back(state.readMsg[i]);           
            }
            //cout << endl;
            return bags;
        }
        // time_advance function
        TIME time_advance() const {
            return ((state.newMsg.empty()&&state.readMsg.empty()) ? std::numeric_limits<TIME>::infinity() : processingTime);
        }
        // << operator for the state
            friend std::ostringstream& operator<<(std::ostringstream& os, const typename DeviceTextClasNewRead<TIME>::state_type& i) {
            os << " newMsg: " << i.newMsg.size() << " readMsg: " << i.readMsg.size(); 
            return os;
        }
};           
#endif //CADMIUM_DEVICE_TEXT_CLAS_NEW_READ_HPP