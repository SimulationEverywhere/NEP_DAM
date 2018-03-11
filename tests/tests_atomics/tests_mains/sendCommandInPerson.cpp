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

//This file include sendCommandInPerson test

#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

#include <cadmium/modeling/coupled_model.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/engine/pdevs_runner.hpp>

#include "../../DESTimes/include/NDTime.hpp"
#include "../../atomics/sendCommandInPerson.hpp"
#include "../../atomics/iestream.hpp"

#include "../../data_structures/struct_types.hpp"
#include "../../data_structures/communication.hpp"
#include "../../data_structures/nep_model_enum_types.hpp"


using namespace std;

using namespace nep_structures;
using namespace nep_model_enum_types;


using hclock=chrono::high_resolution_clock;

/****** Istream atomic model definition *********/

using out_p_iestream_com = iestream_input_defs<Communication>::out;

template<typename TIME>
using iestream_com=iestream_input<Communication,TIME>;

template<typename TIME>
class iestream_test_com_n : public iestream_com<TIME> {
public:
    iestream_test_com_n(): iestream_com<TIME>("tests_inputs/test_sendCommandInPersonCom.txt") {};
};

using out_p_iestream_CWD = iestream_input_defs<CommandWithDevice>::out;
template<typename TIME>
using iestream_CWD=iestream_input<CommandWithDevice,TIME>;
template<typename TIME>
class iestream_test_CWD : public iestream_CWD<TIME> {
public:
    iestream_test_CWD(): iestream_CWD<TIME>("tests_inputs/test_sendCommandInPersonCWD.txt") {};
};
/*******************************************/

/****** sendCommandInPerson atomic model definition *********/

using out_p_communicationOut= sendCommandInPerson_defs::communicationOut;
using out_p_taskFinisedAcknow= sendCommandInPerson_defs::taskFinisedAcknow;
using in_p_communicationIn = sendCommandInPerson_defs::communicationIn;
using in_p_task = sendCommandInPerson_defs::task;

template<typename TIME>
class sendCommandInPerson_test : public sendCommandInPerson<TIME> {
public:
    sendCommandInPerson_test(): sendCommandInPerson<TIME>(DeviceId(DeviceType::IN_PERSON, "1"),TIME("00:00:30:000"),TIME("00:00:50:000"),TIME("00:00:02:000"),TIME("00:00:03:000")) {};
};

/*******************************************/

/****** sendCommandInPerson coupled model definition *********/

using iports = std::tuple<>;
struct output_port_communicationOut : public cadmium::out_port<Communication>{};
struct output_port_taskFinisedAcknow : public cadmium::out_port<TaskDeviceFinished>{};
using oports = std::tuple<output_port_communicationOut,output_port_taskFinisedAcknow>;
using submodels=cadmium::modeling::models_tuple<iestream_test_com_n,iestream_test_CWD, sendCommandInPerson_test>;
using eics=std::tuple<>;
using eocs=std::tuple<
    cadmium::modeling::EOC<sendCommandInPerson_test, out_p_communicationOut, output_port_communicationOut>,
    cadmium::modeling::EOC<sendCommandInPerson_test, out_p_taskFinisedAcknow, output_port_taskFinisedAcknow>
>;
using ics=std::tuple<
    cadmium::modeling::IC<iestream_test_com_n, out_p_iestream_com, sendCommandInPerson_test, in_p_communicationIn>,
    cadmium::modeling::IC<iestream_test_CWD, out_p_iestream_CWD, sendCommandInPerson_test, in_p_task>
>;

template<typename TIME>
using top_model=cadmium::modeling::coupled_model<TIME, iports, oports, submodels, eics, eocs, ics>;

/*******************************************/

/*************** Loggers *******************/

namespace {
    std::ofstream out_data("tests_results/sendCommandInPerson.txt");
    struct oss_sink_provider{
        static std::ostream& sink(){
            return out_data;
        }
    };
}
using log_states=cadmium::logger::logger<cadmium::logger::logger_state, cadmium::logger::verbatim_formatter, oss_sink_provider>;
using log_msg=cadmium::logger::logger<cadmium::logger::logger_messages, cadmium::logger::verbatim_formatter, oss_sink_provider>;
using log_gt=cadmium::logger::logger<cadmium::logger::logger_global_time, cadmium::logger::verbatim_formatter, oss_sink_provider>;
using logger_top=cadmium::logger::multilogger<log_states, log_msg, log_gt>;
/*******************************************/

int main(){
    auto start = hclock::now(); //to measure simulation execution time

    cadmium::engine::runner<NDTime, top_model, logger_top> r{{0}};
    r.runUntil({3000});

    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Simulation took:" << elapsed << "sec" << endl;
    return 0;
}

