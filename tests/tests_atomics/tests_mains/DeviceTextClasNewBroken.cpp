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

//This file include DeviceTextClasNewBroken test

#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

#include <cadmium/modeling/coupled_model.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/engine/pdevs_runner.hpp>

#include "../../DESTimes/include/NDTime.hpp"
#include "../../atomics/DeviceTextClasNewBroken.hpp"
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
class iestream_test_com : public iestream_com<TIME> {
public:
    iestream_test_com(): iestream_com<TIME>("tests_inputs/test_NewBroken.txt") {};
};

/*******************************************/

/****** DeviceTextClasNewBroken atomic model definition *********/
template<typename TIME>
class DeviceTextClasNewBro_test : public DeviceTextClasNewBroken<TIME> {
public:
    DeviceTextClasNewBro_test(): DeviceTextClasNewBroken<TIME>(TIME("00:00:00:500")) {};
};

/*******************************************/

/****** DeviceTextClasNewBroken coupled model definition *********/

using iports = std::tuple<>;
struct output_port_new : public cadmium::out_port<Communication>{};
struct output_port_broken : public cadmium::out_port<Communication>{};
using oports = std::tuple<output_port_new,output_port_broken>;
using submodels=cadmium::modeling::models_tuple<iestream_test_com, DeviceTextClasNewBro_test>;
using eics=std::tuple<>;
using eocs=std::tuple<
    cadmium::modeling::EOC<DeviceTextClasNewBro_test, DeviceTextClasNewBroken_defs::newOut, output_port_new>,
    cadmium::modeling::EOC<DeviceTextClasNewBro_test, DeviceTextClasNewBroken_defs::brokenOut, output_port_broken>
>;
using ics=std::tuple<
    cadmium::modeling::IC<iestream_test_com, out_p_iestream_com, DeviceTextClasNewBro_test, DeviceTextClasNewBroken_defs::in>
>;

template<typename TIME>
using top_model=cadmium::modeling::coupled_model<TIME, iports, oports, submodels, eics, eocs, ics>;

/*******************************************/

/*************** Loggers *******************/

namespace {
    std::ofstream out_data("tests_results/DeviceTextClasNewBroken.txt");
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

