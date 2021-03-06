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
#include "../../atomics/filterPeopleInLocation.hpp"
#include "../../atomics/iestream.hpp"

#include "../../data_structures/struct_types.hpp"
#include "../../data_structures/communication.hpp"
#include "../../data_structures/nep_model_enum_types.hpp"


using namespace std;

using namespace nep_structures;
using namespace nep_model_enum_types;


using hclock=chrono::high_resolution_clock;

/****** Istream atomic model definition *********/

using out_p_iestream = iestream_input_defs<PeopleClassifiedByLocation>::out;

template<typename TIME>
using iestream_PeopleClassifiedByLocation=iestream_input<PeopleClassifiedByLocation,TIME>;

template<typename TIME>
class iestream_test : public iestream_PeopleClassifiedByLocation<TIME> {
public:
    iestream_test(): iestream_PeopleClassifiedByLocation<TIME>("tests_inputs/test_filterPeopleInLocation.txt") {};
};

/*******************************************/

/****** filterPeopleInLocation atomic model definition *********/

using out_p_filterPeopleInLocation = filterPeopleInLocation_defs::out;
using in_p_filterPeopleInLocation = filterPeopleInLocation_defs::in;

template<typename TIME>
class filterPeopleInLocation_1 : public filterPeopleInLocation<TIME> {
public:
    filterPeopleInLocation_1(): filterPeopleInLocation<TIME>(string("1")) {};
};

/*******************************************/

/****** filterPeopleInLocation coupled model text definition *********/

using iports = std::tuple<>;
struct output_port : public cadmium::out_port<PeopleClassifiedByLocation>{};
using oports = std::tuple<output_port>;
using submodels=cadmium::modeling::models_tuple<iestream_test, filterPeopleInLocation_1>;
using eics=std::tuple<>;
using eocs=std::tuple<
    cadmium::modeling::EOC<filterPeopleInLocation_1, out_p_filterPeopleInLocation, output_port>
>;
using ics=std::tuple<
    cadmium::modeling::IC<iestream_test, out_p_iestream, filterPeopleInLocation_1, in_p_filterPeopleInLocation>
>;

template<typename TIME>
using top_model=cadmium::modeling::coupled_model<TIME, iports, oports, submodels, eics, eocs, ics>;

/*******************************************/

/*************** Loggers *******************/

namespace {


    std::ofstream out_data("tests_results/filterPeopleInLocation.txt");
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

int main(){
    auto start = hclock::now(); //to measure simulation execution time

    cadmium::engine::runner<NDTime, top_model, logger_top> r{{0}};
    r.runUntil({3000});

    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Simulation took:" << elapsed << "sec" << endl;
    return 0;
}

