/******************************************************************************
 *    Copyright (c) Open Connectivity Foundation (OCF), AllJoyn Open Source
 *    Project (AJOSP) Contributors and others.
 *
 *    SPDX-License-Identifier: Apache-2.0
 *
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Copyright (c) Open Connectivity Foundation and Contributors to AllSeen
 *    Alliance. All rights reserved.
 *
 *    Copyright (c) V2 Systems, LLC.  All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for
 *    any purpose with or without fee is hereby granted, provided that the
 *    above copyright notice and this permission notice appear in all
 *    copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *    PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *    PERFORMANCE OF THIS SOFTWARE.
******************************************************************************/

// INCLUDES
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include "include/DistributedEnergyResource.h"
#include "include/CommandLineInterface.h"
#include "include/Operator.h"
#include "include/SmartGridDevice.h"
#include "include/ServerListener.h"
#include "include/tsu.h"
#include "include/aj_utility.h"

// NAMESPACES
using namespace std;
using namespace ajn;

// GLOBALS
bool done = false;  		// signal program to stop
bool scheduled = false;		// stops operator if not enabled at runtime

// Program Help
// - command line interface arguments during run, [] items have default values
static void ProgramHelp (const string& name) {
    cout << "\n[Usage] > " << name << " -c <file path> [-o <y/n>] -h\n"
    	"\t[] means it has a default value\n"
    	"\t -h \t help\n"
    	"\t -c \t configuration filename" 
		"\t -o \t enable operator"  << endl;
}  // end Program Help

// Argument Parser
// - method to parse program initial parameters
static std::map <std::string, std::string> ArgumentParser (int argc, 
														   char** argv) {
    string name = argv[0];

    // parse tokens
    map <string, string> parameters;
    string token, argument;

    for (int i = 1; i < argc; i = i+2){
        token = argv[i];

        // check to see if the is an argument for the program control token
        if (argc <= i+1) {
        	cout << "[ERROR] : Invalid program argument: " << token << endl;
            ProgramHelp(name);
            exit(EXIT_FAILURE); 
        } else {
        	argument = argv[i+1];
        }
        

        if ((token == "-h")) {
            ProgramHelp(name);
            exit(EXIT_FAILURE);
        } else if ((token == "-c")) {
            parameters["config"] = argument;
        } else if ((token == "-o")) {
            if ((argument == "y")) {
            	scheduled = true;
            } else {
	        	cout << "[ERROR] : Invalid program argument: " << token << endl;
	            ProgramHelp(name);
	            exit(EXIT_FAILURE); 
            }
        } else {
            cout << "[ERROR] : Invalid parameter: " << token << endl;
            ProgramHelp(name);
            exit(EXIT_FAILURE);
        }
    }
    return parameters;
}  // end Argument Parser

// THREADS
// -------

// Resource Loop
// - this loop runs the resource control loop at the desired frequency
void ResourceLoop (unsigned int sleep, DistributedEnergyResource* der_ptr) {
    unsigned int time_remaining, time_past;
    unsigned int time_wait = sleep;
    auto time_start = chrono::high_resolution_clock::now();
    auto time_end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> time_elapsed;

    while (!done) {
        time_start = chrono::high_resolution_clock::now();
            // time since last control call;
            time_elapsed = time_start - time_end;
            time_past = time_elapsed.count();
            der_ptr->Loop(time_past);
        time_end = chrono::high_resolution_clock::now();
        time_elapsed = time_end - time_start;

        // determine sleep duration after deducting process time
        time_remaining = (time_wait - time_elapsed.count());
        if (time_wait - time_elapsed.count() > 0) {
        	time_remaining = time_wait - time_elapsed.count();
        } else {
        	time_remaining = 0;
        }
        this_thread::sleep_for (chrono::milliseconds (time_remaining));
    }
}  // end Resource Loop

// Operator Loop
// - this loop runs the resource control loop at the desired frequency
void OperatorLoop (unsigned int sleep, Operator* oper_ptr) {
    unsigned int time_remaining;
    unsigned int time_wait = sleep;
    auto time_start = chrono::high_resolution_clock::now();
    auto time_end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> time_elapsed;

    while (!done && scheduled) {
        time_start = chrono::high_resolution_clock::now();
        oper_ptr->Loop();
        time_end = chrono::high_resolution_clock::now();
        time_elapsed = time_end - time_start;

        // determine sleep duration after deducting process time
        time_remaining = (time_wait - time_elapsed.count());
        if (time_wait - time_elapsed.count() > 0) {
        	time_remaining = time_wait - time_elapsed.count();
        } else {
        	time_remaining = 0;
        }
        this_thread::sleep_for (chrono::milliseconds (time_remaining));
    }
}  // end Resource Loop

// Smart Grid Device Loop
// - this loop runs the resource control loop at the desired frequency
void SmartGridDeviceLoop (unsigned int sleep, SmartGridDevice* sgd_ptr) {
    unsigned int time_remaining;
    unsigned int time_wait = sleep;
    auto time_start = chrono::high_resolution_clock::now();
    auto time_end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> time_elapsed;

    while (!done && scheduled) {
        time_start = chrono::high_resolution_clock::now();
        sgd_ptr->Loop();
        time_end = chrono::high_resolution_clock::now();
        time_elapsed = time_end - time_start;

        // determine sleep duration after deducting process time
        time_remaining = (time_wait - time_elapsed.count());
        if (time_wait - time_elapsed.count() > 0) {
        	time_remaining = time_wait - time_elapsed.count();
        } else {
        	time_remaining = 0;
        }
        this_thread::sleep_for (chrono::milliseconds (time_remaining));
    }
}  // end Resource Loop

// Main
// ----
int main (int argc, char** argv) {
    cout << "\n***\tDistributed Control System\t***\n";
    cout << "Initialization...\n";
   if (argc < 3) {
        string name = argv[0];
        ProgramHelp(name);
        return EXIT_FAILURE;
    }
    map <string, string> arguments = ArgumentParser(argc, argv);

    // read config file for program configurations and object attributes
    tsu::config_map configs = tsu::MapConfigFile (arguments["config"]);

    // create program objects
    cout << "\tCreating Distributed Energy Resource\n";
    DistributedEnergyResource* der_ptr 
    	= new DistributedEnergyResource(configs["DER"]);

    cout << "\tCreating Operator\n";
    Operator* oper_ptr = new Operator(configs["Operator"]["schedule"], der_ptr);

    cout << "\tCreating Command Line Interface\n";
    CommandLineInterface CLI(der_ptr);

    cout << "\tCreating AllJoyn Message Bus\n";
    try {
    	cout << "\t\tInitializing AllJoyn...\n";
        AllJoynInit();
    } catch (exception &e) {
        cout << "[ERROR]: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    #ifdef ROUTER
        try {
        	cout << "\t\tInitializing AllJoyn Router...\n";
            AllJoynRouterInit();
        } catch (exception &e) {
            cout << "[ERROR]: " << e.what() << endl;
            return EXIT_FAILURE;
        }
    #endif // ROUTER

    string app = configs["AllJoyn"]["app"];
    bool allow_remote = true;
    BusAttachment* bus_ptr = new BusAttachment(app.c_str(), allow_remote);


    cout << "\tCreating AllJoyn About Data\n";
    AboutData about_data("en");
    AboutObj* about_ptr = new AboutObj(*bus_ptr);

    cout << "\tCreating AllJoyn Session Port\n";
    aj_utility::SessionPortListener SPL;
    SessionPort port = stoul(configs["AllJoyn"]["port"]);

    cout << "\tSetting up AllJoyn Bus Attachment...\n";
    QStatus status = aj_utility::SetupBusAttachment (configs,
                                                     port,
                                                     SPL,
                                                     bus_ptr,
                                                     &about_data);

    cout << "\tCreating AllJoyn Observer\n";
    const char* server_name = configs["AllJoyn"]["server_interface"].c_str();
    Observer *obs_ptr = new Observer(*bus_ptr, &server_name, 1);

    cout << "\tCreating AllJoyn Server Listener\n";
    ServerListener *listner_ptr = new ServerListener(bus_ptr,
                                                     obs_ptr,
                                                     der_ptr,
                                                     server_name);
    obs_ptr->RegisterListener(*listner_ptr);

    cout << "\tCreating AllJoyn Smart Grid Device\n";
    const char* device_name = configs["AllJoyn"]["device_interface"].c_str();
    const char* path = configs["AllJoyn"]["path"].c_str();
    SmartGridDevice *sgd_ptr = new SmartGridDevice(der_ptr, 
                                                   bus_ptr, 
                                                   device_name, 
                                                   path);

    cout << "\t\tRegistering AllJoyn Smart Grid Device\n";
    if (ER_OK != bus_ptr->RegisterBusObject(*sgd_ptr)){
        cout << "\t\t[ERROR]: Failed Registration!\n";
        return EXIT_FAILURE;
    }
    about_ptr->Announce(port, about_data);

    // most objects will have a dedicated thread, but not all
    cout << "\tSpawning threads...\n";
    thread DER (ResourceLoop, stoul(configs["Threads"]["sleep"]), der_ptr);
    thread OPER (
    	OperatorLoop, stoul(configs["Threads"]["sleep"]), oper_ptr
    );
    thread SGD (
    	SmartGridDeviceLoop, stoul(configs["Threads"]["sleep"]), sgd_ptr
    );

    // the CLI will control the program and can signal the program to stop
	cout << "Initialization complete...\n";
    CLI.Help ();
    string input;
    while (!done) {
        getline(cin, input);
        done = CLI.Control (input);
    }

    // when done = true, the program begins the shutdown process
	cout << "Closing program...\n";

	// First join all active threads to main thread
	cout << "\tJoining threads\n";
	DER.join ();
	OPER.join ();
	SGD.join ();

    cout << "\tShutting down AllJoyn\n";
    obs_ptr->UnregisterAllListeners ();

    #ifdef ROUTER
        AllJoynRouterShutdown ();
    #endif // ROUTER

    AllJoynShutdown ();

	// Then delete all pointers that were created using "new" since they do not
	// automaticall deconstruct at the end of the program.
    cout << "\nDeleting pointers...\n";
    delete sgd_ptr;
    delete listner_ptr;
    delete obs_ptr;
    delete about_ptr;
    delete bus_ptr;
   	delete oper_ptr;
    delete der_ptr;

    // return exit status
    return 0;
} // end main
