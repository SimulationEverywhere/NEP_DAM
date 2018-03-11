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

#ifndef CADMIUM_PEOPLE_IN_LOCATION_HPP
#define CADMIUM_PEOPLE_IN_LOCATION_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>

#include <string>
#include <iostream>
#include <vector>
#include <assert.h>

#include "../data_structures/struct_types.hpp"
#include "../data_structures/nep_model_enum_types.hpp"

using namespace std;
using namespace cadmium;

using namespace nep_structures;
using namespace nep_model_enum_types;

//Port definition
struct PeopleInLocation_defs{
  //input ports
  struct in : public in_port<PeopleLocation> {};
  //output ports
  struct out : public out_port<PeopleClassifiedByLocation> {};       
};
//MODEL
template<typename TIME>
class PeopleInLocation {
  using defs=PeopleInLocation_defs; // putting definitions in context
  public:
    //default constructor
    PeopleInLocation() noexcept {}
    // state definition
    struct state_type{
      vector <PeopleLocation> peopleLocation;
      vector <PeopleClassifiedByLocation> peopleClassifiedByLocation;
      vector <string> locationsUpdated;
    };
    state_type state=state_type();            
    // ports definition
    using input_ports=std::tuple<typename defs::in>;
    using output_ports=std::tuple<typename defs::out>;
    // internal transition
    void internal_transition() {
      state.locationsUpdated.clear();   
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
      bool old_location_updated;
      bool new_location_updated;
      bool person_found;
      bool location_in_the_list_to_send;               
      for (const auto &x : get_messages<typename defs::in>(mbs)) {
        //cout << "external: " << "person id: " << x.peopleLocation.id << " location: " << x.peopleLocation.location << endl;
        old_location_updated = false;
        new_location_updated = false;
        person_found = false;
        location_in_the_list_to_send = false;
        //IF THE PERSON IS ALREADY REGISTERED                
        for(int j = 0; j < state.peopleLocation.size(); j++){
          if(x.id == state.peopleLocation[j].id){
            person_found = true;
            //cout << "person_found" << endl;
            for (int z = 0; z < state.peopleClassifiedByLocation.size(); z++){
              //FIND THE PREVIOUS LOCATION
                if(state.peopleLocation[j].location == state.peopleClassifiedByLocation[z].location){ 
                  //FIND THE PERSON AND DELETE IT FROM THAT LOCATION. OLD LOCATION UPDATED            
                  for(int k=0; k < state.peopleClassifiedByLocation[z].ids.size(); k++){
                    if(x.id == state.peopleClassifiedByLocation[z].ids[k]){
                      state.peopleClassifiedByLocation[z].ids.erase(state.peopleClassifiedByLocation[z].ids.begin()+k);
                      old_location_updated = true;
                      //IF THE LOCATION IS EMPTY DELETE FROM THE LIST. ELSE ADD TO THE LOCATIONS TO SEND IF IT IS NOT ALREADY
                      if(state.peopleClassifiedByLocation[z].ids.empty()){
                        state.peopleClassifiedByLocation.erase(state.peopleClassifiedByLocation.begin()+z);
                      }else{
                        location_in_the_list_to_send = false;
                        for(int l = 0; l<state.locationsUpdated.size(); l++){
                          if(state.locationsUpdated[l] == state.peopleClassifiedByLocation[z].location){
                            location_in_the_list_to_send = true;
                            break;
                          } 
                        }
                        if(location_in_the_list_to_send == false) state.locationsUpdated.push_back(state.peopleLocation[j].location);
                      }                            
                      break;
                    }
                  }
                }
              //FIND THE NEW LOCATION AND UPDATE IT
                if(x.location == state.peopleClassifiedByLocation[z].location){
                  //ADD THE PERSON TO THE NEW LOCATION
                  state.peopleClassifiedByLocation[z].ids.push_back(x.id);
                  new_location_updated = true;
                  //UPDATE THE LOCATIONS TO SEND (THE ONES THAT HAVE CHANGED)
                  location_in_the_list_to_send = false;
                  for(int l = 0; l<state.locationsUpdated.size(); l++){
                    if(state.locationsUpdated[l] == state.peopleClassifiedByLocation[z].location){
                      location_in_the_list_to_send = true;
                      break;
                    } 
                  }
                  if(location_in_the_list_to_send == false) state.locationsUpdated.push_back(x.location);                
                }
              //IF OLD AND NEW LOCATION UPDATED. I HAVE FINISHED LOOKING FOR THEM
                if(old_location_updated && new_location_updated) break;
            }
            //UPDATE THE PERSON LOCATION
            state.peopleLocation[j] = x;
            //IF THE NEW LOCATION IS NOT REGISTERED ADD IT
            if(new_location_updated == false){
              PeopleClassifiedByLocation aux_p;
              aux_p.location = x.location;
              aux_p.ids.push_back(x.id);
              state.peopleClassifiedByLocation.push_back(aux_p);
              new_location_updated = true;
              state.locationsUpdated.push_back(x.location);
            }
            //I HAVE FINISHED WITH THE MESSAGE
            break;
          }
        }
        //IF THE PERSON IS NOT REGISTERED
        if(person_found == false){
          //cout << "entra en person_found == false" << endl;
          //REGISTED THE PERSON. THE OLD LOCATION IS UPDATED AS IT DID NOT EXISTS.
            state.peopleLocation.push_back(x);
            old_location_updated = true;
            //UPDATE THE NEW LOCATION AND TO THE LIST TO SEND IF NOT ALREADY
            for (int z = 0; z < state.peopleClassifiedByLocation.size(); z++){
              if(x.location == state.peopleClassifiedByLocation[z].location){
                state.peopleClassifiedByLocation[z].ids.push_back(x.id);
                new_location_updated = true;
                location_in_the_list_to_send = false;
                for(int l = 0; l<state.locationsUpdated.size(); l++){
                  if(state.locationsUpdated[l] == state.peopleClassifiedByLocation[z].location){
                    location_in_the_list_to_send = true;
                    break;
                  } 
                }
                if(location_in_the_list_to_send == false) state.locationsUpdated.push_back(x.location);                
                break;
              }
            }
            if(new_location_updated == false){
              //cout << "entra en new_location_updated == false" << endl;
              PeopleClassifiedByLocation aux_p;
              aux_p.location = x.location;
              aux_p.ids.push_back(x.id);
              state.peopleClassifiedByLocation.push_back(aux_p);
              new_location_updated = true;
              state.locationsUpdated.push_back(x.location);
              //cout << state.locationsUpdated.size() << endl;
            }
        }
        if(old_location_updated ==false || new_location_updated == false) assert(false && "Error programing the function");
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
      for(int i = 0; i < state.locationsUpdated.size(); i++){
        for(int j = 0; j < state.peopleClassifiedByLocation.size(); j++){
          if(state.locationsUpdated[i] == state.peopleClassifiedByLocation[j].location){
            get_messages<typename defs::out>(bags).emplace_back(state.peopleClassifiedByLocation[j]);
            break;
          }
        }      
      }
      return bags;
    }
    // time_advance function
    TIME time_advance() const {
      return (state.locationsUpdated.empty() ? std::numeric_limits<TIME>::infinity() : TIME("00:00:00:001"));
    }
    // << operator for the state
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename PeopleInLocation<TIME>::state_type& i) {
      os << "#locationsUpdated:" << i.locationsUpdated.size(); 
      return os;
    }              
};    
#endif //CADMIUM_PEOPLE_IN_LOCATION_HPP