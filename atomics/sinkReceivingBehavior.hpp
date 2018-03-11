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

#ifndef CADMIUM_SINK_RECEIVING_BEHAVIOR_HPP
#define CADMIUM_SINK_RECEIVING_BEHAVIOR_HPP

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
struct sinkReceivingBehavior_defs{
  struct inCommunication : public in_port<Communication> {
  };

};

template<typename TIME>
class sinkReceivingBehavior {
  using defs=sinkReceivingBehavior_defs; // putting definitions in context
  public:
    // default constructor
    sinkReceivingBehavior() noexcept {
    }
    // state definition
    struct state_type{   
    };
    state_type state;
    // ports definition
    using input_ports=std::tuple<typename defs::inCommunication>;
    using output_ports=std::tuple<>;
    // internal transition
    void internal_transition() {
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
      //inCommunication Port
      if(!get_messages<typename defs::inCommunication>(mbs).empty()){
        assert(false && "This device does not exist in the model. External. inCommunication Port");
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
      return bags;
    }

    // time_advance function
    TIME time_advance() const {
      return std::numeric_limits<TIME>::infinity();
    }
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename sinkReceivingBehavior<TIME>::state_type& i) {
      os << "is a sink"; 
      return os;
    }
};
#endif //CADMIUM_SINK_RECEIVING_BEHAVIOR_HPP