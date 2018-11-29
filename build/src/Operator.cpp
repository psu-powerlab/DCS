// INCLUDES
#include <iostream>
#include <ctime>
#include "include/Operator.h"
#include "include/tsu.h"

Operator::Operator (const std::string& filename, 
                    DistributedEnergyResource* der_ptr) 
    : index_(0), der_ptr_(der_ptr) {
    tsu::string_matrix matrix = tsu::FileToMatrix(filename, ',', 3);

    // reserve memory for each row within the matrix
    schedule_.reserve(matrix.size());

    // loop through each row of the matrix and set each column to it's
    // header data type
    for (const auto &col : matrix) {
        schedule_.emplace_back (stoi(col[0]), col[1], stoul(col[2]));
    }
}  // end constructor

Operator::~Operator () {

}  // end destructor

void Operator::Loop () {
    // get the current time utc and take modulo to neglect days
    unsigned int seconds_per_day = 60*60*24;
    unsigned int utc = time(NULL) % seconds_per_day;

    // loop through each row of schedule looking for current time
    for (unsigned int i = index_; i < schedule_.size(); i++) {
        ScheduleHeader& row = schedule_[i];

        if (row.time % seconds_per_day == utc && index_ != i) {

            // if the time is found check control type
            if (row.control == "import") {
                der_ptr_->SetImportWatts(row.setting);
            } else if (row.control == "export") {
                der_ptr_->SetExportWatts(row.setting);
            } else {
                der_ptr_->SetImportWatts(0);
            }

            // log index so multiple controls are not sent and to reduce
            // search time.
            index_ = i;
        }
    }
}  // end Loop