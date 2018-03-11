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

#ifndef FUCTIONS_TO_INSTANTIATE_COUPLEDS_HPP
#define FUCTIONS_TO_INSTANTIATE_COUPLEDS_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <math.h> 
#include <vector>

#include "functions2InstantiateAtomics.hpp"
#include "../../data_structures/decision_maker_behaviour_struct_types_V2.hpp"
#include "../../data_structures/nep_model_enum_types.hpp"

using namespace std;
using namespace nep_model_enum_types;
using namespace decision_maker_behaviour_structures;

/****COUPLED AUXILIARY FUNCTIONS****/
vector<string> open_coupled(string coupledName){
	vector<string> coupled;
	coupled.push_back(string("using iports_")+coupledName+string(" = std::tuple<"));
	coupled.push_back(string("using oports_")+coupledName+string(" = std::tuple<"));
	coupled.push_back(string("using submodels_")+coupledName+string(" =cadmium::modeling::models_tuple<"));
	coupled.push_back(string("using eics_")+coupledName+string(" = std::tuple<"));
	coupled.push_back(string("using eocs_")+coupledName+string(" = std::tuple<"));
	coupled.push_back(string("using ics_")+coupledName+string(" = std::tuple<"));
	coupled.push_back(string("template<typename TIME> \n struct ")+coupledName+string(" : public cadmium::modeling::coupled_model<TIME, iports_")+coupledName+string(", oports_")+coupledName+string(", submodels_")+coupledName+string(", eics_")+coupledName+string(", eocs_")+coupledName+string(", ics_")+coupledName+string(">{};"));
	return coupled;
}

void close_coupled(vector<string>& coupledName){
	for(int i = 0; i < 6; i++){
		coupledName[i] += ">;";
	}
}

string print_coupled(vector<string>& coupledName){
	string coupled;
	for(int i = 0; i<coupledName.size(); i++){
		coupled += coupledName[i] + string("\n");
	}
	return coupled;
}

/****COUPLED WITH ONLY ATOMIC MODELS *****/

pair<vector<string>,vector<string>> NetworksCoupledModel(){
    pair<vector<string>,vector<string>> AtomicsCoupled;

    AtomicsCoupled.first.push_back(create_atomic_NetPhone(string("Phone"), string("50"), string("00:00:00:100")));
    AtomicsCoupled.first.push_back(create_atomic_NetPhone(string("Satellite"), string("1"), string("00:00:00:100")));
    AtomicsCoupled.first.push_back(create_atomic_NetPhone(string("Radiological"), string("5"), string("00:00:00:100")));
    AtomicsCoupled.first.push_back(create_atomic_NetPhone(string("CivilGuard"), string("1"),string("00:00:00:100")));
    AtomicsCoupled.first.push_back(create_atomic_NetPhone(string("Ertzaintza"), string("1"),string("00:00:00:100")));
    AtomicsCoupled.first.push_back(create_atomic_NetPhone(string("Privateline"), string("1"), string("00:00:00:100")));
    AtomicsCoupled.first.push_back(create_atomic_NetText(string("Internet"), string("00:00:30:000")));
    AtomicsCoupled.first.push_back(create_atomic_NetText(string("Beeper"), string("00:00:20:000")));
    AtomicsCoupled.first.push_back(create_atomic_NetText(string("Fax"), string("00:10:00:000")));
    AtomicsCoupled.first.push_back(create_atomic_NetRadio(string("RadioReman"), string("20"), string("00:00:01:000")));
    AtomicsCoupled.first.push_back(create_atomic_NetRadio(string("RadioRemer"), string("20"), string("00:00:02:000")));

    vector<string> NETWORKS = open_coupled("NETWORKS");
    string nIPORTS = "inp_NetworksState,inp_in_com";
    string nOPORTS = "outp_out_com";
    string nSUBMODELS = "filterNetworks, filterNetworksState, NetPhonePhone, NetPhoneSatellite, NetPhoneRadiological, NetPhoneCivilGuard, NetPhoneErtzaintza, NetPhonePrivateline,";
    nSUBMODELS += "NetTextInternet, NetTextBeeper, NetTextFax, NetRadioRadioReman, NetRadioRadioRemer";

    string nEIC = "cadmium::modeling::EIC<inp_in_com, filterNetworks, filterNetworks_defs::in>,\n";
    nEIC += "cadmium::modeling::EIC<inp_NetworksState, filterNetworksState, filterNetworksState_defs::in>\n";   

    string nEOC = "  cadmium::modeling::EOC<NetPhonePhone, NetPhone_defs::out, outp_out_com>,\n";
    nEOC += "  cadmium::modeling::EOC<NetPhoneSatellite, NetPhone_defs::out, outp_out_com>,\n";
    nEOC += "  cadmium::modeling::EOC<NetPhoneRadiological, NetPhone_defs::out, outp_out_com>,\n";
    nEOC += "  cadmium::modeling::EOC<NetPhoneCivilGuard, NetPhone_defs::out, outp_out_com>,\n";
    nEOC += "  cadmium::modeling::EOC<NetPhoneErtzaintza, NetPhone_defs::out, outp_out_com>,\n";
    nEOC += "  cadmium::modeling::EOC<NetPhonePrivateline, NetPhone_defs::out, outp_out_com>,\n";
    nEOC += "  cadmium::modeling::EOC<NetTextInternet, NetText_defs::out, outp_out_com>,\n";
    nEOC += "  cadmium::modeling::EOC<NetTextBeeper, NetText_defs::out, outp_out_com>,\n";
    nEOC += "  cadmium::modeling::EOC<NetTextFax, NetText_defs::out, outp_out_com>,\n";
    nEOC += "  cadmium::modeling::EOC<NetRadioRadioReman, NetRadio_defs::out, outp_out_com>,\n";
    nEOC += "  cadmium::modeling::EOC<NetRadioRadioRemer, NetRadio_defs::out, outp_out_com>\n";

    string nIC = "  cadmium::modeling::IC<filterNetworks, filterNetworks_defs::outPhoneNet, NetPhonePhone, NetPhone_defs::in>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworks, filterNetworks_defs::outSatelliteNet, NetPhoneSatellite, NetPhone_defs::in>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworks, filterNetworks_defs::outRadiologicalGroupNet, NetPhoneRadiological, NetPhone_defs::in>,\n";    
    nIC += "  cadmium::modeling::IC<filterNetworks, filterNetworks_defs::outCivilGuardNet, NetPhoneCivilGuard, NetPhone_defs::in>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworks, filterNetworks_defs::outAutonomousPoliceNet, NetPhoneErtzaintza, NetPhone_defs::in>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworks, filterNetworks_defs::outSpecialPhoneNet, NetPhonePrivateline, NetPhone_defs::in>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworks, filterNetworks_defs::outInternetNet, NetTextInternet, NetText_defs::in>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworks, filterNetworks_defs::outBeeperNet, NetTextBeeper, NetText_defs::in>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworks, filterNetworks_defs::outFaxNet, NetTextFax, NetText_defs::in>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworks, filterNetworks_defs::outRadioRemanNet, NetRadioRadioReman, NetRadio_defs::in>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworks, filterNetworks_defs::outRadioRemerNet, NetRadioRadioRemer, NetRadio_defs::in>,\n";

    nIC += "  cadmium::modeling::IC<filterNetworksState, filterNetworksState_defs::outPhoneNet, NetPhonePhone, NetPhone_defs::inNetState>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworksState, filterNetworksState_defs::outSatelliteNet, NetPhoneSatellite, NetPhone_defs::inNetState>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworksState, filterNetworksState_defs::outRadiologicalGroupNet, NetPhoneRadiological, NetPhone_defs::inNetState>,\n";    
    nIC += "  cadmium::modeling::IC<filterNetworksState, filterNetworksState_defs::outCivilGuardNet, NetPhoneCivilGuard, NetPhone_defs::inNetState>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworksState, filterNetworksState_defs::outAutonomousPoliceNet, NetPhoneErtzaintza, NetPhone_defs::inNetState>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworksState, filterNetworksState_defs::outSpecialPhoneNet, NetPhonePrivateline, NetPhone_defs::inNetState>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworksState, filterNetworksState_defs::outInternetNet, NetTextInternet, NetText_defs::inNetState>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworksState, filterNetworksState_defs::outBeeperNet, NetTextBeeper, NetText_defs::inNetState>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworksState, filterNetworksState_defs::outFaxNet, NetTextFax, NetText_defs::inNetState>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworksState, filterNetworksState_defs::outRadioRemanNet, NetRadioRadioReman, NetRadio_defs::inNetState>,\n";
    nIC += "  cadmium::modeling::IC<filterNetworksState, filterNetworksState_defs::outRadioRemerNet, NetRadioRadioRemer, NetRadio_defs::inNetState>\n";
  
    /************************/     
    NETWORKS[0] += nIPORTS;
    NETWORKS[1] += nOPORTS;
    NETWORKS[2] += nSUBMODELS;
    NETWORKS[3] += nEIC;
    NETWORKS[4] += nEOC;
    NETWORKS[5] += nIC;         
    close_coupled(NETWORKS);
    AtomicsCoupled.second = NETWORKS;
    return AtomicsCoupled;   
}

pair<vector<string>,vector<string>> TextDeviceCoupledModel(string DeviceType, string Id){
	pair<vector<string>,vector<string>> AtomicsCoupled;
	/***Define atomics ***/
	AtomicsCoupled.first.push_back(create_atomic_DeviceTextInbox(DeviceType, Id, "00:01:00:000", "00:00:10:000"));
	string inbox = "DeviceTextInbox"+DeviceType+Id;
	AtomicsCoupled.first.push_back(create_atomic_DeviceTextOutbox(DeviceType, Id, "00:01:00:000", "00:00:10:000"));
	string outbox = "DeviceTextOutbox"+DeviceType+Id;
    AtomicsCoupled.first.push_back(create_atomic_DeviceTextClasNewRead(DeviceType, Id, "00:00:10:000"));
	string DeviceTextClasNewRead = "DeviceTextClasNewRead"+DeviceType+Id;
    AtomicsCoupled.first.push_back(create_atomic_DeviceTextClasNewBroken(DeviceType, Id, "00:00:10:000"));
    string DeviceTextClasNewBroken = "DeviceTextClasNewBroken"+DeviceType+Id;
	/****Define coupled****/

    vector<string> TEXTCOUPLED = open_coupled(DeviceType+Id);
    string tcIPORTS = "inp_DevicesState,inp_network,inp_fromKeyboard";
    string tcOPORTS = "outp_toScreen,outp_network";
    string tcSUBMODELS = inbox+string(",")+outbox+string(",")+DeviceTextClasNewRead+string(",")+DeviceTextClasNewBroken;

    string tcEIC = "cadmium::modeling::EIC<inp_DevicesState,"+inbox+", DeviceTextInbox_defs::setStateIn>,\n";
    tcEIC += " cadmium::modeling::EIC<inp_DevicesState,"+outbox+", DeviceTextOutbox_defs::setStateIn>,\n";   
    tcEIC += " cadmium::modeling::EIC<inp_network, "+DeviceTextClasNewBroken+", DeviceTextClasNewBroken_defs::in>, \n";
    tcEIC += " cadmium::modeling::EIC<inp_fromKeyboard,"+DeviceTextClasNewRead+", DeviceTextClasNewRead_defs::in> \n";

    string tcEOC = "cadmium::modeling::EOC<"+inbox+string(", DeviceTextInbox_defs::displayOut, outp_toScreen>,");
    tcEOC += "cadmium::modeling::EOC<"+outbox+string(", DeviceTextOutbox_defs::displayOut, outp_toScreen>,");
    tcEOC += "cadmium::modeling::EOC<"+outbox+string(", DeviceTextOutbox_defs::networkOut, outp_network>");

    string tcIC = "cadmium::modeling::IC<"+DeviceTextClasNewRead+string(", DeviceTextClasNewRead_defs::newOut, ")+outbox+",DeviceTextOutbox_defs::newIn>,\n";
    tcIC += "cadmium::modeling::IC<"+DeviceTextClasNewRead+string(", DeviceTextClasNewRead_defs::readOut, ")+inbox+" ,DeviceTextInbox_defs::readIn>,\n";
    tcIC += "cadmium::modeling::IC<"+DeviceTextClasNewBroken+string(", DeviceTextClasNewBroken_defs::newOut, ")+inbox+",DeviceTextInbox_defs::newIn>,\n";
    tcIC += "cadmium::modeling::IC<"+DeviceTextClasNewBroken+string(", DeviceTextClasNewBroken_defs::brokenOut, ")+outbox+" ,DeviceTextOutbox_defs::brokenIn>\n";  
  
    /************************/     
    TEXTCOUPLED[0] += tcIPORTS;
    TEXTCOUPLED[1] += tcOPORTS;
    TEXTCOUPLED[2] += tcSUBMODELS;
    TEXTCOUPLED[3] += tcEIC;
    TEXTCOUPLED[4] += tcEOC;
    TEXTCOUPLED[5] += tcIC;         
    close_coupled(TEXTCOUPLED);
    AtomicsCoupled.second = TEXTCOUPLED;
   
	return AtomicsCoupled;	
}

pair<vector<string>,vector<string>> BehaviorRulesCoupledModel(string File, DecisionMakerBehaviour person){
	pair<vector<string>,vector<string>> AtomicsCoupled;
    
    AtomicsCoupled.first.push_back(create_atomic_PersonBehaviorRules(File, person.id));
    AtomicsCoupled.first.push_back(create_atomic_doActionTask(person.id, person.location));

    vector<string> BEHAVIORRULES = open_coupled("BEHAVIORRULES"+person.id);
    string dcIPORTS = string("inp_generator,inp_decideInfo, inp_peopleInMyLocation, inp_taskDeviceFinished");
    string dcOPORTS = string("outp_send_inperson, outp_send_device, outp_answer_inperson, outp_answer_device, outp_myLocation, outp_taskActionFinished");
    string dcSUBMODELS = "PersonBehaviorRules"+person.id+", doActionTask"+person.id;
    string dcEIC = "cadmium::modeling::EIC<inp_generator,PersonBehaviorRules"+person.id+",PersonBehaviorRules_defs::generatorIn>,\n";
    dcEIC += "cadmium::modeling::EIC<inp_decideInfo,PersonBehaviorRules"+person.id+", PersonBehaviorRules_defs::requestIn>,\n";
    dcEIC += "cadmium::modeling::EIC<inp_peopleInMyLocation,PersonBehaviorRules"+person.id+", PersonBehaviorRules_defs::peopleInSameLocationIn>,\n";
    dcEIC += "cadmium::modeling::EIC<inp_taskDeviceFinished,PersonBehaviorRules"+person.id+", PersonBehaviorRules_defs::taskDeviceFinishedIn>\n";
    string dcEOC="cadmium::modeling::EOC<PersonBehaviorRules"+person.id+", PersonBehaviorRules_defs::taskSendOutInperson, outp_send_inperson>,\n";
    dcEOC += "cadmium::modeling::EOC<PersonBehaviorRules"+person.id+", PersonBehaviorRules_defs::taskAnswerOutInperson, outp_answer_inperson>,\n";
    dcEOC += "cadmium::modeling::EOC<PersonBehaviorRules"+person.id+", PersonBehaviorRules_defs::taskSendOutDevice, outp_send_device>,\n";
    dcEOC += "cadmium::modeling::EOC<PersonBehaviorRules"+person.id+", PersonBehaviorRules_defs::taskAnswerOutDevice, outp_answer_device>,\n";
    dcEOC += "cadmium::modeling::EOC<doActionTask"+person.id+", doActionTask_defs::locationOut, outp_myLocation>,\n";
    dcEOC += "cadmium::modeling::EOC<doActionTask"+person.id+", doActionTask_defs::taskFinisedAcknow, outp_taskActionFinished>\n";
    string dcIC= "cadmium::modeling::IC<PersonBehaviorRules"+person.id+", PersonBehaviorRules_defs::taskDoActionOut, doActionTask"+person.id+", doActionTask_defs::task>,\n";
    dcIC += "cadmium::modeling::IC<doActionTask"+person.id+", doActionTask_defs::taskFinisedAcknow, PersonBehaviorRules"+person.id+", PersonBehaviorRules_defs::taskActionFinishedIn>\n";

    /************************/
    BEHAVIORRULES[0] += dcIPORTS;
    BEHAVIORRULES[1] += dcOPORTS;
    BEHAVIORRULES[2] += dcSUBMODELS;
    BEHAVIORRULES[3] += dcEIC;
    BEHAVIORRULES[4] += dcEOC;
    BEHAVIORRULES[5] += dcIC;
    close_coupled(BEHAVIORRULES);
    AtomicsCoupled.second =BEHAVIORRULES;
    //cout << "sale de BehaviorRulesCoupledModel" << endl;
    return AtomicsCoupled;
}

pair<vector<string>,vector<string>> SendingBehaviorCoupledModel(DecisionMakerBehaviour person){
    //cout << "entra en SendingBehaviorCoupledModel" << endl;
    pair<vector<string>,vector<string>> AtomicsCoupled;

    AtomicsCoupled.first.push_back(create_atomic_filterPersonDeviceTypeTo("SwitchesS"+person.id));
    AtomicsCoupled.first.push_back(create_atomic_filterPersonDeviceTypeFrom("Sending"+person.id));

    vector<string> SENDINGBEHAVIORCOUPLED = open_coupled("SENDINGBEHAVIOR"+person.id);
    string dcIPORTS = string("inp_in_com,inp_task_CWD");
    string dcOPORTS = string("outp_out_com, outp_taskDeviceFinished");
    string dcSUBMODELS = string("filterPersonDeviceTypeToSwitchesS")+person.id+string(", filterPersonDeviceTypeFromSending")+person.id+string(", sinkSendingBehavior,");
    string dcEIC = "cadmium::modeling::EIC<inp_in_com,filterPersonDeviceTypeToSwitchesS"+person.id+",filterPersonDeviceTypeTo_defs::in>,";
    dcEIC += "cadmium::modeling::EIC<inp_task_CWD,filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::in>,";
    string dcEOC;
    string dcIC;
     
    bool mobilephone = false;
    bool landlinephone = false;
    bool privatelinephone = false;
    bool satelitephone = false;
    bool radiological_group_device = false;
    bool beeper = false;
    bool email = false;
    bool fax = false;
    bool radio_reman = false;
    bool radio_remer = false;
    bool tranki_gc = false;
    bool tranki_e = false;

    for(int z = 0; z < person.myDevices.size();z++){
      switch(person.myDevices[z].device){
        case DeviceType::MOBILEPHONE: mobilephone = true; break;
        case DeviceType::LANDLINEPHONE: landlinephone = true; break;
        case DeviceType::PRIVATELINEPHONE: privatelinephone = true; break;
        case DeviceType::SATELLITE_PHONE: satelitephone = true; break;
        case DeviceType::RADIOLOGICAL_GROUP_DEVICE: radiological_group_device = true; break;
        case DeviceType::BEEPER: beeper = true; break;
        case DeviceType::EMAIL: email = true; break;
        case DeviceType::FAX: fax = true; break;
        case DeviceType::RADIO_REMAN: radio_reman = true; break;
        case DeviceType::RADIO_REMER: radio_remer = true; break; 
        case DeviceType::TRANKI_GC: tranki_gc = true; break;
        case DeviceType::TRANKI_E: tranki_e = true; break;
        default: assert(false && "Not such DeviceType"); break;
      }  
    }
    if(mobilephone){
      AtomicsCoupled.first.push_back(create_atomic_callByPhoneTask("MOBILEPHONE", person.id));
      dcSUBMODELS += "callByPhoneTaskMOBILEPHONE"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outMOBILEPHONE, callByPhoneTaskMOBILEPHONE"+person.id+", callByPhoneTask_defs::fromSpeaker>,";
      dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outMOBILEPHONE, callByPhoneTaskMOBILEPHONE"+person.id+", callByPhoneTask_defs::task>,";
      dcEOC += "cadmium::modeling::EOC<callByPhoneTaskMOBILEPHONE"+person.id+", callByPhoneTask_defs::toMicrophone, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<callByPhoneTaskMOBILEPHONE"+person.id+", callByPhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outMOBILEPHONE, sinkSendingBehavior, sinkSendingBehavior_defs::inCommunication>,";
      dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outMOBILEPHONE, sinkSendingBehavior, sinkSendingBehavior_defs::inCommandWithDevice>,";
    }
    if(landlinephone){
        AtomicsCoupled.first.push_back(create_atomic_callByPhoneTask("LANDLINEPHONE", person.id));
        dcSUBMODELS += "callByPhoneTaskLANDLINEPHONE"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outLANDLINEPHONE, callByPhoneTaskLANDLINEPHONE"+person.id+", callByPhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outLANDLINEPHONE, callByPhoneTaskLANDLINEPHONE"+person.id+", callByPhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<callByPhoneTaskLANDLINEPHONE"+person.id+", callByPhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<callByPhoneTaskLANDLINEPHONE"+person.id+", callByPhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outLANDLINEPHONE, sinkSendingBehavior, sinkSendingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outLANDLINEPHONE, sinkSendingBehavior, sinkSendingBehavior_defs::inCommandWithDevice>,";
    }
    if(privatelinephone){
        AtomicsCoupled.first.push_back(create_atomic_callByPhoneTask("PRIVATELINEPHONE", person.id));
        dcSUBMODELS += "callByPhoneTaskPRIVATELINEPHONE"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outPRIVATELINEPHONE, callByPhoneTaskPRIVATELINEPHONE"+person.id+", callByPhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outPRIVATELINEPHONE, callByPhoneTaskPRIVATELINEPHONE"+person.id+", callByPhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<callByPhoneTaskPRIVATELINEPHONE"+person.id+", callByPhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<callByPhoneTaskPRIVATELINEPHONE"+person.id+", callByPhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outPRIVATELINEPHONE, sinkSendingBehavior, sinkSendingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outPRIVATELINEPHONE, sinkSendingBehavior, sinkSendingBehavior_defs::inCommandWithDevice>,";
    }
    if(satelitephone){
        AtomicsCoupled.first.push_back(create_atomic_callByPhoneTask("SATELLITE_PHONE", person.id));
        dcSUBMODELS += "callByPhoneTaskSATELLITE_PHONE"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outSATELLITE_PHONE, callByPhoneTaskSATELLITE_PHONE"+person.id+", callByPhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outSATELLITE_PHONE, callByPhoneTaskSATELLITE_PHONE"+person.id+", callByPhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<callByPhoneTaskSATELLITE_PHONE"+person.id+", callByPhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<callByPhoneTaskSATELLITE_PHONE"+person.id+", callByPhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outSATELLITE_PHONE, sinkSendingBehavior, sinkSendingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outSATELLITE_PHONE, sinkSendingBehavior, sinkSendingBehavior_defs::inCommandWithDevice>,";
    }
    if(radiological_group_device){
        AtomicsCoupled.first.push_back(create_atomic_callByPhoneTask("RADIOLOGICAL_GROUP_DEVICE", person.id));
        dcSUBMODELS += "callByPhoneTaskRADIOLOGICAL_GROUP_DEVICE"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outRADIOLOGICAL_GROUP_DEVICE, callByPhoneTaskRADIOLOGICAL_GROUP_DEVICE"+person.id+", callByPhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outRADIOLOGICAL_GROUP_DEVICE, callByPhoneTaskRADIOLOGICAL_GROUP_DEVICE"+person.id+", callByPhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<callByPhoneTaskRADIOLOGICAL_GROUP_DEVICE"+person.id+", callByPhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<callByPhoneTaskRADIOLOGICAL_GROUP_DEVICE"+person.id+", callByPhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outRADIOLOGICAL_GROUP_DEVICE, sinkSendingBehavior, sinkSendingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outRADIOLOGICAL_GROUP_DEVICE, sinkSendingBehavior, sinkSendingBehavior_defs::inCommandWithDevice>,";
    }
    if(beeper){
        AtomicsCoupled.first.push_back(create_atomic_sendTextTask("BEEPER", person.id));
        dcSUBMODELS += "sendTextTaskBEEPER"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outBEEPER, sendTextTaskBEEPER"+person.id+", sendTextTask_defs::fromScreen>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outBEEPER, sendTextTaskBEEPER"+person.id+", sendTextTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<sendTextTaskBEEPER"+person.id+", sendTextTask_defs::toKeyBoard, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<sendTextTaskBEEPER"+person.id+", sendTextTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outBEEPER, sinkSendingBehavior, sinkSendingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outBEEPER, sinkSendingBehavior, sinkSendingBehavior_defs::inCommandWithDevice>,";
    }
    if(email){
        AtomicsCoupled.first.push_back(create_atomic_sendTextTask("EMAIL", person.id));
        dcSUBMODELS += "sendTextTaskEMAIL"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outEMAIL, sendTextTaskEMAIL"+person.id+", sendTextTask_defs::fromScreen>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outEMAIL, sendTextTaskEMAIL"+person.id+", sendTextTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<sendTextTaskEMAIL"+person.id+", sendTextTask_defs::toKeyBoard, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<sendTextTaskEMAIL"+person.id+", sendTextTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outEMAIL, sinkSendingBehavior, sinkSendingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outEMAIL, sinkSendingBehavior, sinkSendingBehavior_defs::inCommandWithDevice>,";
    }
    if(fax){
        AtomicsCoupled.first.push_back(create_atomic_sendTextTask("FAX", person.id));
        dcSUBMODELS += "sendTextTaskFAX"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outFAX, sendTextTaskFAX"+person.id+", sendTextTask_defs::fromScreen>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outFAX, sendTextTaskFAX"+person.id+", sendTextTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<sendTextTaskFAX"+person.id+", sendTextTask_defs::toKeyBoard, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<sendTextTaskFAX"+person.id+", sendTextTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outFAX, sinkSendingBehavior, sinkSendingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outFAX, sinkSendingBehavior, sinkSendingBehavior_defs::inCommandWithDevice>,";
    }
    if(radio_reman){
        AtomicsCoupled.first.push_back(create_atomic_sendCommandByRadio("RADIO_REMAN", person.id));
        dcSUBMODELS += "sendCommandByRadioRADIO_REMAN"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outRADIO_REMAN, sendCommandByRadioRADIO_REMAN"+person.id+", sendCommandByRadio_defs::communicationIn>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outRADIO_REMAN, sendCommandByRadioRADIO_REMAN"+person.id+", sendCommandByRadio_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<sendCommandByRadioRADIO_REMAN"+person.id+", sendCommandByRadio_defs::communicationOut, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<sendCommandByRadioRADIO_REMAN"+person.id+", sendCommandByRadio_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outRADIO_REMAN, sinkSendingBehavior, sinkSendingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outRADIO_REMAN, sinkSendingBehavior, sinkSendingBehavior_defs::inCommandWithDevice>,";
    }
    if(radio_remer){
        AtomicsCoupled.first.push_back(create_atomic_sendCommandByRadio("RADIO_REMER", person.id));
        dcSUBMODELS += "sendCommandByRadioRADIO_REMER"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outRADIO_REMER, sendCommandByRadioRADIO_REMER"+person.id+", sendCommandByRadio_defs::communicationIn>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outRADIO_REMER, sendCommandByRadioRADIO_REMER"+person.id+", sendCommandByRadio_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<sendCommandByRadioRADIO_REMER"+person.id+", sendCommandByRadio_defs::communicationOut, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<sendCommandByRadioRADIO_REMER"+person.id+", sendCommandByRadio_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outRADIO_REMER, sinkSendingBehavior, sinkSendingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outRADIO_REMER, sinkSendingBehavior, sinkSendingBehavior_defs::inCommandWithDevice>,";
    }
    if(tranki_gc){
        AtomicsCoupled.first.push_back(create_atomic_callByMultiplePhoneTask("TRANKI_GC", person.id));
        dcSUBMODELS += "callByMultiplePhoneTaskTRANKI_GC"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outTRANKI_GC, callByMultiplePhoneTaskTRANKI_GC"+person.id+", callByMultiplePhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outTRANKI_GC, callByMultiplePhoneTaskTRANKI_GC"+person.id+", callByMultiplePhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<callByMultiplePhoneTaskTRANKI_GC"+person.id+", callByMultiplePhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<callByMultiplePhoneTaskTRANKI_GC"+person.id+", callByMultiplePhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outTRANKI_GC, sinkSendingBehavior, sinkSendingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outTRANKI_GC, sinkSendingBehavior, sinkSendingBehavior_defs::inCommandWithDevice>,";
    }
    if(tranki_e){
        AtomicsCoupled.first.push_back(create_atomic_callByMultiplePhoneTask("TRANKI_E", person.id));
        dcSUBMODELS += "callByMultiplePhoneTaskTRANKI_E"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outTRANKI_E, callByMultiplePhoneTaskTRANKI_E"+person.id+", callByMultiplePhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outTRANKI_E, callByMultiplePhoneTaskTRANKI_E"+person.id+", callByMultiplePhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<callByMultiplePhoneTaskTRANKI_E"+person.id+", callByMultiplePhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<callByMultiplePhoneTaskTRANKI_E"+person.id+", callByMultiplePhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesS"+person.id+", filterPersonDeviceTypeTo_defs::outTRANKI_E, sinkSendingBehavior, sinkSendingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeFromSending"+person.id+", filterPersonDeviceTypeFrom_defs::outTRANKI_E, sinkSendingBehavior, sinkSendingBehavior_defs::inCommandWithDevice>,";
    }
    /************************/
    if(!dcSUBMODELS.empty()) dcSUBMODELS.pop_back();
    if(!dcEIC.empty()) dcEIC.pop_back();
    if(!dcEOC.empty()) dcEOC.pop_back();
    if(!dcIC.empty()) dcIC.pop_back();
    SENDINGBEHAVIORCOUPLED[0] += dcIPORTS;
    SENDINGBEHAVIORCOUPLED[1] += dcOPORTS;
    SENDINGBEHAVIORCOUPLED[2] += dcSUBMODELS;
    SENDINGBEHAVIORCOUPLED[3] += dcEIC;
    SENDINGBEHAVIORCOUPLED[4] += dcEOC;
    SENDINGBEHAVIORCOUPLED[5] += dcIC;
    close_coupled(SENDINGBEHAVIORCOUPLED);
    AtomicsCoupled.second = SENDINGBEHAVIORCOUPLED;
    //cout << "sale de SendingBehaviorCoupledModel" << endl;
    return AtomicsCoupled;   
}

pair<vector<string>,vector<string>> ReceivingBehaviorCoupledModel(DecisionMakerBehaviour person){
    //cout << "entra en ReceivingBehaviorCoupledModel" << endl;
    pair<vector<string>,vector<string>> AtomicsCoupled;

    AtomicsCoupled.first.push_back(create_atomic_filterPersonDeviceTypeTo("SwitchesR"+person.id));
    AtomicsCoupled.first.push_back(create_atomic_filterPersonDeviceTypeTo("Receiving"+person.id));

    vector<string> RECEIVINGBEHAVIORCOUPLED = open_coupled("RECEIVINGBEHAVIOR"+person.id);
    string dcIPORTS = string("inp_in_com,inp_task");
    string dcOPORTS = string("outp_out_com, outp_taskDeviceFinished");
    string dcSUBMODELS = string("filterPersonDeviceTypeToSwitchesR")+person.id+string(", filterPersonDeviceTypeToReceiving")+person.id+string(", sinkReceivingBehavior,");
    string dcEIC = "cadmium::modeling::EIC<inp_in_com,filterPersonDeviceTypeToSwitchesR"+person.id+",filterPersonDeviceTypeTo_defs::in>,";
    dcEIC += "cadmium::modeling::EIC<inp_task,filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::in>,";
    string dcEOC;
    string dcIC;

    bool mobilephone = false;
    bool landlinephone = false;
    bool privatelinephone = false;
    bool satelitephone = false;
    bool radiological_group_device = false;
    bool beeper = false;
    bool email = false;
    bool fax = false;
    bool radio_reman = false;
    bool radio_remer = false;
    bool tranki_gc = false;
    bool tranki_e = false;

    for(int z = 0; z < person.myDevices.size();z++){
        switch(person.myDevices[z].device){
            case DeviceType::MOBILEPHONE: mobilephone = true; break;
            case DeviceType::LANDLINEPHONE: landlinephone = true; break;
            case DeviceType::PRIVATELINEPHONE: privatelinephone = true; break;
            case DeviceType::SATELLITE_PHONE: satelitephone = true; break;
            case DeviceType::RADIOLOGICAL_GROUP_DEVICE: radiological_group_device = true; break;
            case DeviceType::BEEPER: beeper = true; break;
            case DeviceType::EMAIL: email = true; break;
            case DeviceType::FAX: fax = true; break;
            case DeviceType::RADIO_REMAN: radio_reman = true; break;
            case DeviceType::RADIO_REMER: radio_remer = true; break;
            case DeviceType::TRANKI_GC: tranki_gc = true; break;
            case DeviceType::TRANKI_E: tranki_e = true; break;
            default: assert(false && "Not such DeviceType"); break;
        }
    }
    if(mobilephone){
        AtomicsCoupled.first.push_back(create_atomic_answerPhoneTask("MOBILEPHONE", person.id));
        dcSUBMODELS += "answerPhoneTaskMOBILEPHONE"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outMOBILEPHONE, answerPhoneTaskMOBILEPHONE"+person.id+", answerPhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outMOBILEPHONE, answerPhoneTaskMOBILEPHONE"+person.id+", answerPhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskMOBILEPHONE"+person.id+", answerPhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskMOBILEPHONE"+person.id+", answerPhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outMOBILEPHONE, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outMOBILEPHONE, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
    }
    if(landlinephone){
        AtomicsCoupled.first.push_back(create_atomic_answerPhoneTask("LANDLINEPHONE", person.id));
        dcSUBMODELS += "answerPhoneTaskLANDLINEPHONE"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outLANDLINEPHONE, answerPhoneTaskLANDLINEPHONE"+person.id+", answerPhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outLANDLINEPHONE, answerPhoneTaskLANDLINEPHONE"+person.id+", answerPhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskLANDLINEPHONE"+person.id+", answerPhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskLANDLINEPHONE"+person.id+", answerPhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outLANDLINEPHONE, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outLANDLINEPHONE, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
    }
    if(privatelinephone){
        AtomicsCoupled.first.push_back(create_atomic_answerPhoneTask("PRIVATELINEPHONE", person.id));
        dcSUBMODELS += "answerPhoneTaskPRIVATELINEPHONE"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outPRIVATELINEPHONE, answerPhoneTaskPRIVATELINEPHONE"+person.id+", answerPhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outPRIVATELINEPHONE, answerPhoneTaskPRIVATELINEPHONE"+person.id+", answerPhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskPRIVATELINEPHONE"+person.id+", answerPhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskPRIVATELINEPHONE"+person.id+", answerPhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outPRIVATELINEPHONE, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outPRIVATELINEPHONE, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
    }
    if(satelitephone){
        AtomicsCoupled.first.push_back(create_atomic_answerPhoneTask("SATELLITE_PHONE", person.id));
        dcSUBMODELS += "answerPhoneTaskSATELLITE_PHONE"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outSATELLITE_PHONE, answerPhoneTaskSATELLITE_PHONE"+person.id+", answerPhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outSATELLITE_PHONE, answerPhoneTaskSATELLITE_PHONE"+person.id+", answerPhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskSATELLITE_PHONE"+person.id+", answerPhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskSATELLITE_PHONE"+person.id+", answerPhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outSATELLITE_PHONE, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outSATELLITE_PHONE, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
    }
    if(radiological_group_device){
        AtomicsCoupled.first.push_back(create_atomic_answerPhoneTask("RADIOLOGICAL_GROUP_DEVICE", person.id));
        dcSUBMODELS += "answerPhoneTaskRADIOLOGICAL_GROUP_DEVICE"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outRADIOLOGICAL_GROUP_DEVICE,answerPhoneTaskRADIOLOGICAL_GROUP_DEVICE"+person.id+", answerPhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outRADIOLOGICAL_GROUP_DEVICE, answerPhoneTaskRADIOLOGICAL_GROUP_DEVICE"+person.id+", answerPhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskRADIOLOGICAL_GROUP_DEVICE"+person.id+", answerPhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskRADIOLOGICAL_GROUP_DEVICE"+person.id+", answerPhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outRADIOLOGICAL_GROUP_DEVICE, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outRADIOLOGICAL_GROUP_DEVICE, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
    }
    if(beeper){
        AtomicsCoupled.first.push_back(create_atomic_readTextOnDigitalDevice("BEEPER", person.id));
        dcSUBMODELS += "readTextOnDigitalDeviceBEEPER"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outBEEPER, readTextOnDigitalDeviceBEEPER"+person.id+", readTextOnDigitalDevice_defs::fromScreen>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outBEEPER, readTextOnDigitalDeviceBEEPER"+person.id+", readTextOnDigitalDevice_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<readTextOnDigitalDeviceBEEPER"+person.id+", readTextOnDigitalDevice_defs::toKeyBoard, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<readTextOnDigitalDeviceBEEPER"+person.id+", readTextOnDigitalDevice_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outBEEPER, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outBEEPER, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
    }
    if(email){
        AtomicsCoupled.first.push_back(create_atomic_readTextOnDigitalDevice("EMAIL", person.id));
        dcSUBMODELS += "readTextOnDigitalDeviceEMAIL"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outEMAIL, readTextOnDigitalDeviceEMAIL"+person.id+", readTextOnDigitalDevice_defs::fromScreen>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outEMAIL, readTextOnDigitalDeviceEMAIL"+person.id+", readTextOnDigitalDevice_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<readTextOnDigitalDeviceEMAIL"+person.id+", readTextOnDigitalDevice_defs::toKeyBoard, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<readTextOnDigitalDeviceEMAIL"+person.id+", readTextOnDigitalDevice_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outEMAIL, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outEMAIL, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
    }
    if(fax){
        AtomicsCoupled.first.push_back(create_atomic_readTextOnPaperDevice("FAX", person.id));
        dcSUBMODELS += "readTextOnPaperDeviceFAX"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outFAX, readTextOnPaperDeviceFAX"+person.id+", readTextOnPaperDevice_defs::fromScreen>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outFAX, readTextOnPaperDeviceFAX"+person.id+", readTextOnPaperDevice_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<readTextOnPaperDeviceFAX"+person.id+", readTextOnPaperDevice_defs::toKeyBoard, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<readTextOnPaperDeviceFAX"+person.id+", readTextOnPaperDevice_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outFAX, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outFAX, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
    }
    if(radio_reman){
        AtomicsCoupled.first.push_back(create_atomic_answerRadioTask("RADIO_REMAN", person.id));
        dcSUBMODELS += "answerRadioTaskRADIO_REMAN"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outRADIO_REMAN, answerRadioTaskRADIO_REMAN"+person.id+", answerRadioTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outRADIO_REMAN, answerRadioTaskRADIO_REMAN"+person.id+", answerRadioTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<answerRadioTaskRADIO_REMAN"+person.id+", answerRadioTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outRADIO_REMAN, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outRADIO_REMAN, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
    }
    if(radio_remer){
        AtomicsCoupled.first.push_back(create_atomic_answerRadioTask("RADIO_REMER", person.id));
        dcSUBMODELS += "answerRadioTaskRADIO_REMER"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outRADIO_REMER, answerRadioTaskRADIO_REMER"+person.id+", answerRadioTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outRADIO_REMER, answerRadioTaskRADIO_REMER"+person.id+", answerRadioTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<answerRadioTaskRADIO_REMER"+person.id+", answerRadioTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outRADIO_REMER, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outRADIO_REMER, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
    }
    if(tranki_gc){
        AtomicsCoupled.first.push_back(create_atomic_answerPhoneTask("TRANKI_GC", person.id));
        dcSUBMODELS += "answerPhoneTaskTRANKI_GC"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outTRANKI_GC, answerPhoneTaskTRANKI_GC"+person.id+", answerPhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outTRANKI_GC, answerPhoneTaskTRANKI_GC"+person.id+", answerPhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskTRANKI_GC"+person.id+", answerPhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskTRANKI_GC"+person.id+", answerPhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outTRANKI_GC, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outTRANKI_GC, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
    }
    if(tranki_e){
        AtomicsCoupled.first.push_back(create_atomic_answerPhoneTask("TRANKI_E", person.id));
        dcSUBMODELS += "answerPhoneTaskTRANKI_E"+person.id+",";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outTRANKI_E, answerPhoneTaskTRANKI_E"+person.id+", answerPhoneTask_defs::fromSpeaker>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outTRANKI_E, answerPhoneTaskTRANKI_E"+person.id+", answerPhoneTask_defs::task>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskTRANKI_E"+person.id+", answerPhoneTask_defs::toMicrophone, outp_out_com>,";
        dcEOC += "cadmium::modeling::EOC<answerPhoneTaskTRANKI_E"+person.id+", answerPhoneTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    }else{
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToSwitchesR"+person.id+", filterPersonDeviceTypeTo_defs::outTRANKI_E, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
        dcIC += "cadmium::modeling::IC<filterPersonDeviceTypeToReceiving"+person.id+", filterPersonDeviceTypeTo_defs::outTRANKI_E, sinkReceivingBehavior, sinkReceivingBehavior_defs::inCommunication>,";
    }
    /************************/
    if(!dcSUBMODELS.empty()) dcSUBMODELS.pop_back();
    if(!dcEIC.empty()) dcEIC.pop_back();
    if(!dcEOC.empty()) dcEOC.pop_back();
    if(!dcIC.empty()) dcIC.pop_back();
    RECEIVINGBEHAVIORCOUPLED[0] += dcIPORTS;
    RECEIVINGBEHAVIORCOUPLED[1] += dcOPORTS;
    RECEIVINGBEHAVIORCOUPLED[2] += dcSUBMODELS;
    RECEIVINGBEHAVIORCOUPLED[3] += dcEIC;
    RECEIVINGBEHAVIORCOUPLED[4] += dcEOC;
    RECEIVINGBEHAVIORCOUPLED[5] += dcIC;
    close_coupled(RECEIVINGBEHAVIORCOUPLED);
    AtomicsCoupled.second = RECEIVINGBEHAVIORCOUPLED;
    //cout << "sale de ReceivingBehaviorCoupledModel" << endl;
    return AtomicsCoupled;
}

/*****COUPLEDS WITH COUPLES INSIDE*****/

pair<vector<string>,vector<vector<string>>> DevicesCoupledModel(DecisionMakerBehaviour person){
	//cout << "entra en DevicesCoupledModel" << endl;
	pair<vector<string>,vector<vector<string>>> AtomicsCoupled;

    AtomicsCoupled.first.push_back(create_atomic_filterDevicesNetwork(person.id));
    AtomicsCoupled.first.push_back(create_atomic_filterDevicesState(person.id));

    vector<string> DEVICESCOUPLED = open_coupled("DEVICES"+person.id);
    string dcIPORTS = string("inp_DevicesState,inp_in_com,inp_network");
    string dcOPORTS = string("outp_out_com, outp_network");
    string dcSUBMODELS = string("filterDevicesState")+person.id+string(", filterDevicesNetwork")+person.id+string(", filterDevicesMicroKeyboard, sinkDevices,");
    string dcEIC = "cadmium::modeling::EIC<inp_DevicesState,filterDevicesState"+person.id+",filterDevicesState_defs::in>,";
    dcEIC += "cadmium::modeling::EIC<inp_in_com,filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::in>,";
    dcEIC += "cadmium::modeling::EIC<inp_network,filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::in>,";
    string dcEOC;
    string dcIC;
     
    bool mobilephone = false;
    bool landlinephone = false;
    bool privatelinephone = false;
    bool satelitephone = false;
    bool radiological_group_device = false;
    bool beeper = false;
    bool email = false;
    bool fax = false;
    bool radio_reman = false;
    bool radio_remer = false;
    bool tranki_gc = false;
    bool tranki_e = false;

    for(int z = 0; z < person.myDevices.size();z++){
      switch(person.myDevices[z].device){
        case DeviceType::MOBILEPHONE: mobilephone = true; break;
        case DeviceType::LANDLINEPHONE: landlinephone = true; break;
        case DeviceType::PRIVATELINEPHONE: privatelinephone = true; break;
        case DeviceType::SATELLITE_PHONE: satelitephone = true; break;
        case DeviceType::RADIOLOGICAL_GROUP_DEVICE: radiological_group_device = true; break;
        case DeviceType::BEEPER: beeper = true; break;
        case DeviceType::EMAIL: email = true; break;
        case DeviceType::FAX: fax = true; break;
        case DeviceType::RADIO_REMAN: radio_reman = true; break;
        case DeviceType::RADIO_REMER: radio_remer = true; break; 
        case DeviceType::TRANKI_GC: tranki_gc = true; break;
        case DeviceType::TRANKI_E: tranki_e = true; break;
        default: assert(false && "Not such DeviceType"); break;
      }  
    }
    if(mobilephone){
      AtomicsCoupled.first.push_back(create_atomic_DevicePhone("MOBILEPHONE", person.id, "00:00:00:500", "00:01:30:000"));
      dcSUBMODELS += "DevicePhoneMOBILEPHONE"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outMOBILEPHONE, DevicePhoneMOBILEPHONE"+person.id+", DevicePhone_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outMOBILEPHONE, DevicePhoneMOBILEPHONE"+person.id+", DevicePhone_defs::microphoneIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outMOBILEPHONE, DevicePhoneMOBILEPHONE"+person.id+", DevicePhone_defs::networkIn>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhoneMOBILEPHONE"+person.id+", DevicePhone_defs::speakerOut, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhoneMOBILEPHONE"+person.id+", DevicePhone_defs::networkOut, outp_network>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outMOBILEPHONE, sinkDevices, sinkDevices_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outMOBILEPHONE, sinkDevices, sinkDevices_defs::microKeyboardIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outMOBILEPHONE, sinkDevices, sinkDevices_defs::networkIn>,";
    }
    if(landlinephone){
      AtomicsCoupled.first.push_back(create_atomic_DevicePhone("LANDLINEPHONE", person.id, "00:00:00:500", "00:01:30:000"));
      dcSUBMODELS += "DevicePhoneLANDLINEPHONE"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outLANDLINEPHONE, DevicePhoneLANDLINEPHONE"+person.id+", DevicePhone_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outLANDLINEPHONE, DevicePhoneLANDLINEPHONE"+person.id+", DevicePhone_defs::microphoneIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outLANDLINEPHONE, DevicePhoneLANDLINEPHONE"+person.id+", DevicePhone_defs::networkIn>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhoneLANDLINEPHONE"+person.id+", DevicePhone_defs::speakerOut, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhoneLANDLINEPHONE"+person.id+", DevicePhone_defs::networkOut, outp_network>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outLANDLINEPHONE, sinkDevices, sinkDevices_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outLANDLINEPHONE, sinkDevices, sinkDevices_defs::microKeyboardIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outLANDLINEPHONE, sinkDevices, sinkDevices_defs::networkIn>,";
    }
    if(privatelinephone){
      AtomicsCoupled.first.push_back(create_atomic_DevicePhone("PRIVATELINEPHONE", person.id, "00:00:00:500", "00:01:30:000"));
      dcSUBMODELS += "DevicePhonePRIVATELINEPHONE"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outPRIVATELINEPHONE, DevicePhonePRIVATELINEPHONE"+person.id+", DevicePhone_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outPRIVATELINEPHONE, DevicePhonePRIVATELINEPHONE"+person.id+", DevicePhone_defs::microphoneIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outPRIVATELINEPHONE, DevicePhonePRIVATELINEPHONE"+person.id+", DevicePhone_defs::networkIn>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhonePRIVATELINEPHONE"+person.id+", DevicePhone_defs::speakerOut, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhonePRIVATELINEPHONE"+person.id+", DevicePhone_defs::networkOut, outp_network>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outPRIVATELINEPHONE, sinkDevices, sinkDevices_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outPRIVATELINEPHONE, sinkDevices, sinkDevices_defs::microKeyboardIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outPRIVATELINEPHONE, sinkDevices, sinkDevices_defs::networkIn>,";
    }
    if(satelitephone){
      AtomicsCoupled.first.push_back(create_atomic_DevicePhone("SATELLITE_PHONE", person.id, "00:00:00:500", "00:01:30:000"));
      dcSUBMODELS += "DevicePhoneSATELLITE_PHONE"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outSATELLITE_PHONE, DevicePhoneSATELLITE_PHONE"+person.id+", DevicePhone_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outSATELLITE_PHONE, DevicePhoneSATELLITE_PHONE"+person.id+", DevicePhone_defs::microphoneIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outSATELLITE_PHONE, DevicePhoneSATELLITE_PHONE"+person.id+", DevicePhone_defs::networkIn>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhoneSATELLITE_PHONE"+person.id+", DevicePhone_defs::speakerOut, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhoneSATELLITE_PHONE"+person.id+", DevicePhone_defs::networkOut, outp_network>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outSATELLITE_PHONE, sinkDevices, sinkDevices_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outSATELLITE_PHONE, sinkDevices, sinkDevices_defs::microKeyboardIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outSATELLITE_PHONE, sinkDevices, sinkDevices_defs::networkIn>,";
    }
    if(radiological_group_device){
      AtomicsCoupled.first.push_back(create_atomic_DevicePhone("RADIOLOGICAL_GROUP_DEVICE", person.id, "00:00:00:500", "00:01:30:000"));
      dcSUBMODELS += "DevicePhoneRADIOLOGICAL_GROUP_DEVICE"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outRADIOLOGICAL_GROUP_DEVICE, DevicePhoneRADIOLOGICAL_GROUP_DEVICE"+person.id+", DevicePhone_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outRADIOLOGICAL_GROUP_DEVICE, DevicePhoneRADIOLOGICAL_GROUP_DEVICE"+person.id+", DevicePhone_defs::microphoneIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outRADIOLOGICAL_GROUP_DEVICE, DevicePhoneRADIOLOGICAL_GROUP_DEVICE"+person.id+", DevicePhone_defs::networkIn>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhoneRADIOLOGICAL_GROUP_DEVICE"+person.id+", DevicePhone_defs::speakerOut, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhoneRADIOLOGICAL_GROUP_DEVICE"+person.id+", DevicePhone_defs::networkOut, outp_network>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outRADIOLOGICAL_GROUP_DEVICE, sinkDevices, sinkDevices_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outRADIOLOGICAL_GROUP_DEVICE, sinkDevices, sinkDevices_defs::microKeyboardIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outRADIOLOGICAL_GROUP_DEVICE, sinkDevices, sinkDevices_defs::networkIn>,";
    }
    if(beeper){
      pair<vector<string>,vector<string>> TEXTMSGDEVICE = TextDeviceCoupledModel("BEEPER", person.id);
      for(int j = 0; j<TEXTMSGDEVICE.first.size(); j++){
          AtomicsCoupled.first.push_back(TEXTMSGDEVICE.first[j]);
      }
      AtomicsCoupled.second.push_back(TEXTMSGDEVICE.second);
      dcSUBMODELS += "BEEPER"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outBEEPER, BEEPER"+person.id+", inp_DevicesState>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outBEEPER, BEEPER"+person.id+", inp_fromKeyboard>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outBEEPER, BEEPER"+person.id+", inp_network>,";          
      dcEOC += "cadmium::modeling::EOC<BEEPER"+person.id+", outp_toScreen, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<BEEPER"+person.id+", outp_network, outp_network>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outBEEPER, sinkDevices, sinkDevices_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outBEEPER, sinkDevices, sinkDevices_defs::microKeyboardIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outBEEPER, sinkDevices, sinkDevices_defs::networkIn>,";
    }
    if(email){
      pair<vector<string>,vector<string>> TEXTMSGDEVICE = TextDeviceCoupledModel("EMAIL", person.id);
      for(int j = 0; j<TEXTMSGDEVICE.first.size(); j++){
          AtomicsCoupled.first.push_back(TEXTMSGDEVICE.first[j]);
      }
      AtomicsCoupled.second.push_back(TEXTMSGDEVICE.second);
      dcSUBMODELS += "EMAIL"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outEMAIL, EMAIL"+person.id+", inp_DevicesState>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outEMAIL, EMAIL"+person.id+", inp_fromKeyboard>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outEMAIL, EMAIL"+person.id+", inp_network>,";          
      dcEOC += "cadmium::modeling::EOC<EMAIL"+person.id+", outp_toScreen, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<EMAIL"+person.id+", outp_network, outp_network>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outEMAIL, sinkDevices, sinkDevices_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outEMAIL, sinkDevices, sinkDevices_defs::microKeyboardIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outEMAIL, sinkDevices, sinkDevices_defs::networkIn>,";
    }
    if(fax){
      pair<vector<string>,vector<string>> TEXTMSGDEVICE = TextDeviceCoupledModel("FAX", person.id);
      for(int j = 0; j<TEXTMSGDEVICE.first.size(); j++){
          AtomicsCoupled.first.push_back(TEXTMSGDEVICE.first[j]);
      }
      AtomicsCoupled.second.push_back(TEXTMSGDEVICE.second);
      dcSUBMODELS += "FAX"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outFAX, FAX"+person.id+", inp_DevicesState>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outFAX, FAX"+person.id+", inp_fromKeyboard>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outFAX, FAX"+person.id+", inp_network>,";           
      dcEOC += "cadmium::modeling::EOC<FAX"+person.id+", outp_toScreen, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<FAX"+person.id+", outp_network, outp_network>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outFAX, sinkDevices, sinkDevices_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outFAX, sinkDevices, sinkDevices_defs::microKeyboardIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outFAX, sinkDevices, sinkDevices_defs::networkIn>,";
    }
    if(radio_reman){
      AtomicsCoupled.first.push_back(create_atomic_DeviceRadio("RADIO_REMAN", person.id, "00:00:00:500"));
      dcSUBMODELS += "DeviceRadioRADIO_REMAN"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outRADIO_REMAN, DeviceRadioRADIO_REMAN"+person.id+", DeviceRadio_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outRADIO_REMAN, DeviceRadioRADIO_REMAN"+person.id+", DeviceRadio_defs::microphoneIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outRADIO_REMAN, DeviceRadioRADIO_REMAN"+person.id+", DeviceRadio_defs::networkIn>,";
      dcEOC += "cadmium::modeling::EOC<DeviceRadioRADIO_REMAN"+person.id+", DeviceRadio_defs::speakerOut, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<DeviceRadioRADIO_REMAN"+person.id+", DeviceRadio_defs::networkOut, outp_network>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outRADIO_REMAN, sinkDevices, sinkDevices_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outRADIO_REMAN, sinkDevices, sinkDevices_defs::microKeyboardIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outRADIO_REMAN, sinkDevices, sinkDevices_defs::networkIn>,";
    }
    if(radio_remer){
      AtomicsCoupled.first.push_back(create_atomic_DeviceRadio("RADIO_REMER", person.id, "00:00:00:500"));
      dcSUBMODELS += "DeviceRadioRADIO_REMER"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outRADIO_REMER, DeviceRadioRADIO_REMER"+person.id+", DeviceRadio_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outRADIO_REMER, DeviceRadioRADIO_REMER"+person.id+", DeviceRadio_defs::microphoneIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outRADIO_REMER, DeviceRadioRADIO_REMER"+person.id+", DeviceRadio_defs::networkIn>,";
      dcEOC += "cadmium::modeling::EOC<DeviceRadioRADIO_REMER"+person.id+", DeviceRadio_defs::speakerOut, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<DeviceRadioRADIO_REMER"+person.id+", DeviceRadio_defs::networkOut, outp_network>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outRADIO_REMER, sinkDevices, sinkDevices_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outRADIO_REMER, sinkDevices, sinkDevices_defs::microKeyboardIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outRADIO_REMER, sinkDevices, sinkDevices_defs::networkIn>,";
    }
    if(tranki_gc){
      AtomicsCoupled.first.push_back(create_atomic_DevicePhoneMultiple("TRANKI_GC", person.id, "00:00:00:500", "00:01:30:000"));
      dcSUBMODELS += "DevicePhoneMultipleTRANKI_GC"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outTRANKI_GC, DevicePhoneMultipleTRANKI_GC"+person.id+", DevicePhoneMultiple_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outTRANKI_GC, DevicePhoneMultipleTRANKI_GC"+person.id+", DevicePhoneMultiple_defs::microphoneIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outTRANKI_GC, DevicePhoneMultipleTRANKI_GC"+person.id+", DevicePhoneMultiple_defs::networkIn>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhoneMultipleTRANKI_GC"+person.id+", DevicePhoneMultiple_defs::speakerOut, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhoneMultipleTRANKI_GC"+person.id+", DevicePhoneMultiple_defs::networkOut, outp_network>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outTRANKI_GC, sinkDevices, sinkDevices_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outTRANKI_GC, sinkDevices, sinkDevices_defs::microKeyboardIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outTRANKI_GC, sinkDevices, sinkDevices_defs::networkIn>,";
    }
    if(tranki_e){
      AtomicsCoupled.first.push_back(create_atomic_DevicePhoneMultiple("TRANKI_E", person.id, "00:00:00:500", "00:01:30:000"));
      dcSUBMODELS += "DevicePhoneMultipleTRANKI_E"+person.id+",";
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outTRANKI_E, DevicePhoneMultipleTRANKI_E"+person.id+", DevicePhoneMultiple_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outTRANKI_E, DevicePhoneMultipleTRANKI_E"+person.id+", DevicePhoneMultiple_defs::microphoneIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outTRANKI_E, DevicePhoneMultipleTRANKI_E"+person.id+", DevicePhoneMultiple_defs::networkIn>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhoneMultipleTRANKI_E"+person.id+", DevicePhoneMultiple_defs::speakerOut, outp_out_com>,";
      dcEOC += "cadmium::modeling::EOC<DevicePhoneMultipleTRANKI_E"+person.id+", DevicePhoneMultiple_defs::networkOut, outp_network>,";
    }else{
      dcIC += "cadmium::modeling::IC<filterDevicesState"+person.id+", filterDevicesState_defs::outTRANKI_E, sinkDevices, sinkDevices_defs::setStateIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesMicroKeyboard, filterDevicesMicroKeyboard_defs::outTRANKI_E, sinkDevices, sinkDevices_defs::microKeyboardIn>,";
      dcIC += "cadmium::modeling::IC<filterDevicesNetwork"+person.id+", filterDevicesNetwork_defs::outTRANKI_E, sinkDevices, sinkDevices_defs::networkIn>,";
    }    
    /************************/
    if(!dcSUBMODELS.empty()) dcSUBMODELS.pop_back();
    if(!dcEIC.empty()) dcEIC.pop_back();
    if(!dcEOC.empty()) dcEOC.pop_back();
    if(!dcIC.empty()) dcIC.pop_back();        
    DEVICESCOUPLED[0] += dcIPORTS;
    DEVICESCOUPLED[1] += dcOPORTS;
    DEVICESCOUPLED[2] += dcSUBMODELS;
    DEVICESCOUPLED[3] += dcEIC;
    DEVICESCOUPLED[4] += dcEOC;
    DEVICESCOUPLED[5] += dcIC;         
    close_coupled(DEVICESCOUPLED);
    AtomicsCoupled.second.push_back(DEVICESCOUPLED);
    //cout << "sale de DevicesCoupledModel" << endl;
    return AtomicsCoupled;   
}

pair<vector<string>,vector<vector<string>>> PersonCoupledModel(string file, DecisionMakerBehaviour person){

    //cout << "entra en PersonCoupledModel" << endl;
    pair<vector<string>, vector<vector<string>>> AtomicsCoupled;

    AtomicsCoupled.first.push_back(create_atomic_filterCommandGenerator(person.id));
    AtomicsCoupled.first.push_back(create_atomic_filterPeopleInLocation(person.id));
    AtomicsCoupled.first.push_back(create_atomic_switchFace2Face(person.id));
    AtomicsCoupled.first.push_back(create_atomic_answerPersonTask("IN_PERSON",person.id));
    AtomicsCoupled.first.push_back(create_atomic_sendCommandInPerson("IN_PERSON",person.id));
    AtomicsCoupled.first.push_back(create_atomic_devicesSwitch(person.id));

    pair<vector<string>,vector<string>> BEHAVIORRULES = BehaviorRulesCoupledModel(file, person);
    for(int j = 0; j<BEHAVIORRULES.first.size(); j++){
        AtomicsCoupled.first.push_back(BEHAVIORRULES.first[j]);
    }
    AtomicsCoupled.second.push_back(BEHAVIORRULES.second);

    pair<vector<string>,vector<string>>  SENDINGBEHAVIOR = SendingBehaviorCoupledModel(person);
    for(int j = 0; j<SENDINGBEHAVIOR.first.size(); j++){
        AtomicsCoupled.first.push_back(SENDINGBEHAVIOR.first[j]);
    }
    AtomicsCoupled.second.push_back(SENDINGBEHAVIOR.second);

    pair<vector<string>,vector<string>>  RECEIVINGBEHAVIOR = ReceivingBehaviorCoupledModel(person);
    for(int j = 0; j<RECEIVINGBEHAVIOR.first.size(); j++){
        AtomicsCoupled.first.push_back(RECEIVINGBEHAVIOR.first[j]);
    }
    AtomicsCoupled.second.push_back(RECEIVINGBEHAVIOR.second);

    vector<string> PERSONCOUPLED = open_coupled("PERSON"+person.id);
    string pIPORTS = string("inp_generator, inp_peopleInMyLocation, inp_atmosphere, inp_in_com");
    string pOPORTS = string("outp_taskDeviceFinished, outp_taskActionFinished, outp_myLocation, outp_atmosphere,outp_out_com");
    string pSUBMODELS = "filterCommandGenerator"+person.id+",filterPeopleInLocation"+person.id;
    pSUBMODELS += ",switchFace2Face"+person.id+",answerPersonTaskIN_PERSON"+person.id+",sendCommandInPersonIN_PERSON"+person.id;
    pSUBMODELS += ",BEHAVIORRULES"+person.id+",devicesSwitch"+person.id+",SENDINGBEHAVIOR"+person.id+",RECEIVINGBEHAVIOR"+person.id;
    string pEIC= "cadmium::modeling::EIC<inp_atmosphere, switchFace2Face"+person.id+", switchFace2Face_defs::communicationIn>,";
    pEIC += "cadmium::modeling::EIC<inp_generator, filterCommandGenerator"+person.id+", filterCommandGenerator_defs::in>,";
    pEIC += "cadmium::modeling::EIC<inp_peopleInMyLocation, filterPeopleInLocation"+person.id+", filterPeopleInLocation_defs::in>,";
    pEIC += "cadmium::modeling::EIC<inp_in_com, devicesSwitch"+person.id+", devicesSwitch_defs::communicationIn>";

    string pEOC = "cadmium::modeling::EOC<BEHAVIORRULES"+person.id+", outp_myLocation, outp_myLocation>,";
    pEOC += "cadmium::modeling::EOC<BEHAVIORRULES"+person.id+", outp_taskActionFinished, outp_taskActionFinished>,";
    pEOC += "cadmium::modeling::EOC<RECEIVINGBEHAVIOR"+person.id+", outp_out_com, outp_out_com>,";
    pEOC += "cadmium::modeling::EOC<SENDINGBEHAVIOR"+person.id+", outp_out_com, outp_out_com>,";
    pEOC += "cadmium::modeling::EOC<RECEIVINGBEHAVIOR"+person.id+", outp_taskDeviceFinished, outp_taskDeviceFinished>,";
    pEOC += "cadmium::modeling::EOC<SENDINGBEHAVIOR"+person.id+", outp_taskDeviceFinished, outp_taskDeviceFinished>,";
    pEOC += "cadmium::modeling::EOC<answerPersonTaskIN_PERSON"+person.id+", answerPersonTask_defs::toPerson, outp_atmosphere>,";
    pEOC += "cadmium::modeling::EOC<sendCommandInPersonIN_PERSON"+person.id+", sendCommandInPerson_defs::communicationOut, outp_atmosphere>,";
    pEOC += "cadmium::modeling::EOC<answerPersonTaskIN_PERSON"+person.id+", answerPersonTask_defs::taskFinisedAcknow, outp_taskDeviceFinished>,";
    pEOC += "cadmium::modeling::EOC<sendCommandInPersonIN_PERSON"+person.id+", sendCommandInPerson_defs::taskFinisedAcknow, outp_taskDeviceFinished>";

    string pIC = "cadmium::modeling::IC<filterCommandGenerator"+person.id+", filterCommandGenerator_defs::out, BEHAVIORRULES"+person.id+", inp_generator>,";
    pIC += "cadmium::modeling::IC<filterPeopleInLocation"+person.id+", filterPeopleInLocation_defs::out, BEHAVIORRULES"+person.id+", inp_peopleInMyLocation>,";

    pIC += "cadmium::modeling::IC<switchFace2Face"+person.id+", switchFace2Face_defs::decideOut, BEHAVIORRULES"+person.id+", inp_decideInfo>,";
    pIC += "cadmium::modeling::IC<devicesSwitch"+person.id+", devicesSwitch_defs::decideOut, BEHAVIORRULES"+person.id+", inp_decideInfo>,";
    pIC += "cadmium::modeling::IC<SENDINGBEHAVIOR"+person.id+", outp_taskDeviceFinished, BEHAVIORRULES"+person.id+", inp_taskDeviceFinished>,";
    pIC += "cadmium::modeling::IC<RECEIVINGBEHAVIOR"+person.id+", outp_taskDeviceFinished, BEHAVIORRULES"+person.id+", inp_taskDeviceFinished>,";
    pIC += "cadmium::modeling::IC<answerPersonTaskIN_PERSON"+person.id+", answerPersonTask_defs::taskFinisedAcknow, BEHAVIORRULES"+person.id+", inp_taskDeviceFinished>,";
    pIC += "cadmium::modeling::IC<sendCommandInPersonIN_PERSON"+person.id+", sendCommandInPerson_defs::taskFinisedAcknow, BEHAVIORRULES"+person.id+", inp_taskDeviceFinished>,";

    pIC += "cadmium::modeling::IC<SENDINGBEHAVIOR"+person.id+", outp_taskDeviceFinished, devicesSwitch"+person.id+", devicesSwitch_defs::setDecideIn>,";
    pIC += "cadmium::modeling::IC<RECEIVINGBEHAVIOR"+person.id+", outp_taskDeviceFinished, devicesSwitch"+person.id+", devicesSwitch_defs::setDecideIn>,";
    pIC += "cadmium::modeling::IC<answerPersonTaskIN_PERSON"+person.id+", answerPersonTask_defs::taskFinisedAcknow, switchFace2Face"+person.id+", switchFace2Face_defs::setDecideIn>,";
    pIC += "cadmium::modeling::IC<sendCommandInPersonIN_PERSON"+person.id+", sendCommandInPerson_defs::taskFinisedAcknow, switchFace2Face"+person.id+", switchFace2Face_defs::setDecideIn>,";


    pIC += "cadmium::modeling::IC<devicesSwitch"+person.id+", devicesSwitch_defs::sendOut, SENDINGBEHAVIOR"+person.id+", inp_in_com>,";
    pIC += "cadmium::modeling::IC<devicesSwitch"+person.id+", devicesSwitch_defs::answerOut, RECEIVINGBEHAVIOR"+person.id+", inp_in_com>,";
    pIC += "cadmium::modeling::IC<switchFace2Face"+person.id+", switchFace2Face_defs::sendOut, sendCommandInPersonIN_PERSON"+person.id+", sendCommandInPerson_defs::communicationIn>,";
    pIC += "cadmium::modeling::IC<switchFace2Face"+person.id+", switchFace2Face_defs::answerOut, answerPersonTaskIN_PERSON"+person.id+", answerPersonTask_defs::fromPerson>,";

    pIC += "cadmium::modeling::IC<BEHAVIORRULES"+person.id+", outp_send_device, SENDINGBEHAVIOR"+person.id+", inp_task_CWD>,";
    pIC += "cadmium::modeling::IC<BEHAVIORRULES"+person.id+", outp_answer_device, RECEIVINGBEHAVIOR"+person.id+", inp_task>,";
    pIC += "cadmium::modeling::IC<BEHAVIORRULES"+person.id+", outp_send_device, devicesSwitch"+person.id+", devicesSwitch_defs::setSendIn>,";
    pIC += "cadmium::modeling::IC<BEHAVIORRULES"+person.id+", outp_answer_device, devicesSwitch"+person.id+", devicesSwitch_defs::setAnswerIn>,";

    pIC += "cadmium::modeling::IC<BEHAVIORRULES"+person.id+", outp_send_inperson, sendCommandInPersonIN_PERSON"+person.id+", sendCommandInPerson_defs::task>,";
    pIC += "cadmium::modeling::IC<BEHAVIORRULES"+person.id+", outp_answer_inperson, answerPersonTaskIN_PERSON"+person.id+", answerPersonTask_defs::task>,";
    pIC += "cadmium::modeling::IC<BEHAVIORRULES"+person.id+", outp_send_inperson, switchFace2Face"+person.id+", switchFace2Face_defs::setSendIn>,";
    pIC += "cadmium::modeling::IC<BEHAVIORRULES"+person.id+", outp_answer_inperson, switchFace2Face"+person.id+", switchFace2Face_defs::setAnswerIn>";

      PERSONCOUPLED[0] += pIPORTS;
      PERSONCOUPLED[1] += pOPORTS;
      PERSONCOUPLED[2] += pSUBMODELS;
      PERSONCOUPLED[3] += pEIC;
      PERSONCOUPLED[4] += pEOC; 
      PERSONCOUPLED[5] += pIC;           
      close_coupled(PERSONCOUPLED);
    AtomicsCoupled.second.push_back(PERSONCOUPLED);
    //cout << AtomicsCoupled.first.size() << endl;
    //for(int j = 0; j<AtomicsCoupled.first.size(); j++){
    //    cout << AtomicsCoupled.first[j] << endl;
   //}
    //cout << "sale de PersonCoupledModel" << endl;
    return  AtomicsCoupled;
    }

#endif //FUCTIONS_TO_INSTANTIATE_COUPLEDS_HPP
