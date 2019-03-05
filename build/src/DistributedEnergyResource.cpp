// INCLUDES
#include <iostream>
#include <random>
#include <ctime>
#include "include/DistributedEnergyResource.h"
#include "include/logger.h"

// This constructor is to be used by child classes since they will populate
// properties through device queries
DistributedEnergyResource::DistributedEnergyResource () : 
    rated_export_power_(0),
    rated_export_energy_(0),
    export_ramp_(0),
    rated_import_power_(0),
    rated_import_energy_(0),
    import_ramp_(0),
    idle_losses_(0),
    export_power_(0),
    import_power_(0),
    export_watts_(0),
    import_watts_(0),
    delta_time_(0),
    last_utc_(0) {
    // do nothing
}  // end constructor

// this constructor is used to simulate a genaric der
DistributedEnergyResource::DistributedEnergyResource (
    std::map <std::string, std::string> init) : 
    rated_export_power_(stoul(init["rated_export_power"])),
    rated_export_energy_(stoul(init["rated_export_energy"])),
    export_ramp_(stoul(init["rated_export_ramp"])),
    rated_import_power_(stoul(init["rated_import_power"])),
    rated_import_energy_(stoul(init["rated_import_energy"])),
    import_ramp_(stoul(init["rated_import_ramp"])),
    idle_losses_(stoul(init["idle_losses"])),
    export_power_(0),
    import_power_(0),
    export_watts_(0),
    import_watts_(0),
    delta_time_(0),
    last_utc_(0),
    log_inc_(stoul(init["log_inc"])),
    log_path_(init["log_path"]) {

    // randomly assign energy capacity based on normal distripution
    // - reference: 
    // https://en.cppreference.com/w/cpp/numeric/random/normal_distribution
    std::random_device seed {};     // uniform distribution seed number
    std::mt19937 gen {seed ()};     // high quality seed
    float mean = stof(init["normal_mean"]);
    float std_dev = stof(init["standard_deviation"]);
    std::normal_distribution<double> distribution(mean, std_dev);

    float percent = 2;
    while (percent > 1 || percent < 0) {
        percent = distribution (gen);
    }

    // use random percentage to set import and export where export is (1-p)
    import_energy_ = rated_import_energy_ * percent;
    export_energy_ = rated_export_energy_ * (1 - percent);
}  // end constructor

DistributedEnergyResource::~DistributedEnergyResource () {
    //dtor
}

// Set Export Watts
// - export watts is used as a control setpoint by ExportPowe and turns import
// - power off.
void DistributedEnergyResource::SetExportWatts (unsigned int power) {
    import_watts_ = 0;
    import_power_ = 0;
    if (power > rated_export_power_) {
        export_watts_ = rated_export_power_;
    } else {
        export_watts_ = power;
    }
}  // end Set Export Watts

// Set Export Power
// - regulates export power
void DistributedEnergyResource::SetExportPower (float power) {
    if (power > rated_export_power_) {
        export_power_ = rated_export_power_;
    } else if (power <= 0) {
        export_power_ = 0;
    } else {
        export_power_ = power;
    }
}  // end Set Export Power

// Set Export Energy
// - regulates export energy
void DistributedEnergyResource::SetExportEnergy (float energy) {
    if (energy > rated_export_energy_) {
        export_energy_ = rated_export_energy_;
    } else if (energy <= 0) {
        export_energy_ = 0;
    } else {
        export_energy_ = energy;
    }
}  // end Set Export Energy

// Set Rated Export Power
// - set the watt value available to export to the grid
void DistributedEnergyResource::SetRatedExportPower (unsigned int power) {
    rated_export_power_ = power;
}  // end Rated Export Power

// Set Rated Export Energy
// - set the watt-hour value available to export to the grid
void DistributedEnergyResource::SetRatedExportEnergy (unsigned int energy) {
    rated_export_energy_ = energy;
}  // end Set Export Energy

// Set Export Ramp
// - set the watt per second value available to export to the grid
void DistributedEnergyResource::SetExportRamp (unsigned int ramp) {
    export_ramp_ = ramp;
}  // end Set Export Ramp

// Set Import Watts
// - turn off export power and set control setting for ImportPower method
void DistributedEnergyResource::SetImportWatts (unsigned int power) {
    export_watts_ = 0;
    export_power_ = 0;
    if (power > rated_import_power_) {
        import_watts_ = rated_import_power_;
    } else {
        import_watts_ = power;
    }
}  // end Set Import Watts

// Set Import Power
// - regulates import power
void DistributedEnergyResource::SetImportPower (float power) {
    if (power > rated_import_power_) {
        import_power_ = rated_import_power_;
    } else if (power <= 0) {
        import_power_ = 0;
    } else {
        import_power_ = power;
    }
}  // end Set Import Power

// Set Import Energy
// - regulates import energy balance export energy
void DistributedEnergyResource::SetImportEnergy (float energy) {
    if (energy > rated_import_energy_) {
        import_energy_ = rated_import_energy_;
    } else if (energy <= 0) {
        import_energy_ = 0;
        import_watts_ = 0;  // stop importing
    } else {
        import_energy_ = energy;
    }
}  // end Set Import Energy

// Set Rated Import Power
// - set the watt value available to import from the grid
void DistributedEnergyResource::SetRatedImportPower (unsigned int power) {
    rated_import_power_ = power;
}  // end Set Rated Import Power

// Set Rated Import Energy
// - set the watt-hour value available to import from the grid
void DistributedEnergyResource::SetRatedImportEnergy (unsigned int energy) {
    rated_import_energy_ = energy;
}  // end Set Import Energy

// Set Import Ramp
// - set the watt per second value available to import from the grid
void DistributedEnergyResource::SetImportRamp (unsigned int ramp) {
    import_ramp_ = ramp;
}  // end Set Import Ramp

// Set Idle Losses
// - set the watt-hours per hour loss when idle
void DistributedEnergyResource::SetIdleLosses (unsigned int losses) {
    idle_losses_ = losses;
}  // end Set Idle Losses

// Set Remote Time
// - the remote time can be updated to sycronize the der to some other clock
void DistributedEnergyResource::SetRemoteTime (unsigned int utc) {
    remote_utc_ = utc;
}  // end Set Remote Time

// Set Price
// - set the dollars per kilo-watt price for electricity
void DistributedEnergyResource::SetPrice (float price) {
    price_ = price;
}  // end Set Idle Losses

// Set Log Path
// - set the directory path for log files. It must be a valid path
void DistributedEnergyResource::SetLogPath (std::string path) {
    log_path_ = path;
}  // end Set Log Path

// Set Log Increment
// - log increment is seconds between logging
void DistributedEnergyResource::SetLogIncrement (unsigned int inc) {
    log_inc_ = inc;
}  // end Set Log Increment

// Get Rated Export Watts
// - get the control watt value available to export to the grid
unsigned int DistributedEnergyResource::GetExportWatts () {
    return export_watts_;
}  // end Get Rated Export Watts

// Get Rated Export Power
// - get the rated watt value available to export to the grid
unsigned int DistributedEnergyResource::GetRatedExportPower () {
    return rated_export_power_;
}  // end Get Rated Export Power

// Get Export Power
// - get the watt value available to export to the grid
unsigned int DistributedEnergyResource::GetExportPower () {
    unsigned int power = export_power_;
    return power;
}  // end Get Export Power

// Get Rated Export Energy
// - get the watt value available to import from the grid
unsigned int DistributedEnergyResource::GetRatedExportEnergy () {
    return rated_export_energy_;
}  // end Rated Export energy

// Get Export Energy
// - get the watt-hour value available to export to the grid
unsigned int DistributedEnergyResource::GetExportEnergy () {
    unsigned int energy = export_energy_;
    return energy;
}  // end Get Export Energy

// Get Export Ramp
// - get the watt per second value available to export to the grid
unsigned int DistributedEnergyResource::GetExportRamp () {
    return export_ramp_;
}  // end Get Export Ramp

// Get Rated Import Watts
// - get the control watt value available to export to the grid
unsigned int DistributedEnergyResource::GetImportWatts () {
    return import_watts_;
}  // end Get Rated Import Watts

// Get Rated Import Power
// - get the rated watt value available to import from the grid
unsigned int DistributedEnergyResource::GetRatedImportPower () {
    return rated_import_power_;
}  // end Rated Import Power

// Get Import Power
// - get the watt value available to import from the grid
unsigned int DistributedEnergyResource::GetImportPower () {
    unsigned int power = import_power_;
    return power;
}  // end Get Import Power

// Get Rated Import Energy
// - get the watt value available to import from the grid
unsigned int DistributedEnergyResource::GetRatedImportEnergy () {
    return rated_import_energy_;
}  // end Rated Import energy

// Get Import Energy
// - get the watt-hour value available to import from the grid
unsigned int DistributedEnergyResource::GetImportEnergy () {
    unsigned int energy = import_energy_;
    return energy;
}  // end Get Import Energy

// Get Import Ramp
// - get the watt per second value available to import from the grid
unsigned int DistributedEnergyResource::GetImportRamp () {
    return import_ramp_;
}  // end Get Import Ramp

// Get Idle Losses
// - get the watt-hours per hour loss when idle
unsigned int DistributedEnergyResource::GetIdleLosses () {
    return idle_losses_;
}  // end Get Idle Losses

// Get Log Path
// - return the path for logging file
std::string DistributedEnergyResource::GetLogPath () {
    return log_path_;
}  // end Get Log Path

// Get Remote Time
// - not sure if this will be required by others, but it is an accessor
unsigned int DistributedEnergyResource::GetRemoteTime () {
    return remote_utc_;
}  // end Get Remote TIme

// Get Price
// - I don't thing other objects will need this either
float DistributedEnergyResource::GetPrice () {
    return price_;
}  // end Get Price

// Import Power
// - calculate power/energy change 
// - degrement import energy and increment export energy
void DistributedEnergyResource::ImportPower () {
    // regulate import power
    float seconds = delta_time_ / 1000;
    float ramp_watts = import_ramp_ * seconds;
    float power;
    if (import_power_ == import_watts_){
        // do nothing
    } else if (import_power_ < import_watts_) {
        power = import_power_ + ramp_watts;
        if (power > import_watts_) {
            power = import_watts_;
        }
        DistributedEnergyResource::SetImportPower (power);
    } else if (import_power_ > import_watts_) {
        power = import_power_ - ramp_watts;
        if (power < import_watts_) {
            power = import_watts_;
        }
        DistributedEnergyResource::SetImportPower (power);
    }

    // regulate energy
    float hours = seconds / (60*60);
    float watt_hours = 0;

    // if the import power didn't reach rated then include ramp in energy calc
    // this function does not account for the cycle where it reaches rated,
    // but the error is negligable comparted to rated energy
    if (import_power_ < rated_import_power_) {
        // area under the linear function
        watt_hours += import_power_ * hours;  // area under triangle
        watt_hours += ramp_watts * hours/2;   // area of triangle
        DistributedEnergyResource::SetImportEnergy(import_energy_ - watt_hours);
        DistributedEnergyResource::SetExportEnergy(export_energy_ + watt_hours);
    } else {
        // area under the linear function excluding ramp.
        // note: this will exclude some energy if it reached peak within bounds
        watt_hours += import_power_ * hours;
        DistributedEnergyResource::SetImportEnergy(import_energy_ - watt_hours);
        DistributedEnergyResource::SetExportEnergy(export_energy_ + watt_hours);
    }
}  // end Import Power

// Export Power
// - calculate power/energy change 
// - degrement export energy and increment import energy
void DistributedEnergyResource::ExportPower () {
    float seconds = delta_time_ / 1000;
    float ramp_watts = export_ramp_ * seconds;
    float power;
    if (export_power_ == export_watts_){
        // do nothing
    } else if (export_power_ < export_watts_) {
        power = export_power_ + ramp_watts;
        if (power > export_watts_) {
            power = export_watts_;
        }
        DistributedEnergyResource::SetExportPower (power);
    } else if (export_power_ > export_watts_) {
        power = export_power_ - ramp_watts;
        if (power < export_watts_) {
            power = export_watts_;
        }
        DistributedEnergyResource::SetExportPower (power);
    }

    // regulate energy
    float hours = seconds / (60*60);
    float watt_hours = 0;

    // if the import power didn't reach rated then include ramp in energy calc
    // this function does not account for the cycle where it reaches rated,
    // but the error is negligable comparted to rated energy
    if (export_power_ < rated_export_power_) {
        // area under the linear function
        watt_hours += export_power_ * hours;  // area under triangle
        watt_hours += ramp_watts * hours/2;   // area of triangle
        DistributedEnergyResource::SetImportEnergy(import_energy_ + watt_hours);
        DistributedEnergyResource::SetExportEnergy(export_energy_ - watt_hours);
    } else {
        // area under the linear function excluding ramp.
        // note: this will exclude some energy if it reached peak within bounds
        watt_hours += export_power_ * hours;
        DistributedEnergyResource::SetImportEnergy(import_energy_ + watt_hours);
        DistributedEnergyResource::SetExportEnergy(export_energy_ - watt_hours);
    }
}  // end Export Power

// Idle Loss
// - update energy available based on energy lost
void DistributedEnergyResource::IdleLoss () {
    float seconds = delta_time_ / 1000;
    float hours = seconds / (60*60);
    float energy_loss = idle_losses_ * hours;
    DistributedEnergyResource::SetImportEnergy(import_energy_ + energy_loss);
    DistributedEnergyResource::SetExportEnergy(export_energy_ - energy_loss);
}  // end Idle Loss

// Log
// - log important physical attributes of DER on a frequency set by config file
void DistributedEnergyResource::Log () {
    unsigned int utc = time (0);
    if (utc % log_inc_ == 0 && last_utc_ != utc){
        Logger ("DER_Data", log_path_)
            << import_watts_ << "\t"
            << import_power_ << "\t"
            << import_energy_ << "\t" 
            << export_watts_ << "\t"
            << export_power_ << "\t"
            << export_energy_ << "\t";
        last_utc_ = utc;
    }
}  // end Log

// Loop
// - for non simulated devices the delta_time value can be ignored.
void DistributedEnergyResource::Loop (float delta_time) {
    delta_time_ = delta_time;

    if (import_watts_ > 0) {
        DistributedEnergyResource::ImportPower ();
    } else if (export_watts_ > 0) {
        DistributedEnergyResource::ExportPower ();
    } else {
        IdleLoss ();
    }
    DistributedEnergyResource::Log ();
}  // end Control

// Display
// - print device properties to terminal
void DistributedEnergyResource::Display () {
    std::cout 
        << "Import Power:\t" << import_power_ << "\twatts\n"
        << "Import Control:\t" << import_watts_ << "\twatts\n"
        << "Import Energy:\t" << import_energy_ << "\twatt-hours\n"
        << "Export Power:\t" << export_power_ << "\twatts\n"
        << "Export Control:\t" << export_watts_ << "\twatts\n"
        << "Export Energy:\t" << export_energy_ << "\twatt-hours\n" 
        << "Power Price:\t" << price_ << "\tcents/watt-hours\n" << std::endl;
}  // end Display