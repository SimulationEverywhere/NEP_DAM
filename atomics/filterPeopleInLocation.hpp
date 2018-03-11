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

#ifndef CADMIUM_FILTER_PEOPLE_IN_LOCATION_HPP
#define CADMIUM_FILTER_PEOPLE_IN_LOCATION_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>

#include "../data_structures/nep_model_enum_types.hpp"
#include "../data_structures/struct_types.hpp"


using namespace std;
using namespace cadmium;

using namespace nep_model_enum_types;
using namespace nep_structures;

//Port definition
    struct filterPeopleInLocation_defs{
        struct out : public out_port<PeopleClassifiedByLocation> {
        };
        struct in : public in_port<PeopleClassifiedByLocation> {
        };
    };
//This is a meta-model, it should be overloaded for declaring the "id" parameter
    template<typename TIME>
    class filterPeopleInLocation {
        using defs=filterPeopleInLocation_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            string id; // string id to to be filtered
            // default constructor
            filterPeopleInLocation(string id_param) noexcept {
                id = id_param;
            }
            // state definition
            struct state_type{
              vector<PeopleClassifiedByLocation> msgPassingFilter;
            }; 
            state_type state;
            // ports definition
            using input_ports=std::tuple<typename defs::in>;
            using output_ports=std::tuple<typename defs::out>;

            // internal transition
            void internal_transition() {
                state.msgPassingFilter.clear();
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {                       
                for (const auto &x : get_messages<typename defs::in>(mbs)) {
                  for(int i = 0; i<x.ids.size(); i++){
                    if(x.ids[i] == id){
                      state.msgPassingFilter.emplace_back(x); 
                      break;
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
                for (int i = 0; i < (state.msgPassingFilter.size()); i++){
                  get_messages<typename defs::out>(bags).push_back(state.msgPassingFilter[i]);        
                }
                return bags;
            }

            // time_advance function
            TIME time_advance() const {
              return (state.msgPassingFilter.empty() ? std::numeric_limits<TIME>::infinity() : TIME("00:00:00:001"));
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename filterPeopleInLocation<TIME>::state_type& i) {
                os << "#msgPassingFilter:" << i.msgPassingFilter.size(); 
            return os;
            }
        };    
            
#endif //CADMIUM_FILTER_PEOPLE_IN_LOCATION_HPP