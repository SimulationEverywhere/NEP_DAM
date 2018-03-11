#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <assert.h>
#include <algorithm>
#include <limits>

#include <cadmium/modeling/coupled_model.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/engine/pdevs_runner.hpp>

#include "../../atomics/iestream.hpp"

#include "../../../DESTimes/include/NDTime.hpp"
#include "../../../data_structures/nep_model_enum_types.hpp"
#include "../../../data_structures/communication.hpp"
#include "../../../data_structures/struct_types.hpp"
#include "../../../data_structures/decision_maker_behaviour_struct_types_V2.hpp"
#include "../../../data_structures/decision_maker_behaviour_enum_types.hpp"
#include "../../../atomics/PersonBehaviorRules.hpp"

using namespace std;

using namespace nep_structures;
using namespace nep_model_enum_types;


using hclock=chrono::high_resolution_clock;

/****** Istream atomic model definition *********/

using out_p_iestream_com = iestream_input_defs<Communication>::out;
using out_p_iestream_tDF = iestream_input_defs<TaskDeviceFinished>::out;

template<typename TIME>
using iestream_communication=iestream_input<Communication,TIME>;

template<typename TIME>
using iestream_tDF=iestream_input<TaskDeviceFinished,TIME>;

template<typename TIME>
class iestream_request : public iestream_communication<TIME> {
public:
    iestream_request(): iestream_communication<TIME>("requestIn.txt") {};
};

template<typename TIME>
class iestream_taskDF : public iestream_tDF<TIME> {
public:
    iestream_taskDF(): iestream_tDF<TIME>("taskDeviceFinishedIn.txt") {};
};

/*******************************************/

/****** PersonBehaviorRules atomic model definition *********/

using out_p_taskSIP = PersonBehaviorRules_defs::taskSendOutInperson;
using out_p_taskSD = PersonBehaviorRules_defs::taskSendOutDevice;
using out_p_taskAIP = PersonBehaviorRules_defs::taskAnswerOutInperson;
using out_p_taskAD = PersonBehaviorRules_defs::taskAnswerOutDevice;
using out_p_taskD = PersonBehaviorRules_defs::taskDoActionOut;
using in_p_generator = PersonBehaviorRules_defs::generatorIn;
using in_p_request = PersonBehaviorRules_defs::requestIn;
using in_p_taskDeviceFinished = PersonBehaviorRules_defs::taskDeviceFinishedIn;
using in_p_taskActionFinished = PersonBehaviorRules_defs::taskActionFinishedIn;
using in_p_peopleInSameLocation = PersonBehaviorRules_defs::peopleInSameLocationIn;

template<typename TIME>
class PersonBehaviorRules_test : public PersonBehaviorRules<TIME> {
public:
    PersonBehaviorRules_test(): PersonBehaviorRules<TIME>(string("XML_Eg1.xml")) {};
};

/*******************************************/

/****** PersonBehaviorRules coupled model text definition *********/

using iports = std::tuple<>;
struct output_port_taskSIP : public cadmium::out_port<CommandWithDevice>{};
struct output_port_taskAIP : public cadmium::out_port<Communication>{};
struct output_port_taskSD : public cadmium::out_port<CommandWithDevice>{};
struct output_port_taskAD : public cadmium::out_port<Communication>{};
struct output_port_taskD : public cadmium::out_port<Action2Do>{};
using oports = std::tuple<output_port_taskSIP, output_port_taskSD, output_port_taskAIP, output_port_taskAD, output_port_taskD>;
using submodels=cadmium::modeling::models_tuple<iestream_request, iestream_taskDF, PersonBehaviorRules_test>;
using eics=std::tuple<>;
using eocs=std::tuple<
    cadmium::modeling::EOC<PersonBehaviorRules_test, out_p_taskSIP, output_port_taskSIP>,
    cadmium::modeling::EOC<PersonBehaviorRules_test, out_p_taskSD, output_port_taskSD>,
    cadmium::modeling::EOC<PersonBehaviorRules_test, out_p_taskAIP, output_port_taskAIP>,
    cadmium::modeling::EOC<PersonBehaviorRules_test, out_p_taskAD, output_port_taskAD>,
    cadmium::modeling::EOC<PersonBehaviorRules_test, out_p_taskD, output_port_taskD>
>;
using ics=std::tuple<
    cadmium::modeling::IC<iestream_request, out_p_iestream_com, PersonBehaviorRules_test, in_p_request>,
    cadmium::modeling::IC<iestream_taskDF, out_p_iestream_tDF, PersonBehaviorRules_test, in_p_taskDeviceFinished>
>;

template<typename TIME> 
struct TOP : public cadmium::modeling::coupled_model<TIME, iports, oports, submodels, eics, eocs, ics>{};


/*******************************************/

/*************** Loggers *******************/

namespace {

    std::ofstream out_data("DecisionMaker.txt");
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

    cadmium::engine::runner<NDTime, TOP, logger_top> r{{0}};
    r.runUntil({3000000});

    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Simulation took:" << elapsed << "sec" << endl;
    return 0;
}

