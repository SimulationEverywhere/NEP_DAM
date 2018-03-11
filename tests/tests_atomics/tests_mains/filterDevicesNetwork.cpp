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

//This file include iestream test

#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

#include <cadmium/modeling/coupled_model.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/engine/pdevs_runner.hpp>

#include "../../DESTimes/include/NDTime.hpp"
#include "../../atomics/filterDevicesNetwork.hpp"
#include "../../atomics/iestream.hpp"

#include "../../data_structures/struct_types.hpp"
#include "../../data_structures/communication.hpp"
#include "../../data_structures/nep_model_enum_types.hpp"


using namespace std;

using namespace nep_structures;
using namespace nep_model_enum_types;


using hclock=chrono::high_resolution_clock;

/****** Istream atomic model definition *********/

using out_p_iestream = iestream_input_defs<Communication>::out;

template<typename TIME>
using iestream_communication=iestream_input<Communication,TIME>;

template<typename TIME>
class iestream_test : public iestream_communication<TIME> {
public:
    iestream_test(): iestream_communication<TIME>("tests_inputs/test_filterDevicesNetwork.txt") {};
};

/*******************************************/

/****** filterDevicesNetwork atomic model definition *********/

template<typename TIME>
class filterDevicesNetwork_1 : public filterDevicesNetwork<TIME> {
public:
    filterDevicesNetwork_1(): filterDevicesNetwork<TIME>("1") {};
};

/*******************************************/

/****** filterDeviceTypeFrom coupled model text definition *********/
struct outP_MOBILEPHONE : public cadmium::out_port<Communication> {
};
struct outP_LANDLINEPHONE : public cadmium::out_port<Communication> {
};
struct outP_EMAIL : public cadmium::out_port<Communication> {
};
struct outP_FAX : public cadmium::out_port<Communication> {
};
struct outP_BEEPER : public cadmium::out_port<Communication> {
};
struct outP_RADIO_REMAN : public cadmium::out_port<Communication> {
};
struct outP_RADIO_REMER : public cadmium::out_port<Communication> {
};
struct outP_RADIOLOGICAL_GROUP_DEVICE : public cadmium::out_port<Communication> {
};
struct outP_TRANKI_GC : public cadmium::out_port<Communication> {
};
struct outP_TRANKI_E : public cadmium::out_port<Communication> {
};
struct outP_SATELLITE_PHONE : public cadmium::out_port<Communication> {
}; 
struct outP_PRIVATELINEPHONE : public cadmium::out_port<Communication> {
}; 
using iports = std::tuple<>;
using oports = std::tuple<outP_MOBILEPHONE,outP_LANDLINEPHONE, outP_EMAIL, outP_FAX, outP_BEEPER, outP_RADIO_REMAN, outP_RADIO_REMER, outP_RADIOLOGICAL_GROUP_DEVICE, outP_TRANKI_GC, outP_TRANKI_E, outP_SATELLITE_PHONE, outP_PRIVATELINEPHONE>;
using submodels=cadmium::modeling::models_tuple<iestream_test, filterDevicesNetwork_1>;
using eics=std::tuple<>;
using eocs=std::tuple<
    cadmium::modeling::EOC<filterDevicesNetwork_1, filterDevicesNetwork_defs::outMOBILEPHONE, outP_MOBILEPHONE>,    
    cadmium::modeling::EOC<filterDevicesNetwork_1, filterDevicesNetwork_defs::outLANDLINEPHONE, outP_LANDLINEPHONE>,
    cadmium::modeling::EOC<filterDevicesNetwork_1, filterDevicesNetwork_defs::outEMAIL, outP_EMAIL>,
    cadmium::modeling::EOC<filterDevicesNetwork_1, filterDevicesNetwork_defs::outFAX, outP_FAX>,
    cadmium::modeling::EOC<filterDevicesNetwork_1, filterDevicesNetwork_defs::outBEEPER, outP_BEEPER>,
    cadmium::modeling::EOC<filterDevicesNetwork_1, filterDevicesNetwork_defs::outRADIO_REMAN, outP_RADIO_REMAN>,
    cadmium::modeling::EOC<filterDevicesNetwork_1, filterDevicesNetwork_defs::outRADIO_REMER, outP_RADIO_REMER>,
    cadmium::modeling::EOC<filterDevicesNetwork_1, filterDevicesNetwork_defs::outRADIOLOGICAL_GROUP_DEVICE, outP_RADIOLOGICAL_GROUP_DEVICE>,
    cadmium::modeling::EOC<filterDevicesNetwork_1, filterDevicesNetwork_defs::outTRANKI_GC, outP_TRANKI_GC>,
    cadmium::modeling::EOC<filterDevicesNetwork_1, filterDevicesNetwork_defs::outTRANKI_E, outP_TRANKI_E>,
    cadmium::modeling::EOC<filterDevicesNetwork_1, filterDevicesNetwork_defs::outSATELLITE_PHONE, outP_SATELLITE_PHONE>,
    cadmium::modeling::EOC<filterDevicesNetwork_1, filterDevicesNetwork_defs::outPRIVATELINEPHONE, outP_PRIVATELINEPHONE>
>;
using ics=std::tuple<
    cadmium::modeling::IC<iestream_test, out_p_iestream, filterDevicesNetwork_1, filterDevicesNetwork_defs::in>
>;

template<typename TIME>
using top_model=cadmium::modeling::coupled_model<TIME, iports, oports, submodels, eics, eocs, ics>;

/*******************************************/

/*************** Loggers *******************/

namespace {
  std::ofstream out_data("tests_results/filterDevicesNetwork.txt");
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

