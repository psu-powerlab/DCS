// INCLUDES
#include <iostream>
#include <random>
#include <ctime>
#include "include/DistributedEnergyResource.h"
#include "include/logger.h"


DistributedEnergyResource::DistributedEnergyResource (
    std::map <std::string, std::string> init) : 
    rated_export_power_(stoul(init["rated_export_power"])),
    rated_export_energy_(stoul(init["rated_export_energy"])),
    export_ramp_(stoul(init["rated_export_ramp"])),
    rated_import_power_(stoul(init["rated_import_power"])),
    rated_import_energy_(stoul(init["rated_import_energy"])),
    import_ramp_(stoul(init["rated_import_power"])),
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
// - set the export control property used in the control loop
void DistributedEnergyResource::SetExportWatts (unsigned int power) {
    import_watts_ = 0;
    import_power_ = 0;
    if (power > rated_export_power_) {
        power = rated_export_power_;
    }
    export_watts_ = power;
}  // end Set Export Watts

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
// - set the import control property used in the control loop
void DistributedEnergyResource::SetImportWatts (unsigned int power) {
    export_watts_ = 0;
    export_power_ = 0;
    if (power > rated_import_power_) {
        power = rated_import_power_;
    }
    import_watts_ = power;
}  // end Set Import Watts

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

// Get Rated Export Power
// - get the watt value available to export to the grid
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

// Get Rated Import Power
// - get the watt value available to import from the grid
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

// Import Power
// - called by control loop if import power is set
// - assume loss is factored into import power
void DistributedEnergyResource::ImportPower () {
    float seconds = delta_time_ / 1000;
    float watts = import_ramp_ * seconds;

    // regulate import power
    if (import_power_ + watts < import_watts_) {
        import_power_ += watts;
    } else {
        import_power_ = import_watts_;
    }

    // regulate energy
    float hours = seconds / (60*60);
    if (import_energy_ - import_power_ > 0) {
        // area under the linear function
        import_energy_ -= (import_power_*hours + watts*hours/2);
        export_energy_ = rated_export_energy_ - import_energy_;
    } else {
        import_power_ = 0;
        import_energy_ = 0;
        export_energy_ = rated_export_energy_;
    }
}  // end Import Power

// Export Power
// - called by control loop if export power is set
// - assume loss is factored into export power
void DistributedEnergyResource::ExportPower () {
    float seconds = delta_time_ / 1000;
    float watts = export_ramp_ * seconds;

    // regulate import power
    if (export_power_ + watts < export_watts_) {
        export_power_ += watts;
    } else {
        export_power_ = export_watts_;
    }

    // regulate energy
    float hours = seconds / (60*60);
    if (export_energy_ - export_power_ > 0) {
        // area under the linear function
        export_energy_ -= (export_power_*hours + watts*hours/2);
        import_energy_ = rated_import_energy_ - export_energy_;
    } else {
        export_power_ = 0;
        export_energy_ = 0;
        import_energy_ = rated_import_energy_;
    }
}  // end Export Power

// Idle Loss
// - update energy available based on energy lost
void DistributedEnergyResource::IdleLoss () {
    float seconds = delta_time_ / 1000;
    float hours = seconds / (60*60);
    float energy_loss = idle_losses_ * hours;

    if (import_energy_ + energy_loss < rated_import_energy_) {
        import_energy_ += energy_loss;
    }

    if (export_energy_ - energy_loss > 0) {
        export_energy_ -= energy_loss;
    }
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
    std::cout << "Import Control:\t" << import_power_ << "\twatts\n";
    std::cout << "Import Power:\t" << import_watts_ << "\twatts\n";
    std::cout << "Import Energy:\t" << import_energy_ << "\twatt-hours\n";
    std::cout << "Export Control:\t" << export_power_ << "\twatts\n";
    std::cout << "Export Power:\t" << export_watts_ << "\twatts\n";
    std::cout << "Export Energy:\t" << export_energy_ << "\twatt-hours\n";
}  // end Display