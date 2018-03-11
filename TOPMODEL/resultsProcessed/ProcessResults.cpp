//
// Created by cris on 28/09/17.
//
#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <assert.h>
#include <algorithm>
#include <limits>
#include <stdlib.h>

using namespace std;

struct LogLine{
    string task;
    string deviceMe;
    string idMe;
    string deviceWith;
    string idWith;
    string broken;
    string command;
    string from;
    string to;
};

istream& operator>> (istream& is, LogLine& l);
ostream& operator<<(ostream& os, const LogLine& l);

using hclock=chrono::high_resolution_clock;

int main(int argc, char ** argv) {
    auto start = hclock::now(); //to measure simulation execution time

  if (argc < 4) {
    cout << "you are using this program with wrong parameters. Te program should be invoked as follow:";
    cout << argv[0] << " \"path to the folder where the results are stored\" \"number of simulations\" \"file name to identify the proccesed results\"" <<  endl;
    return 1;
  } 
    string folder = argv[1];
    cout << folder << endl;
    int numberOfSimulations = stoi(argv[2]);
    ofstream outfiletask;
    outfiletask.open(argv[3]+string("task.txt"));
    ofstream outfilecommand;
    outfilecommand.open(argv[3]+string("command.txt"));
    ofstream outfiledevice;
    outfiledevice.open(argv[3]+string("device.txt"));

    std::vector<string> commands2analize;
    commands2analize.push_back("Establish_Emergency_Level_0");
    commands2analize.push_back("Establish_Emergency_Level_1");
    commands2analize.push_back("Establish_Emergency_Level_2");
    commands2analize.push_back("Establish_Emergency_Level_3");

    std::vector<string> devices2analyze;
    devices2analyze.push_back("IN_PERSON");
    devices2analyze.push_back("MOBILEPHONE");
    devices2analyze.push_back("LANDLINEPHONE");
    devices2analyze.push_back("PRIVATELINEPHONE");
    devices2analyze.push_back("SATELLITE_PHONE");
    devices2analyze.push_back("RADIOLOGICAL_GROUP_DEVICE");
    devices2analyze.push_back("BEEPER");
    devices2analyze.push_back("RADIO_REMAN");
    devices2analyze.push_back("RADIO_REMER");
    devices2analyze.push_back("EMAIL");
    devices2analyze.push_back("FAX");
    devices2analyze.push_back("TRANKI_GC");
    devices2analyze.push_back("TRANKI_E");

    outfiletask << "Simulation PersonId Answer Send Fail" << endl;
    outfilecommand << "Simulation Command #Persons" << endl;
    outfiledevice << "Simulation Device #Uses" << endl;

    for (int i = 1; i <= numberOfSimulations; i++){
        LogLine line;
        vector<LogLine> simulation;
        cout << "processing simulation " << i << endl;
        ifstream infile;
        infile.open(folder+string("TOPOUT")+to_string(i)+string(".txt"));
        if(!infile) {
            cout << "Cannot open input file.\n";
            return 1;
        }
        //infile.get();
        if(infile.eof()==true){
            cout << "file empty" << endl;
            return 1;
        }
        while(infile.eof()!=true){
            infile >> line;
            simulation.push_back(line);
            //cout << line << endl;
        }
        infile.close();
        for(int c = 0; c< commands2analize.size(); c++){
            int countcommand = 0;
            for(int z =1; z<= 832; z++){                
                for (int k = 0; k<simulation.size(); k++){
                    if(simulation[k].task == "ANSWER" && simulation[k].idMe == to_string(z) && simulation[k].command == commands2analize[c]){
                        countcommand++;
                        break;
                    }
                }
            }
            outfilecommand << to_string(i) + " " + commands2analize[c] + " " + to_string(countcommand) << endl;
        }
        for(int c = 0; c< devices2analyze.size(); c++){
            int countdevice = 0;
            for (int k = 0; k<simulation.size(); k++){
                if(simulation[k].deviceMe == devices2analyze[c]){
                    countdevice++;
                }
            }
            outfiledevice << to_string(i) + " " + devices2analyze[c] + " " + to_string(countdevice) << endl;
        }
        for(int z =1; z<= 832; z++){
            //count send/answer task
            int countsend = 0;
            int countanswer = 0;
            int countfail = 0;
            for (int k = 0; k<simulation.size(); k++){
                if(simulation[k].idMe == to_string(z)){
                    if(simulation[k].task == "SEND"){ 
                        countsend++;
                    }else if(simulation[k].task == "ANSWER"){
                        countanswer++;
                    }else{
                        cout << "Error with the task";
                    }
                    if(simulation[k].command == "-"){
                        countfail++;
                    }
                }
            }
            outfiletask << to_string(i)+" "+to_string(z)+" "+to_string(countanswer)+" "+to_string(countsend)+" "+to_string(countfail) << endl;
        }      
    }    
   
    outfiletask.close();
    outfilecommand.close();
    outfiledevice.close();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Process Results took:" << elapsed << "sec" << endl;
    return 0;
};

ostream& operator<<(ostream& os, const LogLine& l){
    os << l.task << " " << l.deviceMe << " " << l.idMe << " " << l.deviceWith << " " << l.idWith << " " << l.broken << " " << l.command << " " << l.from << " " << l.to;
    return os;
}

istream& operator>> (istream& is, LogLine& l){
    is >> l.task;
    is >> l.deviceMe;
    is >> l.idMe;
    is >> l.deviceWith;
    is >> l.idWith;
    is >> l.broken;
    is >> l.command;
    is >> l.from;
    is >> l.to;
    return is;
}
