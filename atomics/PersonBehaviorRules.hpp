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

/**
* Decision Maker in Cadmium
* Choose between answer a communication request, send a communication or do an action
* It is only valid for doing one task at a time
* choose_com_channel auxiliary function. Only mobile, landline, email and beeper implemented
*/

#ifndef CADMIUM_DECISION_MAKER_HPP
#define CADMIUM_DECISION_MAKER_HPP
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>

using namespace std;
using namespace cadmium;

#include "../data_structures/decision_maker_behaviour_struct_types_V2.hpp"
#include "../data_structures/nep_model_enum_types.hpp"
#include "../data_structures/communication.hpp"
#include "../data_structures/struct_types.hpp"

using namespace decision_maker_behaviour_structures;
using namespace decision_maker_behaviour_enum_types;
using namespace nep_model_enum_types;
using namespace nep_structures;

//Port definition
  struct PersonBehaviorRules_defs{
    struct generatorIn : public in_port<Command> {
    };
    struct requestIn : public in_port<Communication> {
    };
    struct taskDeviceFinishedIn : public in_port<TaskDeviceFinished> {
    };
    struct taskActionFinishedIn : public in_port<ActionFinished> {
    };
    struct peopleInSameLocationIn : public in_port<PeopleClassifiedByLocation> {
    };
    struct  taskSendOutInperson: public out_port<CommandWithDevice> {
    };
    struct  taskSendOutDevice: public out_port<CommandWithDevice> {
    };
    struct  taskAnswerOutInperson: public out_port<Communication> {
    };
    struct  taskAnswerOutDevice: public out_port<Communication> {
    };
    struct  taskDoActionOut: public out_port<Action2Do> {
    };      
  };
//This is a meta-model, it should be overloaded for declaring the Id parameter
  template<typename TIME>
  class PersonBehaviorRules {
    using defs=PersonBehaviorRules_defs; // putting definitions in context 

  public:
    enum Task{ANSWER, SEND, DO_ACTION, IDLE};
    // state definition
    struct state_type{
      //PARAMETERS
      DecisionMakerBehaviour                behaviour;
      //DECISION VARIABLES
      vector<pair<DeviceId,Communication>>  answerRequests; //DeviceId(My device + who is requesting)
      vector<Msg2Send>                      commands2Send;
      vector<Msg2Send>                      commands2SendDeviceNotAvailable;
      vector<ActionBehaviour>               actions2Do;
      vector<tuple<DeviceId,Task,Communication,CommandWithDevice,Action2Do>>      taskState; //MyDeviceId OR IDLE (in the case of DO_ACTION)
      vector<tuple<Confirmation,Msg2Send>>  confirmationTrack;
      vector<DeviceType>                    devicesBusy; //Only the ones that cannot receive and send at the same time
      //OTHER VARIABLES
      bool                                  makeNewDecision;
      bool                                  newTaskChoosen;
      vector<tuple<DeviceId,Task,Communication,CommandWithDevice,Action2Do>>               choosenTask; //MyDeviceId
      TIME                                  advanceTime;
      //TRACKING VARIABLES
      vector<Command>                       commandsReceived;
      vector<ActionBehaviour>               actionsDone;
      vector<Msg2Send>                      commandsSent;
      vector<DeviceType>                    devicesOutOfOrder;   
    }; 

    state_type state;
  
    //default constructor  
    PersonBehaviorRules(string file_name_in) noexcept {
      const char * in = file_name_in.c_str();
      state.behaviour.load(in);
      #ifdef DEBUG_MODE
        string debugname = string("DEBUG_DECISION_MAKER_PARAMETERS_")+state.behaviour.id+(string(".xml"));      
        const char * out = debugname.c_str();
        state.behaviour.save(out);
      #endif
      state.advanceTime = std::numeric_limits<TIME>::infinity();
      state.makeNewDecision = true;
      state.newTaskChoosen = false;
    }

    // ports definition
    using input_ports=std::tuple<typename defs::peopleInSameLocationIn, typename defs::generatorIn, typename defs::requestIn, typename defs::taskDeviceFinishedIn, typename defs::taskActionFinishedIn>;
    using output_ports=std::tuple<typename defs::taskSendOutInperson, typename defs::taskSendOutDevice, typename defs::taskAnswerOutInperson, typename defs::taskAnswerOutDevice, typename defs:: taskDoActionOut>;

  // internal transition
  void internal_transition() {
    /***********************************************************************************/
    /*If the choosen task is answer, delete the choosen task from answer answerRequests*/
    /***********************************************************************************/
      if(get<Task>(state.choosenTask[0]) == Task::ANSWER){    
        Communication requestAnswered = get<Communication>(state.choosenTask[0]);
        for(int i = 0; i < state.answerRequests.size(); i++){
          if(requestAnswered == get<Communication>(state.answerRequests[i])) {
            state.answerRequests.erase(state.answerRequests.begin()+i);
            break;
          }
        }
      }
    /********************/
    /* Update TaskState */
    /********************/
      switch(get<Task>(state.choosenTask[0])){
      case Task::ANSWER:
      case Task::SEND:        
      case Task::DO_ACTION:
        state.taskState.push_back(state.choosenTask[0]);break;
      default: assert(false && ("The internal should not arrive this point")); break;
      }    
    /*******************/
    /*Clear choosenTask*/
    /*******************/
      state.choosenTask.clear();
    /*****************************/
    /*Set make new decision false*/
    /*****************************/
      state.makeNewDecision = false;
      state.newTaskChoosen = false;
    /*********************/
    /*Passivate the model*/
    /*********************/
      state.advanceTime = std::numeric_limits<TIME>::infinity();
  }
  //external
  void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
    //cout << "EXTERNAL BEHAVIOUR RULES " << state.behaviour.id << " ";
    /***********************************************************/
    /* Process the inputs. Update decision & tacking variables */
    /***********************************************************/    
     //generatorIn
    for (const auto &x : get_messages<typename defs::generatorIn>(mbs)){
      //cout << "generator in" << x << endl;
      /**Update commands2Send**/
       vector<Msg2Send> newCommands2Send = extract_commnads_to_send_from_command(x, state.behaviour);
       for(int z = 0; z<newCommands2Send.size(); z++){
         state.commands2Send.push_back(newCommands2Send[z]);
       }
       /**Update actions2Do**/
       vector<ActionBehaviour> newActions2Do = extract_actions_to_do_from_command(x, state.behaviour);
       for(int z = 0; z<newActions2Do.size(); z++){
         state.actions2Do.push_back(newActions2Do[z]);
       }
    }
    //requestIn (new request or request over)
    for (const auto &x : get_messages<typename defs::requestIn>(mbs)) {
      switch(x.type){
        case MessageType::PHONE_MESSAGE:
          switch (x.msg_phone){
            case PhoneMsgType::CALL_REQUEST:
              {
              /** Add answer request to list **/
              state.answerRequests.push_back(make_pair(DeviceId(x.to.type, x.from.id), x));
              /** if device cannot send and recieve at the same time, add to the list of busy devices*/
              bool isdeviceBusy = is_device_busy(x.to.type, state.devicesBusy);
              if(isdeviceBusy) state.devicesBusy.push_back(x.to.type);
              }
              break;
            case PhoneMsgType::CALL_OVER:
              /** Delete answer request over from the list **/
              for(int j = 0; j < state.answerRequests.size(); j++){
                if(state.answerRequests[j].first == DeviceId(x.to.type, x.from.id)){
                  state.answerRequests.erase(state.answerRequests.begin()+j);
                  break;
                }
              }
              /** If device in the list of busy devices, remove from it**/
              for(int j=0; j<state.devicesBusy.size(); j++){
                if(x.to.type == state.devicesBusy[j]){
                  state.devicesBusy.erase(state.devicesBusy.begin()+j);
                  break;
                }
              }
              break;
            case PhoneMsgType::OUT_OF_ORDER:
              /** Delete answer request over from the list **/
              for(int j = 0; j < state.answerRequests.size(); j++){
                if(state.answerRequests[j].first == DeviceId(x.to.type, x.from.id)){
                  state.answerRequests.erase(state.answerRequests.begin()+j);
                  break;
                }
              }
              /** If device in the list of busy devices, remove from it**/
              for(int j=0; j<state.devicesBusy.size(); j++){
                if(x.to.type == state.devicesBusy[j]){
                  state.devicesBusy.erase(state.devicesBusy.begin()+j);
                  break;
                }
              }
              break;
            default:assert(false&&"This PhoneMsgType is not allowed in this model"); break;
          }
          break;
        case MessageType::TEXT_MESSAGE:
          switch (x.msg_text){
            case TextMsgType::NEW:
              { 
              /** Add answer request to list **/
              state.answerRequests.push_back(make_pair(DeviceId(x.to.type, x.from.id), x));
              /** if device cannot send and recieve at the same time, add to the list of busy devices*/
              bool isdeviceBusy = is_device_busy(x.to.type, state.devicesBusy);
              if(isdeviceBusy) state.devicesBusy.push_back(x.to.type);
              }
              break;
            case TextMsgType::OUT_OF_ORDER:
              /** Delete answer request over from the list **/
              for(int j = 0; j < state.answerRequests.size(); j++){
                if(state.answerRequests[j].first == DeviceId(x.to.type, x.from.id)){
                  state.answerRequests.erase(state.answerRequests.begin()+j);
                  break;
                }
              }
              /** If device in the list of busy devices, remove from it**/
              for(int j=0; j<state.devicesBusy.size(); j++){
                if(x.to.type == state.devicesBusy[j]){
                  state.devicesBusy.erase(state.devicesBusy.begin()+j);
                  break;
                }
              }
              break;
            default:assert(false&&"This TextMsgType is not allowed in this model"); break;
          }
          break;
        case MessageType::RADIO_MESSAGE:
          switch (x.msg_radio){
            case RadioMsgType::REQUEST:
              {
              /** Add answer request to list **/
              state.answerRequests.push_back(make_pair(DeviceId(x.to.type, x.from.id), x));
              /** if device cannot send and recieve at the same time, add to the list of busy devices*/
              bool isdeviceBusy = is_device_busy(x.to.type, state.devicesBusy);
              if(isdeviceBusy) state.devicesBusy.push_back(x.to.type);
              }
              break;
            case RadioMsgType::COMMAND_TRANSMITTED:
              /** Delete answer request over from the list **/
              for(int j = 0; j < state.answerRequests.size(); j++){
                if(state.answerRequests[j].first == DeviceId(x.to.type, x.from.id)){
                  state.answerRequests.erase(state.answerRequests.begin()+j);
                  break;
                }
              }
              /** If device in the list of busy devices, remove from it**/
              for(int j=0; j<state.devicesBusy.size(); j++){
                if(x.to.type == state.devicesBusy[j]){
                  state.devicesBusy.erase(state.devicesBusy.begin()+j);
                  break;
                }
              }
              break;
            case RadioMsgType::INTERFERENCES:
              /** Delete answer request over from the list **/
              for(int j = 0; j < state.answerRequests.size(); j++){
                if(state.answerRequests[j].first == DeviceId(x.to.type, x.from.id)){
                  state.answerRequests.erase(state.answerRequests.begin()+j);
                  break;
                }
              }
              /** If device in the list of busy devices, remove from it**/
              for(int j=0; j<state.devicesBusy.size(); j++){
                if(x.to.type == state.devicesBusy[j]){
                  state.devicesBusy.erase(state.devicesBusy.begin()+j);
                  break;
                }
              }
              break;
            case RadioMsgType::OUT_OF_ORDER:
              /** Delete answer request over from the list **/
              for(int j = 0; j < state.answerRequests.size(); j++){
                if(state.answerRequests[j].first == DeviceId(x.to.type, x.from.id)){
                  state.answerRequests.erase(state.answerRequests.begin()+j);
                  break;
                }
              }
              /** If device in the list of busy devices, remove from it**/
              for(int j=0; j<state.devicesBusy.size(); j++){
                if(x.to.type == state.devicesBusy[j]){
                  state.devicesBusy.erase(state.devicesBusy.begin()+j);
                  break;
                }
              }
              break;
            default:assert(false&&"This RadioMsgType is not allowed in this model"); break;
          }
          break;
        default: assert(false && "MessageType to implement"); break;
      }      
    }
    //taskDeviceFinishedIn
    for (const auto &x : get_messages<typename defs::taskDeviceFinishedIn>(mbs)){
      //cout << "taskDeviceFinishedIn in" << x << endl;
     // cout << "BR - taskDeviceFinishedIn" << endl;
      /** Update the state of devices out of order **/
        if(x.out_of_order){
          for(int j = 0; j < state.behaviour.myDevices.size(); j++){
            if(x.my_id.type == state.behaviour.myDevices[j].device){
              state.behaviour.myDevices.erase(state.behaviour.myDevices.begin()+j);
              state.devicesOutOfOrder.push_back(x.my_id.type);
              break;
            }
          }
        }
      /** Process the command received/sent & update the commands2Send, actions2Do, confirmationTrack and other variables**/
       // cout << "BR - taskDeviceFinishedIn - taskstate size: " << state.taskState.size() << endl; 
        for(int j =0; j < state.taskState.size(); j++){
         // cout << "x.my_id: " << x.my_id << " | DeviceId task state: " << get<DeviceId>(state.taskState[j]) << endl; 
          if(x.my_id == get<DeviceId>(state.taskState[j])){
           // cout << "BR entra en x.makeNewDecision == get DeviceIdTask" << endl;
            switch (get<Task>(state.taskState[j])){
              case Task::ANSWER:
               // cout << "BR - taskDeviceFinishedIn ANSWER" << endl;
                if(x.command.command != string("-")){
                 // cout << "BR - taskDeviceFinishedIn ANSWER - command not -" << x.command.command <<  endl;
                  /**Update commands2Send**/
                  vector<Msg2Send> newCommands2Send = extract_commnads_to_send_from_command(x.command, state.behaviour);
                  for(int z = 0; z<newCommands2Send.size(); z++){
                    state.commands2Send.push_back(newCommands2Send[z]);
                  }
                  /**Update actions2Do**/
                  vector<ActionBehaviour> newActions2Do = extract_actions_to_do_from_command(x.command, state.behaviour);
                  for(int z = 0; z<newActions2Do.size(); z++){
                    state.actions2Do.push_back(newActions2Do[z]);
                  }
                  /**Update confirmationTrack**/
                  for(int z = 0; z<state.confirmationTrack.size(); z++){
                    if(x.command.command == get<Confirmation>(state.confirmationTrack[z]).content && x.command.from == get<Confirmation>(state.confirmationTrack[z]).from){
                      state.confirmationTrack.erase(state.confirmationTrack.begin()+z);
                      break;
                    }
                  }
                  /**Track command received**/
                  state.commandsReceived.push_back(x.command);
                  //cout << "new ammount of commnads to send: " << state.commands2Send.size() << endl;
                }
                break;
              case Task::SEND:
                //cout << "BR - taskDeviceFinishedIn SEND" << endl;
                if(x.command.command != string("-")){
                 // cout << "BR - taskDeviceFinishedIn SEND - command not -" << x.command.command <<  endl;
                  Msg2Send auxCommandSent = extract_command_just_sent(x.command, state.commands2Send);
                 // cout << auxCommandSent.to << " " << auxCommandSent.content << endl;
                  /**Update commands2Send**/
                 // cout <<  state.commands2Send.size() << endl;                  
                  for(int z = 0; z < state.commands2Send.size(); z++){             
                    if(auxCommandSent.equal(state.commands2Send[z])){
                     // cout << auxCommandSent.equal(state.commands2Send[z]) << endl;
                      state.commands2Send.erase(state.commands2Send.begin()+z);
                      break;
                    }                    
                  }
                 // cout << "new ammount of commnads to send: " << state.commands2Send.size() << endl; 
                  /**Update commandsSent**/
                  state.commandsSent.push_back(auxCommandSent);
                  /**Update confirmationTrack**/
                  vector<Confirmation> auxConfirmations2Receive = auxCommandSent.confirmations;
                  for(int z = 0; z < auxConfirmations2Receive.size(); z++){
                    state.confirmationTrack.push_back(make_tuple(auxConfirmations2Receive[z], auxCommandSent));
                  }            
                }              
                break;
              default: assert(false && "Task not defined for devices"); break;
            }
            /** Delete the task finished from current tasks **/
            state.taskState.erase(state.taskState.begin()+j);
            //cout << "taskState size after procesing task finished: " << state.taskState.size() << endl;
            break;
          }
        }
    }
    //taskActionFinishedIn 
    for (const auto &x : get_messages<typename defs::taskActionFinishedIn>(mbs)){
      /** Update Location **/
        state.behaviour.location = x.location;
      /** Extract action done **/
        ActionBehaviour auxActionDone = extract_action_done_from_actions2Do(x.action_id, state.actions2Do);
      /** Add action done to actionsDone **/
        state.actionsDone.push_back(auxActionDone);
      /** Delete action done from actions2Do **/
        for(int j = 0; state.actions2Do.size(); j++){
          if(state.actions2Do[j].equal(auxActionDone)){
            state.actions2Do.erase(state.actions2Do.begin()+j);
            break;
          }
        }
      /** Add new Msg2Send from the action done to command2send**/
        vector<Msg2Send> auxNewMsg2Send = auxActionDone.msg2send;
        for(int j = 0; auxNewMsg2Send.size(); j++){
          state.commands2Send.push_back(auxNewMsg2Send[j]);
        }
      /** Delete the task finished from current tasks **/
        //We are assuming that we can only do one task at a time
        for(int j =0; j < state.taskState.size(); j++){
          if(get<Task>(state.taskState[j]) == Task::DO_ACTION){
            state.taskState.erase(state.taskState.begin()+j);
            break;
          }
        }
    }
    //peopleInSameLocationIn 
    for (const auto &x : get_messages<typename defs::peopleInSameLocationIn>(mbs)){
      /**Update people in same location**/
      state.behaviour.peopleSameLocation = x.ids;
    }
   
    /***********************************/
    /* Update makeNewDecision variable */
    /***********************************/    
      /**If I am not doing any task set make new decision = true **/
      if(state.taskState.empty()) state.makeNewDecision = true;
      //cout << "make new decision: " << state.makeNewDecision  << endl;
    /**************************************************/
    /* If makeNewDecision = true, make a new decision */
    /**************************************************/    
      tuple<DeviceId,Task,Communication,CommandWithDevice,Action2Do>   newChoosenTask;
      if(state.makeNewDecision == true){
        state.newTaskChoosen = false;
        state.choosenTask.clear();
        for(int i = 0; i < state.behaviour.taskPrioritized.size(); i++){
          vector<TaskPriorityType> taskWsamePriority = extract_task_with_priority_j((i+1), state.behaviour);
          for(int j = 0; j < taskWsamePriority.size(); j++){
            switch(taskWsamePriority[j]){
              case TaskPriorityType::ANSWER: /** Do Task AnswerDevice if any answer request **/
               // cout << "chosing task answer" << endl;
                if(!state.answerRequests.empty()){
                  if(state.answerRequests.size() > 1){
                    vector<pair<DeviceId,Communication>>   aux_answerRequests = state.answerRequests;
                    switch(state.behaviour.answerPriority){
                      case AnswerPriorityType::DEVICE_PRIORITY: //Answer behaviour if the answer priority is based on devices first
                        aux_answerRequests = extract_requests_with_same_device_priority(aux_answerRequests, state.behaviour);
                        if(aux_answerRequests.size()>1){
                          aux_answerRequests = extract_requests_with_same_person_priority(aux_answerRequests, state.behaviour);
                          if(aux_answerRequests.size()>1){
                            #ifdef DEBUG_MODE
                              newChoosenTask = make_tuple(DeviceId(aux_answerRequests[0].first.type, state.behaviour.id), Task::ANSWER, aux_answerRequests[0].second,CommandWithDevice(),Action2Do());
                              state.newTaskChoosen = true;                             
                            #else
                              unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // seed variable is used to ensure a different set of results for each simulation
                              srand (seed);
                              int a = rand() % (aux_answerRequests.size()-1);
                              newChoosenTask = make_tuple(DeviceId(aux_answerRequests[a].first.type, state.behaviour.id), Task::ANSWER, aux_answerRequests[a].second,CommandWithDevice(),Action2Do());
                              state.newTaskChoosen = true;
                            #endif
                          }else{
                            newChoosenTask = make_tuple(DeviceId(aux_answerRequests[0].first.type, state.behaviour.id), Task::ANSWER, aux_answerRequests[0].second,CommandWithDevice(),Action2Do());
                            state.newTaskChoosen = true;
                          }
                        }else{
                          newChoosenTask = make_tuple(DeviceId(aux_answerRequests[0].first.type, state.behaviour.id), Task::ANSWER, aux_answerRequests[0].second,CommandWithDevice(),Action2Do());
                          state.newTaskChoosen = true;
                        }
                        break;//case DEVICE_PRIORITY
                      case AnswerPriorityType::PERSON_PRIORITY: //Answer behaviour if the answer priority is based on persons first
                        aux_answerRequests = extract_requests_with_same_person_priority(aux_answerRequests, state.behaviour);
                          if(aux_answerRequests.size()>1){
                            aux_answerRequests = extract_requests_with_same_device_priority(aux_answerRequests, state.behaviour);
                            if(aux_answerRequests.size()>1){
                              #ifdef DEBUG_MODE
                                newChoosenTask = make_tuple(aux_answerRequests[0].first, Task::ANSWER, aux_answerRequests[0].second,CommandWithDevice(),Action2Do());
                                state.newTaskChoosen = true;
                              #else
                                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // seed variable is used to ensure a different set of results for each simulation
                                srand (seed);
                                int a = rand() % (aux_answerRequests.size()-1);
                                newChoosenTask = make_tuple(DeviceId(aux_answerRequests[a].first.type, state.behaviour.id), Task::ANSWER, aux_answerRequests[a].second,CommandWithDevice(),Action2Do());
                                state.newTaskChoosen = true;
                              #endif
                            }else{
                              newChoosenTask = make_tuple(DeviceId(aux_answerRequests[0].first.type, state.behaviour.id), Task::ANSWER, aux_answerRequests[0].second,CommandWithDevice(),Action2Do());
                              state.newTaskChoosen = true;
                            }
                          }else{
                            newChoosenTask = make_tuple(DeviceId(aux_answerRequests[0].first.type, state.behaviour.id), Task::ANSWER, aux_answerRequests[0].second,CommandWithDevice(),Action2Do());
                            state.newTaskChoosen = true;
                          }
                          break; //break PERSON_PRIORITY
                      default: assert(false && "AnswerPriority not implemented"); break;
                    }     
                  }else{
                    newChoosenTask = make_tuple(DeviceId(state.answerRequests[0].first.type, state.behaviour.id), Task::ANSWER, state.answerRequests[0].second,CommandWithDevice(),Action2Do());
                    state.newTaskChoosen = true;
                  }                         
                }
                break;//break ANSWER
              case TaskPriorityType::SEND: /** Do Task SendCommand if any command to send **/
                if(!state.commands2Send.empty()){
                  pair<DeviceType, DeviceType>  com_chan; //from, to
                  vector<Msg2Send>              commandsWithSamePriority;      
                  Msg2Send                      commandSelected;
                  switch(state.behaviour.sendPriority){
                    case SendPriorityType::RECEPTION_ORDER: //Send commands based on reception order
                      while(!state.commands2Send.empty()){
                        com_chan = choose_com_channel(state.commands2Send[0], state.behaviour);
                        if(com_chan.first != DeviceType::IDLE){
                         // cout << "SEND - RECEPTION ORDER NOT IDLE" << endl;
                          bool isdeviceBusy = is_device_busy(com_chan.first, state.devicesBusy);
                          if(!isdeviceBusy){
                            newChoosenTask = make_tuple(DeviceId(com_chan.first, state.behaviour.id),Task::SEND, Communication(),CommandWithDevice(DeviceId(com_chan.first, state.behaviour.id), DeviceId(com_chan.second, state.commands2Send[0].to), Command(state.behaviour.id, state.commands2Send[0].to, state.commands2Send[0].content)),Action2Do());
                            state.newTaskChoosen = true;                      
                          }//if device is busy, I go to the next task answer or do action. I do not look for other commands to send.
                          //if device is busy it means that, at least, I have an answer request.
                          break;//break while
                        }else{ //if device not working, I forguet about the compulsory channel restriction
                         // cout << "SEND - RECEPTION ORDER IDLE" << endl;
                          state.commands2SendDeviceNotAvailable.push_back(state.commands2Send[0]);                          
                          if(!state.commands2Send[0].deviceCompulsory.empty()){
                            state.commands2Send[0].deviceCompulsory.clear();
                            state.commands2Send.push_back(state.commands2Send[0]);
                          }                          
                          state.commands2Send.erase(state.commands2Send.begin());
                        }
                      }
                      break;//Break send based on reception order
                    case SendPriorityType::INVERSE_RECEPTION_ORDER://Send commands based on inverse reception order
                      while(!state.commands2Send.empty()){
                        int a = state.commands2Send.size()-1;
                        com_chan = choose_com_channel(state.commands2Send[a], state.behaviour);
                        if(com_chan.first != DeviceType::IDLE){
                          bool isdeviceBusy = is_device_busy(com_chan.first, state.devicesBusy);
                          if(!isdeviceBusy){
                            newChoosenTask = make_tuple(DeviceId(com_chan.first, state.behaviour.id),Task::SEND, Communication(), CommandWithDevice(DeviceId(com_chan.first, state.behaviour.id), DeviceId(com_chan.second, state.commands2Send[a].to), Command(state.behaviour.id, state.commands2Send[a].to, state.commands2Send[a].content)),Action2Do());
                            state.newTaskChoosen = true;                        
                          }  //if device is busy, I go to the next task answer or do action. I do not look for other commands to send                      
                          //if device is busy it means that, at least, I have an answer request.
                          break;//break while
                        }else{//if device not working, I forguet about the compulsory channel restriction
                          state.commands2SendDeviceNotAvailable.push_back(state.commands2Send[a]);                          
                          if(!state.commands2Send[a].deviceCompulsory.empty()){
                            state.commands2Send[a].deviceCompulsory.clear();
                            state.commands2Send.insert(state.commands2Send.begin(),state.commands2Send[a]);
                          }                          
                          state.commands2Send.pop_back();
                        }
                      }
                      break;//break send commands inverse reception order
                    case SendPriorityType::PRIORITY_LIST: 
                      while(!state.commands2Send.empty()){
                        commandsWithSamePriority = extract_commands_with_same_pripority(state.commands2Send, state.behaviour);
                        if(commandsWithSamePriority.size()>1){
                        #ifdef DEBUG_MODE
                          commandSelected = commandsWithSamePriority[0];
                        #else
                          unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // seed variable is used to ensure a different set of results for each simulation
                          srand (seed);
                          int a = rand() % (commandsWithSamePriority.size()-1);
                          commandSelected = commandsWithSamePriority[a];
                        #endif
                        }else{
                          commandSelected = commandsWithSamePriority[0];
                        }
                        com_chan = choose_com_channel(commandSelected, state.behaviour);
                        if(com_chan.first != DeviceType::IDLE){
                          bool isdeviceBusy = is_device_busy(com_chan.first, state.devicesBusy);
                          if(!isdeviceBusy){
                            newChoosenTask = make_tuple(DeviceId(com_chan.first, state.behaviour.id),Task::SEND, Communication(), CommandWithDevice(DeviceId(com_chan.first, state.behaviour.id), DeviceId(com_chan.second, commandSelected.to), Command(state.behaviour.id, commandSelected.to, commandSelected.content)),Action2Do());
                            state.newTaskChoosen = true;
                          }//if device is busy, I go to the next task answer or do action. I do not look for other commands to send                           
                          //if device is busy it means that, at least, I have an answer request.
                          break; //break while
                        }else{//if device not working, I forguet about the compulsory channel restriction
                          for(int z= 0; z < state.commands2Send.size(); z++){
                            if(state.commands2Send[z].equal(commandSelected)){                              
                              state.commands2SendDeviceNotAvailable.push_back(state.commands2Send[z]);
                              Msg2Send aux;
                              if(!state.commands2Send[z].deviceCompulsory.empty()){
                                state.commands2Send[z].deviceCompulsory.clear();
                                aux = state.commands2Send[z];
                              } 
                              state.commands2Send.erase(state.commands2Send.begin()+z);
                              if(aux.equal(state.commands2SendDeviceNotAvailable.back())) state.commands2Send.push_back(aux);
                              break; //break for z
                            }
                          }
                        }
                      }
                      break; //break send commands based on priority list
                    default: assert(false && "Priority not implemented"); break;
                  }
                }
                //TO_IMPLEMENT: if in person or phoneMultiple - selected all the people that I need to send the message through that channel
                break;//break SEND
              case TaskPriorityType::DO_ACTION: /** Do an action if any action to do **/
                if(!state.actions2Do.empty()){
                  vector<ActionBehaviour>    actionsWithSamePriority = extract_actions_with_same_pripority(state.actions2Do, state.behaviour);
                  ActionBehaviour            actionSelected;
                  if(actionsWithSamePriority.size()>1){
                  #ifdef DEBUG_MODE
                    actionSelected = actionsWithSamePriority[0];
                  #else
                    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // seed variable is used to ensure a different set of results for each simulation
                    srand (seed);
                    int a = rand() % (actionsWithSamePriority.size()-1);
                    actionSelected = actionsWithSamePriority[a];
                  #endif
                  }else{
                    actionSelected = actionsWithSamePriority[0];
                  }
                  newChoosenTask = make_tuple(DeviceId(DeviceType::IDLE, state.behaviour.id), Task::DO_ACTION, Communication(), CommandWithDevice(), Action2Do(actionSelected.id, actionSelected.averageExectionTime, actionSelected.location));
                  state.newTaskChoosen = true;                    
                }
                break;//break DO_ACTION
              default: assert(false && "Task not implemented"); break;
            }
            if(state.newTaskChoosen == true) break; //break for j
          }
          if(state.newTaskChoosen == true) break; //break for i
        }
      }

    //*************************************************/
    /*** Set advance time & update choosen task ******/
    /*************************************************/
      if(state.makeNewDecision == false){
        state.advanceTime = std::numeric_limits<TIME>::infinity();
      }else{
        if(state.newTaskChoosen == true){
          if(!state.choosenTask.empty()){
            if(newChoosenTask == state.choosenTask[0]) state.advanceTime = state.advanceTime - e;
          }else{
            state.choosenTask.push_back(newChoosenTask);
            state.advanceTime = TIME(state.behaviour.reactionTime);
          }
        }else{
          state.advanceTime = std::numeric_limits<TIME>::infinity();
        }      
      } 


  }

  // confluence transition
  void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
      cout << "**** CONFLUENCE PERSON BEHAVIOR RULES ************  " << state.behaviour.id << endl;
      internal_transition();
      external_transition(TIME(), std::move(mbs));
  }

  // output function
  typename make_message_bags<output_ports>::type output() const { 
    //cout << "OUTPUT " << state.behaviour.id ;
    typename make_message_bags<output_ports>::type bags;
    switch(get<Task>(state.choosenTask[0])){
      case Task::ANSWER:
        if(get<Communication>(state.choosenTask[0]).to.type == DeviceType::IN_PERSON){
          get_messages<typename defs::taskAnswerOutInperson>(bags).push_back(get<Communication>(state.choosenTask[0])); 
        }else{
          get_messages<typename defs::taskAnswerOutDevice>(bags).push_back(get<Communication>(state.choosenTask[0])); 
        }
       break;
      case Task::SEND:
        if(get<CommandWithDevice>(state.choosenTask[0]).from.type == DeviceType::IN_PERSON){
          get_messages<typename defs::taskSendOutInperson>(bags).push_back(get<CommandWithDevice>(state.choosenTask[0])); 
        }else{
          get_messages<typename defs::taskSendOutDevice>(bags).push_back(get<CommandWithDevice>(state.choosenTask[0])); 
        }
        break;
      case Task::DO_ACTION: get_messages<typename defs::taskDoActionOut>(bags).push_back(get<Action2Do>(state.choosenTask[0])); break;
      case Task::IDLE: break;
      default: assert(false && "the task state is not implemented or does not exists"); break;
    }

    //switch(get<Task>(state.choosenTask[0])){
    //  case Task::ANSWER: 
    //    cout << get<Communication>(state.choosenTask[0]);
    //    break;
    //  case Task::SEND:
    //    cout << get<CommandWithDevice>(state.choosenTask[0]);
    //    break;
    //  case Task::DO_ACTION:
    //    cout << get<Action2Do>(state.choosenTask[0]); break;
    //  case Task::IDLE: break;
    //  default: assert(false && "the task state is not implemented or does not exists"); break;
    //}

    return bags;
  }

  // time_advance function
  TIME time_advance() const {
    return state.advanceTime;
  }


  friend std::ostringstream& operator<<(std::ostringstream& os, const typename PersonBehaviorRules<TIME>::state_type& i) {
    os << "advanceTime: "<< i.advanceTime << " answerRequests: " << i.answerRequests.size() << " commands2Send " << i.commands2Send.size() << ": ";
    for (int j = 0; j < i.commands2Send.size(); j++){
      os << i.commands2Send[j].to << " " << i.commands2Send[j].content << ";";
    } 
    return os;
  }
  /*****************************/
  /**** AUXILIARY FUNCTIONS ****/
  /*****************************/
    bool is_device_busy(DeviceType device, const vector<DeviceType> & devicesBusy){
      bool isBusy = false;
      for(int i=0; i<devicesBusy.size();i++){
        if(device == devicesBusy[i]){
          isBusy = true;
          break;
        }
      }
      return isBusy;
    }

    vector<Msg2Send> extract_commnads_to_send_from_command(const Command & command, const DecisionMakerBehaviour& behaviour){
      vector<Msg2Send> auxcommands2Send;
      vector<Msg2Send> commands2Send;
      for(int i = 0; i<behaviour.msgBehaviour.size(); i++){
        if(command.command == behaviour.msgBehaviour[i].content && command.from == behaviour.msgBehaviour[i].from){
          auxcommands2Send = behaviour.msgBehaviour[i].msg2send;
          break;
        }
      }
      /** Decide if send or not the ones that are not compulsory **/
      /** Send with 50% probability when they are not compulsory **/
      for(int i = 0; i<auxcommands2Send.size(); i++){
        if(auxcommands2Send[i].compulsory){
          commands2Send.push_back(auxcommands2Send[i]);
        }else{
          bool send;
          #ifdef SEND_NEVER_OPTIONAL
            send = false;
          #elif SEND_ALWAYS_OPTIONAL
            send = true;
          #else
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // seed variable is used to ensure a different set of results for each simulation
            srand (seed);
            int a = rand() % 1;
            (a == 0) ? send = false : send = true;
          #endif
          if(send) commands2Send.push_back(auxcommands2Send[i]);
        }
      }    
      return commands2Send;
    }
  
    vector<ActionBehaviour> extract_actions_to_do_from_command(const Command & command, const DecisionMakerBehaviour& behaviour){
      vector<string> auxActions2Do;
      vector<ActionBehaviour> actions2Do;
      for(int i = 0; i<behaviour.msgBehaviour.size(); i++){
        if(command.command == behaviour.msgBehaviour[i].content && command.from == behaviour.msgBehaviour[i].from){
          auxActions2Do = behaviour.msgBehaviour[i].actions2Do;
          break;
        }
      }
      for(int i = 0; i<auxActions2Do.size(); i++){
        for(int j=0; j<behaviour.actionBehaviour.size(); j++){
          if(auxActions2Do[i] == behaviour.actionBehaviour[j].id){
            actions2Do.push_back(behaviour.actionBehaviour[j]);
            break;
          }
        }
      }  
      return actions2Do;
    }
  
    Msg2Send extract_command_just_sent(const Command & command, const vector<Msg2Send> & commands2Send){
      Msg2Send commandSent;
      for(int i = 0; i<commands2Send.size(); i++){
        if(command.command == commands2Send[i].content && command.to == commands2Send[i].to){
          commandSent = commands2Send[i];
          break;
        }
      }    
      return commandSent;
    }
      
    ActionBehaviour extract_action_done_from_actions2Do(string actionId, const vector<ActionBehaviour> & actions2Do){
      ActionBehaviour actionDone;
      for(int i = 0; i<actions2Do.size(); i++){
        if(actionId == actions2Do[i].id){
          actionDone = actions2Do[i];
          break;
        }
      }    
      return actionDone;
    }
  
    vector<TaskPriorityType> extract_task_with_priority_j(int j,  const DecisionMakerBehaviour& behaviour){
      vector<TaskPriorityType> same_priority;    
      for(int i = 0; i < behaviour.taskPrioritized.size();i++){
        if(behaviour.taskPrioritized[i].priority == j) same_priority.push_back(behaviour.taskPrioritized[i].task);
      }
      return same_priority;
    }
  
    vector<pair<DeviceId,Communication>> const extract_requests_with_same_device_priority(const vector<pair<DeviceId,Communication>> & answerRequests, const DecisionMakerBehaviour& behaviour){
      vector<pair<DeviceId,Communication>> same_priority;
      int highestPriority = 999;
      for(int i =0; i < behaviour.answerDevicePriority.size();i++){ //Devices with same priority level      
        for(int z = 0; z < answerRequests.size(); z++){
          if(answerRequests[z].first.type == behaviour.answerDevicePriority[i].device && behaviour.answerDevicePriority[i].priority < highestPriority) highestPriority = behaviour.answerDevicePriority[i].priority;
          if(highestPriority == 1) break;
        }
      }
      if(highestPriority < 999){
        for(int i = 0; i < behaviour.answerDevicePriority.size(); i++){
          for(int z = 0; z < answerRequests.size(); z++){
            if(behaviour.answerDevicePriority[i].priority == highestPriority && answerRequests[z].first.type == behaviour.answerDevicePriority[i].device) same_priority.push_back(answerRequests[z]);
          }          
        }
      }else{
        same_priority = answerRequests;
      }
      return same_priority;
    }
  
    vector<pair<DeviceId,Communication>> const extract_requests_with_same_person_priority(const vector<pair<DeviceId,Communication>> & answerRequests, const DecisionMakerBehaviour& behaviour){
      vector<pair<DeviceId,Communication>> same_priority;
      int highestPriority = 999;
      for(int i =0; i < behaviour.answerPersonPriority.size();i++){ //Person with same priority level      
        for(int z = 0; z < answerRequests.size(); z++){
          if(answerRequests[z].first.id == behaviour.answerPersonPriority[i].personId && behaviour.answerPersonPriority[i].priority < highestPriority) highestPriority = behaviour.answerPersonPriority[i].priority;
          if(highestPriority == 1) break;
        }
      }
      if(highestPriority < 999){
        for(int i = 0; i < behaviour.answerPersonPriority.size(); i++){
          for(int z = 0; z < answerRequests.size(); z++){
            if(behaviour.answerPersonPriority[i].priority == highestPriority && answerRequests[z].first.id == behaviour.answerPersonPriority[i].personId) same_priority.push_back(answerRequests[z]);
          }          
        }
      }else{
        same_priority = answerRequests;
      }
      return same_priority;
    }
  
    pair<DeviceType, DeviceType> const choose_com_channel(const Msg2Send& command, const DecisionMakerBehaviour& behaviour){
      pair<DeviceType, DeviceType> device_choosen; // from - to
      bool in_person_available = false;
      std::vector<DeviceType> devices_to;
      bool person_found = false;
      for(int i = 0; i<behaviour.peopleSameLocation.size();i++){
        if(command.to == behaviour.peopleSameLocation[i]){          
          in_person_available = true;
          break;
        }
      }
      for(int i = 0; i<behaviour.communicationRelations.size(); i++){
        if(command.to == behaviour.communicationRelations[i].personId){
          devices_to = behaviour.communicationRelations[i].devices;//Retrieve the list of channels I can use with that person
          person_found = true;
          break;
        }
      }
      assert(command.deviceCompulsory.size() <= 1 && "Only one device compulsory is accepted" );
      DeviceType compulsory;
      (command.deviceCompulsory.empty()) ? compulsory = DeviceType::IDLE : compulsory = command.deviceCompulsory[0];
      if(compulsory != DeviceType::IDLE){
        bool compulsoryDeviceAvailable = false;
        for(int i = 0; i<devices_to.size(); i++){
          if(compulsory == devices_to[i]){
            compulsoryDeviceAvailable = true;
            devices_to.clear();
            devices_to.push_back(compulsory);
            break;
          }
        }
        if(compulsoryDeviceAvailable == false) devices_to.clear();
      }      
      if(in_person_available && compulsory == DeviceType::IDLE){
        device_choosen = make_pair(DeviceType::IN_PERSON, DeviceType::IN_PERSON);
      }else{
        if(person_found){
          for(int i = 0; i < devices_to.size(); i++){ //choose the first one in the list that is available
            switch(devices_to[i]){
              case DeviceType::MOBILEPHONE:
              case DeviceType::LANDLINEPHONE:
                for(int j = 0; j < behaviour.myDevices.size(); j++){
                  if(behaviour.myDevices[j].device == DeviceType::MOBILEPHONE || behaviour.myDevices[j].device == DeviceType::LANDLINEPHONE){
                    device_choosen = make_pair(behaviour.myDevices[j].device, devices_to[i]);
                    i = devices_to.size();
                    break;
                  }
                }
                break;
              case DeviceType::EMAIL:
              case DeviceType::BEEPER:
              case DeviceType::FAX:
              case DeviceType::SATELLITE_PHONE:
              case DeviceType::RADIOLOGICAL_GROUP_DEVICE:
              case DeviceType::PRIVATELINEPHONE:
              case DeviceType::RADIO_REMAN:
              case DeviceType::RADIO_REMER:
              case DeviceType::TRANKI_GC:
              case DeviceType::TRANKI_E:
                for(int j = 0; j < behaviour.myDevices.size(); j++){
                  if(behaviour.myDevices[j].device == devices_to[i]){
                    device_choosen = make_pair(behaviour.myDevices[j].device, devices_to[i]);
                    i = devices_to.size();
                    break;
                  }
                }
                break;
              default: assert(false && "DEVICE NOT IMPLEMENTED");
            }
          }
        }else{
          device_choosen = make_pair(DeviceType::IDLE, DeviceType::IDLE);
        }       
      }          
      return device_choosen;
    }
  
    vector<Msg2Send> const extract_commands_with_same_pripority(const vector<Msg2Send>& commands2Send, const DecisionMakerBehaviour& behaviour){
      vector<Msg2Send> same_priority;
      int highestPriority = 999;
      for(int i =0; i < behaviour.sendCommandPriority.size();i++){ //Highest priority level
        for(int z = 0; z < commands2Send.size(); z++){
          if(commands2Send[z].to == behaviour.sendCommandPriority[i].to && commands2Send[z].content == behaviour.sendCommandPriority[i].msg &&behaviour.sendCommandPriority[i].priority < highestPriority) highestPriority = behaviour.sendCommandPriority[i].priority;
          if(highestPriority == 1) break;
        }
      }  
      if(highestPriority < 999){
        for(int i = 0; i < behaviour.sendCommandPriority.size(); i++){
          for(int z = 0; z < commands2Send.size(); z++){
            if(behaviour.sendCommandPriority[i].priority == highestPriority && commands2Send[z].to == behaviour.sendCommandPriority[i].to && commands2Send[z].content == behaviour.sendCommandPriority[i].msg) same_priority.push_back(commands2Send[z]);
          }          
        }
      }else{
        same_priority = commands2Send;
      }
     return same_priority;
    }

    vector<ActionBehaviour> extract_actions_with_same_pripority(const vector<ActionBehaviour> actions2Do, const DecisionMakerBehaviour& behaviour){
      //The actions are ordered in actionExecutionPriority based on priority
      vector<ActionBehaviour> actionsSamePriority;
      for(int i =0; i < behaviour.actionExecutionPriority.size();i++){ //Find action with highest priority
        for(int z = 0; z < actions2Do.size(); z++){
          if(actions2Do[z].id == behaviour.actionExecutionPriority[i]){
            actionsSamePriority.push_back(actions2Do[z]);
            break; //for z
            break; //for i
          }
        }
      }  
      if(actionsSamePriority.empty()) actionsSamePriority = actions2Do;
      return actionsSamePriority;
    }
};
#endif // CADMIUM_DECISION_MAKER_HPP