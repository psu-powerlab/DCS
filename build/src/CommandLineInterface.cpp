// INCLUDES
#include <iostream>
#include <sstream>
#include <vector>
#include "include/CommandLineInterface.h"

bool scheduled;  // this variable is a global from main

CommandLineInterface::CommandLineInterface (
	DistributedEnergyResource* der_ptr) : der_ptr_(der_ptr) {
}  // end constructor

CommandLineInterface::~CommandLineInterface () {
    // do nothing at this time
}  // end destructor

// Help
// - CLI interface description
void CommandLineInterface::Help () {
    std::cout << "\n\t[Help Menu]\n\n"
        << "> q            quit\n"
        << "> h            display help menu\n"
        << "> o <y/n>      operator enable/disable\n"
        << "> i <watts>    import power\n"
        << "> e <watts>    export power\n"
        << "> d            display properties\n";
} // end Help

// Command Line Interface
// - method to allow user controls during program run-time
bool CommandLineInterface::Control (const std::string& input) {
    // check for program argument
    if (input == "") {
        CommandLineInterface::Help ();
        return false;
    }
    char cmd = input[0];

    // deliminate input string to argument parameters
    std::vector <std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }

    switch (cmd) {
        case 'q':
           return true;

        case 'i': {
            try {
                der_ptr_->SetImportWatts(stoul(tokens.at(1)));
            } catch (...) {
                std::cout << "[ERROR]: Invalid Argument.\n";
                break;
            }
            
            break;
        }

        case 'o': {
            try {
                if (tokens.at(1) == "n") {
                    scheduled = false;
                } else if (tokens.at(1) == "y") {
                    scheduled = true;
                } else {
                    throw;
                }
            } catch(...) {
                std::cout << "[ERROR]: Invalid Argument.\n";
            }
            break;
        }

        case 'e': {
            try {
                der_ptr_->SetExportWatts(stoul(tokens.at(1)));
            } catch(...) {
                std::cout << "[ERROR]: Invalid Argument.\n";
            }
            break;
        }

        case 'd': {
            der_ptr_->Display ();
            break;
        }

        default: {
            CommandLineInterface::Help ();
            break;
        }
    }

    return false;
}  // end Command Line Interface