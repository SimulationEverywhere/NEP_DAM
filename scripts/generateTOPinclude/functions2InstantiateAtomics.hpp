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

#ifndef FUCTIONS_TO_INSTANTIATE_ATOMICS_HPP
#define FUCTIONS_TO_INSTANTIATE_ATOMICS_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <math.h> 
#include <vector>

using namespace std;

/****ATOMICS****/

//FILTER DEVICES MICRO KEYBOARD DO NOT NEED INSTANTIATION

string create_atomic_filterDevicesNetwork(string id){
	return string("template<typename TIME> \n class filterDevicesNetwork")+id+string(": public filterDevicesNetwork<TIME> { \n public: \n filterDevicesNetwork")+id+string("(): filterDevicesNetwork<TIME>(\"")+id+string("\") {}; \n };");
}

string create_atomic_filterDevicesState(string id){
	return string("template<typename TIME> \n class filterDevicesState")+id+string(": public filterDevicesState<TIME> { \n public: \n filterDevicesState")+id+string("(): filterDevicesState<TIME>(\"")+id+string("\") {}; \n };");
}

//FILTER NETWORKS DO NOT NEED INSTANTIATION

//FILTER NETWORKS STATE DO NOT NEED INSTANTIATION

string create_atomic_DeviceTextInbox(string deviceType, string id, string timeSend, string timeOutOfOrder){
return string("template<typename TIME> \n class DeviceTextInbox")+deviceType+id+string(" : public DeviceTextInbox<TIME> { \n public: \n DeviceTextInbox")+deviceType+id+string("(): DeviceTextInbox<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"")+timeSend+string("\"),TIME(\"")+timeOutOfOrder+string("\")) {}; \n };");
}

string create_atomic_DeviceTextClasNewRead(string deviceType, string id, string timeProc){
	return string("template<typename TIME> \n class DeviceTextClasNewRead")+deviceType+id+string(" : public DeviceTextClasNewRead<TIME> { \n public: \n DeviceTextClasNewRead")+deviceType+id+string("(): DeviceTextClasNewRead<TIME>(TIME(\"")+timeProc+string("\")) {}; \n };");
}

string create_atomic_DeviceTextClasNewBroken(string deviceType, string id, string timeProc){
	return string("template<typename TIME> \n class DeviceTextClasNewBroken")+deviceType+id+string(" : public DeviceTextClasNewBroken<TIME> { \n public: \n DeviceTextClasNewBroken")+deviceType+id+string("(): DeviceTextClasNewBroken<TIME>(TIME(\"")+timeProc+string("\")) {}; \n };");
}

string create_atomic_DeviceTextOutbox(string deviceType, string id, string timeSend, string timeOutOfOrder){
	return string("template<typename TIME> \n class DeviceTextOutbox")+deviceType+id+string(" : public DeviceTextOutbox<TIME> { \n public: \n DeviceTextOutbox")+deviceType+id+string("(): DeviceTextOutbox<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"")+timeSend+string("\"),TIME(\"")+timeOutOfOrder+string("\")) {}; \n };");
}
// PEOPLE IN LOCATION DO NOT NEED INSTANTIATION

// SINK DEVICES DO NOT NEED INSTANTIATION

string create_atomic_DevicePhone(string deviceType, string id, string timeProc, string timeRing){
	return string("template<typename TIME> \n class DevicePhone")+deviceType+id+string(" : public DevicePhone<TIME> { \n public: \n DevicePhone")+deviceType+id+string("(): DevicePhone<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"")+timeProc+string("\"),TIME(\"")+timeRing+string("\")) {}; \n };");
}

string create_atomic_DevicePhoneMultiple(string deviceType, string id, string timeProc, string timeRing){
	return string("template<typename TIME> \n class DevicePhoneMultiple")+deviceType+id+string(" : public DevicePhoneMultiple<TIME> { \n public: \n DevicePhoneMultiple")+deviceType+id+string("(): DevicePhoneMultiple<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"")+timeProc+string("\"),TIME(\"")+timeRing+string("\")) {}; \n };");
}

string create_atomic_DeviceRadio(string deviceType, string id, string timeProc){
	return string("template<typename TIME> \n class DeviceRadio")+deviceType+id+string(" : public DeviceRadio<TIME> { \n public: \n DeviceRadio")+deviceType+id+string("(): DeviceRadio<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"")+timeProc+string("\")) {}; \n };");
}

string create_atomic_NetPhone(string id, string busy, string delay){
	return string(" template<typename TIME> \n class NetPhone")+id+string(" : public NetPhone<TIME> { \n public: \n NetPhone")+id+string("(): NetPhone<TIME>(\"")+id+string("\",TIME(\"")+delay+string("\"),")+busy+string(") {}; \n };");
}

string create_atomic_NetRadio(string id, string interferences, string delay){
	return string(" template<typename TIME> \n class NetRadio")+id+string(" : public NetRadio<TIME> { \n public: \n NetRadio")+id+string("(): NetRadio<TIME>(\"")+id+string("\",TIME(\"")+delay+string("\"),")+interferences+string(") {}; \n };");
}

string create_atomic_NetText(string id, string delay){
	return string(" template<typename TIME> \n class NetText")+id+string(" : public NetText<TIME> { \n public: \n NetText")+id+string("(): NetText<TIME>(\"")+id+string("\",TIME(\"")+delay+string("\")) {} \n };");
}

//filters Inside Person
string create_atomic_filterCommandGenerator(string id){
	return string("template<typename TIME> \n class filterCommandGenerator")+id+string(": public filterCommandGenerator<TIME> { \n public: \n filterCommandGenerator")+id+string("(): filterCommandGenerator<TIME>(\"")+id+string("\") {}; \n };");
}

string create_atomic_filterPeopleInLocation(string id){
	return string("template<typename TIME> \n class filterPeopleInLocation")+id+string(": public filterPeopleInLocation<TIME> { \n public: \n filterPeopleInLocation")+id+string("(): filterPeopleInLocation<TIME>(\"")+id+string("\") {}; \n };");
}

string create_atomic_filterPersonDeviceTypeFrom(string id){
	return string("template<typename TIME> \n class filterPersonDeviceTypeFrom")+id+string(": public filterPersonDeviceTypeFrom<TIME> { \n public: \n filterPersonDeviceTypeFrom")+id+string("(): filterPersonDeviceTypeFrom<TIME>() {}; \n };");
}

string create_atomic_filterPersonDeviceTypeMyId(string id){
	return string("template<typename TIME> \n class filterPersonDeviceTypeMyId")+id+string(": public filterPersonDeviceTypeMyId<TIME> { \n public: \n filterPersonDeviceTypeMyId")+id+string("(): filterPersonDeviceTypeMyId<TIME>() {}; \n };");
}

string create_atomic_filterPersonDeviceTypeTo(string id){
	return string("template<typename TIME> \n class filterPersonDeviceTypeTo")+id+string(": public filterPersonDeviceTypeTo<TIME> { \n public: \n filterPersonDeviceTypeTo")+id+string("(): filterPersonDeviceTypeTo<TIME>() {}; \n };");
}

//
string create_atomic_answerPersonTask(string deviceType, string id){
	return string("template<typename TIME> \n class answerPersonTask")+deviceType+id+string(" : public answerPersonTask<TIME> { \n public: \n answerPersonTask")+deviceType+id+string("(): answerPersonTask<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"00:00:05:000\"),TIME(\"00:00:10:000\"),TIME(\"00:00:15:000\")) {}; \n };");
}

string create_atomic_answerPhoneTask(string deviceType, string id){
	return string("template<typename TIME> \n class answerPhoneTask")+deviceType+id+string(" : public answerPhoneTask<TIME> { \n public: \n answerPhoneTask")+deviceType+id+string("(): answerPhoneTask<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"00:00:15:000\"),TIME(\"00:00:05:000\"),TIME(\"00:00:10:000\")) {}; \n };");
}

string create_atomic_answerRadioTask(string deviceType, string id){
	return string("template<typename TIME> \n class answerRadioTask")+deviceType+id+string(" : public answerRadioTask<TIME> { \n public: \n answerRadioTask")+deviceType+id+string("(): answerRadioTask<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"00:01:00:000\"),TIME(\"00:00:05:000\")) {}; \n };");
}

string create_atomic_callByMultiplePhoneTask(string deviceType, string id){
	return string("template<typename TIME> \n class callByMultiplePhoneTask")+deviceType+id+string(" : public callByMultiplePhoneTask<TIME> { \n public: \n callByMultiplePhoneTask")+deviceType+id+string("(): callByMultiplePhoneTask<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"00:02:00:000\"), TIME(\"00:05:00:000\"),TIME(\"00:00:20:000\"),TIME(\"00:00:05:000\")) {}; \n };");
}

string create_atomic_callByPhoneTask(string deviceType, string id){
	return string("template<typename TIME> \n class callByPhoneTask")+deviceType+id+string(" : public callByPhoneTask<TIME> { \n public: \n callByPhoneTask")+deviceType+id+string("(): callByPhoneTask<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"00:05:00:000\"),TIME(\"00:00:20:000\"),TIME(\"00:00:05:000\")) {}; \n };");
}

string create_atomic_PersonBehaviorRules(string file, string id){
	return string("template<typename TIME> \n class PersonBehaviorRules")+id+string(" : public PersonBehaviorRules<TIME> { \n public: \n PersonBehaviorRules")+id+string("(): PersonBehaviorRules<TIME>(\"")+file+string("\") {}; \n };");
}

string create_atomic_doActionTask(string id, string location){
	return string("template<typename TIME> \n class doActionTask")+id+string(" : public doActionTask<TIME> { \n public: \n doActionTask")+id+string("(): doActionTask<TIME>(\"")+id+string("\", \"")+location+string("\") {}; \n };");
}

string create_atomic_readTextOnDigitalDevice(string deviceType, string id){
	return string("template<typename TIME> \n class readTextOnDigitalDevice")+deviceType+id+string(" : public readTextOnDigitalDevice<TIME> { \n public: \n readTextOnDigitalDevice")+deviceType+id+string("(): readTextOnDigitalDevice<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"00:00:10:000\"),TIME(\"00:02:00:000\"), TIME(\"00:00:05:000\")) {}; \n };");
}

string create_atomic_readTextOnPaperDevice(string deviceType, string id){
	return string("template<typename TIME> \n class readTextOnPaperDevice")+deviceType+id+string(" : public readTextOnPaperDevice<TIME> { \n public: \n readTextOnPaperDevice")+deviceType+id+string("(): readTextOnPaperDevice<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"00:00:10:000\"),TIME(\"00:02:00:000\")) {}; \n };");
}

string create_atomic_sendCommandByRadio(string deviceType, string id){
	return string("template<typename TIME> \n class sendCommandByRadio")+deviceType+id+string(" : public sendCommandByRadio<TIME> { \n public: \n sendCommandByRadio")+deviceType+id+string("(): sendCommandByRadio<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"00:02:00:000\"),TIME(\"00:00:10:000\")) {}; \n };");
}

string create_atomic_sendCommandInPerson(string deviceType, string id){
	return string("template<typename TIME> \n class sendCommandInPerson")+deviceType+id+string(" : public sendCommandInPerson<TIME> { \n public: \n sendCommandInPerson")+deviceType+id+string("(): sendCommandInPerson<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"00:01:00:000\"),TIME(\"00:03:00:000\"),TIME(\"00:00:30:000\"),TIME(\"00:00:05:000\")) {}; \n };");
}

string create_atomic_sendTextTask(string deviceType, string id){
	return string("template<typename TIME> \n class sendTextTask")+deviceType+id+string(" : public sendTextTask<TIME> { \n public: \n sendTextTask")+deviceType+id+string("(): sendTextTask<TIME>(DeviceId(DeviceType::")+deviceType+string(", \"")+id+string("\"),TIME(\"00:00:15:000\"),TIME(\"00:02:00:000\"),TIME(\"00:00:20:000\")) {}; \n };");
}

string create_atomic_devicesSwitch(string id){
	return string("template<typename TIME> \n class devicesSwitch")+id+string(" : public devicesSwitch<TIME> { \n public: \n devicesSwitch")+id+string("(): devicesSwitch<TIME>() {}; \n };");
}

string create_atomic_switchFace2Face(string id){
	return string("template<typename TIME> \n class switchFace2Face")+id+string(" : public switchFace2Face<TIME> { \n public: \n switchFace2Face")+id+string("(): switchFace2Face<TIME>(DeviceId(DeviceType::IN_PERSON,\""+id+"\")) {}; \n };");
}

#endif //FUCTIONS_TO_INSTANTIATE_ATOMICS_HPP
