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



#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

#include <cadmium/modeling/coupled_model.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/engine/pdevs_runner.hpp>

#include "../../DESTimes/include/NDTime.hpp"
#include "../../atomics/switchFace2Face.hpp"
#include "../../atomics/iestream.hpp"

#include "../../data_structures/struct_types.hpp"
#include "../../data_structures/communication.hpp"


using namespace std;

using namespace nep_structures;

using hclock=chrono::high_resolution_clock;

/****** Istream set answer atomic model definition *********/

using out_p_iestreamCom = iestream_input_defs<Communication>::out;
using out_p_iestreamCWD = iestream_input_defs<CommandWithDevice>::out;
using out_p_iestreamTDF = iestream_input_defs<TaskDeviceFinished>::out;

template<typename TIME>
using iestream_Communication=iestream_input<Communication,TIME>;
template<typename TIME>
using iestream_CommandWithDevice=iestream_input<CommandWithDevice,TIME>;
template<typename TIME>
using iestream_TaskDeviceFinished=iestream_input<TaskDeviceFinished,TIME>;

template<typename TIME>
class iestream_test_set_answer : public iestream_Communication<TIME> {
public:
    iestream_test_set_answer(): iestream_Communication<TIME>("tests_inputs/test_switch_answer.txt") {};
};

/*******************************************/

/****** Istream set send atomic model definition *********/

template<typename TIME>
class iestream_test_set_send : public iestream_CommandWithDevice<TIME> {
public:
    iestream_test_set_send(): iestream_CommandWithDevice<TIME>("tests_inputs/test_switch_send.txt") {};
};

/*******************************************/

/****** Istream set decide atomic model definition *********/

template<typename TIME>
class iestream_test_set_decide : public iestream_TaskDeviceFinished<TIME> {
public:
    iestream_test_set_decide(): iestream_TaskDeviceFinished<TIME>("tests_inputs/test_switch_decide.txt") {};
};

/*******************************************/

/****** Istream communication atomic model definition *********/

template<typename TIME>
class iestream_test_communication : public iestream_Communication<TIME> {
public:
    iestream_test_communication(): iestream_Communication<TIME>("tests_inputs/test_phone_communication.txt") {};
};
/****** switch3OutPerson atomic model definition *********/
using out_send_p = switchFace2Face_defs::sendOut;
using out_answer_p = switchFace2Face_defs::answerOut;
using out_decide_p = switchFace2Face_defs::decideOut;
using in_communication_p = switchFace2Face_defs::communicationIn;
using in_answer_p = switchFace2Face_defs::setAnswerIn;
using in_send_p = switchFace2Face_defs::setSendIn;
using in_decide_p = switchFace2Face_defs::setDecideIn;

template<typename TIME>
class switchFace2Face_person20 : public switchFace2Face<TIME> {
public:
    switchFace2Face_person20(): switchFace2Face<TIME>(DeviceId(DeviceType::IN_PERSON, "20")) {};
};

/*******************************************/

/****** switch3OutPerson coupled model text definition *********/

using iports = std::tuple<>;
struct coupled_ports{
        struct sendOut : public out_port<Communication> {
        };
        struct answerOut : public out_port<Communication> {
        };
        struct decideOut : public out_port<Communication> {
        };
    };
using oports = std::tuple<coupled_ports::sendOut, coupled_ports::answerOut, coupled_ports::decideOut>;
using submodels=cadmium::modeling::models_tuple<iestream_test_communication,iestream_test_set_decide,iestream_test_set_send,iestream_test_set_answer, switchFace2Face_person20>;
using eics=std::tuple<>;
using eocs=std::tuple<
    cadmium::modeling::EOC<switchFace2Face_person20, out_send_p, coupled_ports::sendOut>,
    cadmium::modeling::EOC<switchFace2Face_person20, out_answer_p, coupled_ports::answerOut>,
    cadmium::modeling::EOC<switchFace2Face_person20, out_decide_p, coupled_ports::decideOut>
>;
using ics=std::tuple<
    cadmium::modeling::IC<iestream_test_communication, out_p_iestreamCom, switchFace2Face_person20, in_communication_p>,
    cadmium::modeling::IC<iestream_test_set_decide, out_p_iestreamTDF, switchFace2Face_person20, in_decide_p>,
    cadmium::modeling::IC<iestream_test_set_send, out_p_iestreamCWD, switchFace2Face_person20, in_send_p>,
    cadmium::modeling::IC<iestream_test_set_answer, out_p_iestreamCom, switchFace2Face_person20, in_answer_p>
>;

template<typename TIME>
using top_model=cadmium::modeling::coupled_model<TIME, iports, oports, submodels, eics, eocs, ics>;

/*******************************************/

/*************** Loggers *******************/

namespace {


    std::ofstream out_data("tests_results/switchFace2Face.txt");
    struct oss_sink_provider{
        static std::ostream& sink(){
            return out_data;
        }
    };
}
//using messages_logger=cadmium::logger::logger<cadmium::logger::logger_message_routing, cadmium::logger::verbatim_formatter, cadmium::logger::cout_sink_provider>;

using info=cadmium::logger::logger<cadmium::logger::logger_info, cadmium::logger::verbatim_formatter, oss_sink_provider>;
using debug=cadmium::logger::logger<cadmium::logger::logger_debug, cadmium::logger::verbatim_formatter, oss_sink_provider>;
using state=cadmium::logger::logger<cadmium::logger::logger_state, cadmium::logger::verbatim_formatter, oss_sink_provider>;
using log_messages=cadmium::logger::logger<cadmium::logger::logger_messages, cadmium::logger::verbatim_formatter, oss_sink_provider>;
using routing=cadmium::logger::logger<cadmium::logger::logger_message_routing, cadmium::logger::verbatim_formatter, oss_sink_provider>;
using global_time=cadmium::logger::logger<cadmium::logger::logger_global_time, cadmium::logger::verbatim_formatter, oss_sink_provider>;
using local_time=cadmium::logger::logger<cadmium::logger::logger_local_time, cadmium::logger::verbatim_formatter, oss_sink_provider>;
using log_all=cadmium::logger::multilogger<info, debug, state, log_messages, routing, global_time, local_time>;

//using log_states=cadmium::logger::logger<cadmium::logger::logger_state, cadmium::logger::verbatim_formatter, oss_sink_provider>;
//using log_msg=cadmium::logger::logger<cadmium::logger::logger_messages, cadmium::logger::verbatim_formatter, oss_sink_provider>;
//using log_gt=cadmium::logger::logger<cadmium::logger::logger_global_time, cadmium::logger::verbatim_formatter, oss_sink_provider>;
using logger_top=cadmium::logger::multilogger<log_messages , state , global_time>;
/*******************************************/
/*******************************************/

int main(){
    auto start = hclock::now(); //to measure simulation execution time

    cadmium::engine::runner<NDTime, top_model, logger_top> r{{0}};
    r.runUntil({3000});

    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Simulation took:" << elapsed << "sec" << endl;
    return 0;
}