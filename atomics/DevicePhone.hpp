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

#ifndef CADMIUM_DEVICE_PHONE_HPP
#define CADMIUM_DEVICE_PHONE_HPP

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
struct DevicePhone_defs{
  //input ports
  struct microphoneIn : public in_port<Communication> {};
  struct networkIn : public in_port<Communication> {};
  struct setStateIn : public in_port<SetDeviceState> {};
  //output ports
  struct speakerOut : public out_port<Communication> {};
  struct networkOut : public out_port<Communication> {};
};
//MODEL
template<typename TIME>
class DevicePhone {
  using defs=DevicePhone_defs; // putting definitions in context
  public:
    enum class PhoneState {IDLE, CALL_REQUESTED, INCOMING_CALL_REQUESTED, COMMUNICATION_STABLISHED, COMMAND_TRANSMITTED, COMMUNICATION_OVER, OUT_OF_ORDER};
    //Parameters
    DeviceId id;
    TIME timeProcessing;
	  TIME timeRinging;
    // default constructor
    DevicePhone(DeviceId id_param, TIME timeProcessing_param, TIME timeRinging_param) noexcept {
      id = id_param;
      timeRinging = timeRinging_param;
      timeProcessing = timeProcessing_param;
      state.state = PhoneState::IDLE;
      state.advanceTime = std::numeric_limits<TIME>::infinity();
    }
    // state definition
	  struct state_type{
	   	vector <Communication> networkOutMsg;
	   	vector <Communication> speakerOutMsg;
	   	PhoneState   state;
	   	DeviceId     busyWith;
	   	TIME         advanceTime;
	  };
    state_type state;
    // ports definition
    using input_ports=std::tuple<typename defs::microphoneIn, typename defs::setStateIn, typename defs::networkIn>;
    using output_ports=std::tuple<typename defs::speakerOut, typename defs::networkOut>;
    // internal transition
    void internal_transition() {
      //cout << id << " ENTRA EN LA INTERNAL" << endl;
      if(!state.networkOutMsg.empty() || !state.speakerOutMsg.empty()){
        state.networkOutMsg.clear();
    	  state.speakerOutMsg.clear();
    	  (state.state == PhoneState::CALL_REQUESTED) ? state.advanceTime = timeRinging : state.advanceTime = std::numeric_limits<TIME>::infinity();
      }else{ //The time requesting is over
    		if(state.state == PhoneState::CALL_REQUESTED){
    			state.networkOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, id, state.busyWith));
    			state.speakerOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::CALL_OVER, state.busyWith, id));
    			state.advanceTime = timeProcessing;
    			state.state = PhoneState::COMMUNICATION_OVER;   
    		}else{
    			assert(false && "There is not internal if there is nothing to send && state != CALL_REQUESTED"); 
    		}	    
      }
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
      //cout << id << " ENTRA EN LA EXTERNAL" << endl;
      TIME aux_time;
  	  if(state.state == PhoneState::CALL_REQUESTED) aux_time = state.advanceTime - e; //actualize the remining time that the phone will be ringing (CALL_REQUESTED)
  	  //setStateIn Port
      if(!get_messages<typename defs::setStateIn>(mbs).empty()){
        if(get_messages<typename defs::setStateIn>(mbs).size() > 1) throw std::logic_error("Only one message at a time allowed");
        else{
          for(const auto &x : get_messages<typename defs::setStateIn>(mbs)){
    		    if(state.state == PhoneState::OUT_OF_ORDER && x.outOfOrder == false){
              state.state = PhoneState::IDLE;
            }else if(state.state != PhoneState::OUT_OF_ORDER && x.outOfOrder == true){
              if((state.state != PhoneState::IDLE) || !state.speakerOutMsg.empty() || !state.networkOutMsg.empty()){
      			    state.speakerOutMsg.clear();
      			    state.speakerOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE, PhoneMsgType::OUT_OF_ORDER, id, id));
      			    if(!state.networkOutMsg.empty()){
                  for(int z = 0; z < state.networkOutMsg.size(); z++){
                    state.networkOutMsg[z].msg_phone = PhoneMsgType::CALL_OVER;
        			    }
      			    }
    			    }
    			    state.state = PhoneState::OUT_OF_ORDER;
            }
          }
        }
  	  }
      //microphoneIn Port
		  for (const auto &x : get_messages<typename defs::microphoneIn>(mbs)){  
  			//Assert for correct implementation of the model
  			if(state.state != PhoneState::IDLE && x.msg_phone != PhoneMsgType::CALL_OVER && state.state != PhoneState::OUT_OF_ORDER){
    				if(state.busyWith != x.to) throw std::logic_error("Message not allowed. External. Microphone port");
  			}
  			//State modification
    		switch(state.state){
      		case PhoneState::IDLE:
        		if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
          		state.state = PhoneState::CALL_REQUESTED;
          		state.busyWith = x.to;
          		state.networkOutMsg.emplace_back(x);          
        		}else if(x.msg_phone == PhoneMsgType::CALL_OVER){
        			//There is no call
        		}else{
          		throw std::logic_error("Message not allowed in IDLE state. External. Microphone port");
        		}
        		break;
      		case PhoneState::CALL_REQUESTED:
      			if(x.msg_phone == PhoneMsgType::CALL_OVER){
      				state.state = PhoneState::IDLE;
      				state.busyWith.clear();
      				state.speakerOutMsg.clear();
      			  state.networkOutMsg.emplace_back(x);
      			}else{
					    throw std::logic_error("Message not allowed in CALL_REQUESTED state. External Microphone port");
					  }
					  break;
      		case PhoneState::INCOMING_CALL_REQUESTED:
        		if(x.msg_phone == PhoneMsgType::CALL_ACCEPTED) {
        			state.state = PhoneState::COMMUNICATION_STABLISHED;
        		  state.networkOutMsg.emplace_back(x);
        		}else if(x.msg_phone == PhoneMsgType::CALL_OVER){
        		  state.state = PhoneState::IDLE;
      			  state.busyWith.clear();
      			  state.speakerOutMsg.clear();
      			  state.networkOutMsg.emplace_back(x);
        		}else{
        		  throw std::logic_error("Message not allowed in INCOMING_CALL_REQUESTED state. External Microphone port");
        		}              				
        		break;
      		case PhoneState::COMMUNICATION_STABLISHED:
      		case PhoneState::COMMAND_TRANSMITTED:
        		if(x.msg_phone == PhoneMsgType::COMMAND_TRANSMITTED){
        			state.state = PhoneState::COMMAND_TRANSMITTED;
        		}else if(x.msg_phone == PhoneMsgType::CALL_OVER){
        			state.state = PhoneState::IDLE;
      			  state.busyWith.clear();
      			  state.speakerOutMsg.clear();            		    
        		}else{
        			throw std::logic_error("Message not allowed in COMMUNICATION_STABLISHED or COMMAND_TRANSMITTED state. External Microphone port");
        		}
            state.networkOutMsg.emplace_back(x);
        		break;
      		case PhoneState::COMMUNICATION_OVER:
        		if (x.msg_phone == PhoneMsgType::CALL_OVER) (state.state = PhoneState::IDLE);
        		else throw std::logic_error("Message not allowed in COMMUNICATION_OVER state.External Microphone port");
        		break;
      		case PhoneState::OUT_OF_ORDER:
        		state.speakerOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::OUT_OF_ORDER,id,id));
      			if(x.msg_phone != PhoneMsgType::CALL_REQUEST){
          		state.networkOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::CALL_OVER,id,x.to));
        		}              
        		break;
	        default: assert(false && "Not that state"); break;
    		}            
		  }
    	//networkIn Port
    	for (const auto &x : get_messages<typename defs::networkIn>(mbs)){      
      	//Assert for correct implementation of the model
  			if(state.state != PhoneState::IDLE && x.msg_phone != PhoneMsgType::CALL_OVER && x.msg_phone != PhoneMsgType::CALL_REQUEST && state.state != PhoneState::OUT_OF_ORDER){
    			if(state.busyWith != x.from) throw std::logic_error("Message not allowed. External phone networkIn");
 				}
  			//state modification based on the input in networkIn
  			switch(state.state){
      		case PhoneState::IDLE:
        		if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
        			state.state = PhoneState::INCOMING_CALL_REQUESTED;
        			state.busyWith = x.from;       
        			state.speakerOutMsg.emplace_back(x);
        		}else if(x.msg_phone == PhoneMsgType::CALL_OVER){
        			//The call is already over
        		}else{
        			throw std::logic_error("Message not allowed in IDLE state. External. NetworkIn");
        		}
        		break;
      		case PhoneState::CALL_REQUESTED:
      			if(x.msg_phone == PhoneMsgType::CALL_ACCEPTED){
      				state.state = PhoneState::COMMUNICATION_STABLISHED;
      				state.speakerOutMsg.emplace_back(x);
      			}else if(x.msg_phone == PhoneMsgType::CALL_OVER || x.msg_phone == PhoneMsgType::NET_OUT_OF_ORDER){
      				state.state = PhoneState::COMMUNICATION_OVER;
      				state.busyWith.clear();
      				state.networkOutMsg.clear();
      				state.speakerOutMsg.clear();
	          	state.speakerOutMsg.emplace_back(x);
      			}else if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
      				//The phone is busy. Ignore. We do not acknowledge that the phone is busy
      			}else{
					    throw std::logic_error("Message not allowed in CALL_REQUESTED state. External phone networkIn");       
      			}
      			break;
      		case PhoneState::INCOMING_CALL_REQUESTED:
      			if(x.msg_phone == PhoneMsgType::CALL_OVER || x.msg_phone == PhoneMsgType::NET_OUT_OF_ORDER){
      				state.state = PhoneState::IDLE;
      				state.busyWith.clear();
      				state.networkOutMsg.clear();
      				state.speakerOutMsg.clear();
	          	state.speakerOutMsg.emplace_back(x);
      			}else if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
      				//The phone is busy. Ignore. We do not acknowledge that the phone is busy
      			}else{
					    throw std::logic_error("Message not allowed in INCOMING_CALL_REQUESTED state. External phone networkIn");       
      			}
        		break;
      		case PhoneState::COMMUNICATION_STABLISHED:
      		case PhoneState::COMMAND_TRANSMITTED:
        		if(x.msg_phone == PhoneMsgType::COMMAND_TRANSMITTED){
        			state.state = PhoneState::COMMAND_TRANSMITTED;              						
        		}else if(x.msg_phone == PhoneMsgType::CALL_OVER || x.msg_phone == PhoneMsgType::NET_OUT_OF_ORDER){
      				state.state = PhoneState::COMMUNICATION_OVER;
      				state.busyWith.clear();
      				state.networkOutMsg.clear();
      				state.speakerOutMsg.clear();				            	
      			}else if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
      				//The phone is busy. Ignore. We do not acknowledge that the phone is busy
      			}else{
      				throw std::logic_error("Message not allowed in COMMUNICATION_STABLISHED or COMMAND_TRANSMITED state. External phone networkIn");
         		}
            state.speakerOutMsg.emplace_back(x);
        		break;
      		case PhoneState::COMMUNICATION_OVER:
      			if(x.msg_phone == PhoneMsgType::CALL_REQUEST){
      				//The phone is busy. Ignore. We do not acknowledge that the phone is busy
      			}else if(x.msg_phone == PhoneMsgType::CALL_OVER || x.msg_phone == PhoneMsgType::NET_OUT_OF_ORDER){
      				//The communication is already over
      			}else{
      				throw std::logic_error("Message not allowed in COMMUNICATION_OVER state. External phone networkIn");
         		}
        		break;
      		case PhoneState::OUT_OF_ORDER:     
        		state.networkOutMsg.emplace_back(Communication(MessageType::PHONE_MESSAGE,PhoneMsgType::CALL_OVER, id, x.from));             
        		break;
      		default: assert(false && "Not that state"); break;
    		}      
			}
      //TimeAdvance Setting
    	if(!state.networkOutMsg.empty()||!state.speakerOutMsg.empty()) state.advanceTime = timeProcessing;
      else if(state.state == PhoneState::CALL_REQUESTED) state.advanceTime = aux_time;
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
      for (int i = 0; i < state.networkOutMsg.size(); i++){
				get_messages<typename defs::networkOut>(bags).push_back(state.networkOutMsg[i]);      
		  }   
		  for (int i = 0; i < state.speakerOutMsg.size(); i++){
				get_messages<typename defs::speakerOut>(bags).push_back(state.speakerOutMsg[i]);      
		  }
      return bags;
    }
    // time_advance function
    TIME time_advance() const {
      return state.advanceTime;
    }
    // << operator fot the state
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename DevicePhone<TIME>::state_type& i) {
      os << "next time: " << i.advanceTime << " networkOutMsg: " << i.networkOutMsg.size() << " speakerOutMsg: " << i.speakerOutMsg.size(); 
      return os;
    }
};
#endif //CADMIUM_DEVICE_PHONE_HPP