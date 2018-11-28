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
#include "include/DistributedEnergyResource.h"
#include "include/tsu.h"

// NAMESPACES
using namespace std;

// GLOBALS
bool done = false;  // signal program to stop

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

// Main
// ----
int main (int argc, char** argv) {
    cout << "Starting program...\n";

    // read config file for program configurations and object attributes
    tsu::config_map configs = tsu::MapConfigFile ("../data/config.ini");

    // create program objects
    cout << "\tCreating Distributed Energy Resource\n";
    DistributedEnergyResource* der_ptr 
    	= new DistributedEnergyResource(configs["DER"]);


    // most objects will have a dedicated thread, but not all
    cout << "\tSpawning threads...\n";
    thread DER (ResourceLoop, stoul(configs["Threads"]["der_sleep"]), der_ptr);

    // the CLI will control the program and can signal the program to stop
    string input;
    while (!done) {
        getline(cin, input);
        done = true;
    }

    // when done = true, the program begins the shutdown process
	cout << "Closing program...\n";

	// First join all active threads to main thread
	cout << "\nJoining threads...\n";
	DER.join ();

	// Then delete all pointers that were created using "new" since they do not
	// automaticall deconstruct at the end of the program.
    cout << "\nDeleting pointers...\n";
    delete der_ptr;

    // return exit status
    return 0;
} // end main
