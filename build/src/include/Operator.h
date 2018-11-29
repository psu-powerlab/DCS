#ifndef OPERATOR_H_INCLUDED
#define OPERATOR_H_INCLUDED

// INCLUDES
#include <string>
#include <vector>
#include "DistributedEnergyResource.h"

class Operator {
public:
    Operator (const std::string& filename, DistributedEnergyResource* der_ptr);
    virtual ~Operator ();
    void Loop ();

private:
    // since the file columns are known we can create and object to represent
    // the values within the column.
    struct ScheduleHeader {
        unsigned int time;
        std::string control;
        unsigned int setting;

        ScheduleHeader (int time, std::string control, unsigned int setting)
            : time(time), control(control), setting(setting) {
        };
    };

private:
    unsigned int index_;
    DistributedEnergyResource* der_ptr_;
    std::vector <ScheduleHeader> schedule_;
};  // end Operator

#endif // OPERATOR_H_INCLUDED