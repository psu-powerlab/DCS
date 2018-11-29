#ifndef COMMANDLINEINTERFACE_H_INCLUDED
#define COMMANDLINEINTERFACE_H_INCLUDED

// INCLUDES
#include <string>
#include "DistributedEnergyResource.h"

class CommandLineInterface {
    public:
        // constructor / destructor
        CommandLineInterface (DistributedEnergyResource* der_ptr);
        virtual ~CommandLineInterface ();
        void Help ();
        bool Control (const std::string& input);

    private:
        DistributedEnergyResource* der_ptr_;

};  // end Command Line Interface



#endif // COMMANDLINEINTERFACE_H_INCLUDED
