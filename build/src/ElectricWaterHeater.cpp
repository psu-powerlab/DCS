#include <iostream>
#include <sstream>
#include <chrono>
#include "include/ElectricWaterHeater.h"
#include "include/logger.h"

// MACROS
#define DEBUG(x) std::cout << x << std::endl

ElectricWaterHeater::ElectricWaterHeater (
	std::map <std::string, std::string> configs) : 
		DistributedEnergyResource (),
		sp_(configs["serial_port"]),
	  	current_transducer_1_(stoul(configs["mcp_channel"])),
	  	heartbeat_(stoul(configs["ucm_heartbeat"])) {

	// verivy serial port is available and connected to UCM
	if (!sp_.open ()) {
		Logger("ERROR", GetLogPath ()) 
			<< "failed to open serial port: " << strerror(errno);
		exit (1);
	} else {
		device_ptr_ = cea2045::DeviceFactory::createUCM(&sp_, &ucm_);
		device_ptr_->start ();
		device_ptr_->basicOutsideCommConnectionStatus(
			cea2045::OutsideCommuncatonStatusCode::Found
		);
	}
	response_codes_ = device_ptr_->querySuportDataLinkMessages().get();
	response_codes_ = device_ptr_->queryMaxPayload().get();
	response_codes_ = device_ptr_->querySuportIntermediateMessages().get();
	response_codes_ = device_ptr_->intermediateGetDeviceInformation().get();

	SetLogPath (configs["log_path"]);
	SetLogIncrement (stoul(configs["log_inc"]));
	Logger("INFO", GetLogPath ()) << "startup complete";
}  // end Constructor

ElectricWaterHeater::~ElectricWaterHeater () {
	delete device_ptr_;
}  // end Destructor

// Begin critical peak event status
void ElectricWaterHeater::SetCriticalPeak () {
	device_ptr_->basicCriticalPeakEvent (0);
	opstate_ = 4;
	Logger("INFO", GetLogPath ()) << "Critical peak event command received";
}  /// end critical peak event status change

// Begin load up status
void ElectricWaterHeater::SetLoadUp () {
	opstate_ = 3;
	device_ptr_->basicLoadUp (0);
	Logger("INFO", GetLogPath ()) << "Load up command received";
}  /// end load up command

// Begin Grid Emergency status
void ElectricWaterHeater::SetGridEmergency () {
	opstate_ = 5;
	device_ptr_->basicGridEmergency (0);
	Logger("INFO", GetLogPath ()) << "Grid Emergency command received";
}  /// end grid emergency command

// Get Real Import Power
// - use current transducer and approximated voltage to get real power draw from
// - water heater
unsigned int ElectricWaterHeater::GetRealImportPower () {
	return current_transducer_1_.GetCurrent() * 240; // assuming Vrms = 240
}  // end Get Real Import Power

// Query Properties
// - update basic DER properties using EWH methods
void ElectricWaterHeater::QueryProperties () {
	device_ptr_->intermediateGetCommodity ();
	std::vector <CommodityData> commodities = ucm_.GetCommodityData ();
	for (const auto &commodity : commodities) {
		if (commodity.code == 0) {
			SetImportPower (commodity.rate);
		} else if (commodity.code == 6) {
			SetRatedImportPower (commodity.cumulative);
		} else if (commodity.code == 7) {
			SetImportPower (commodity.cumulative);
		}
	}
}  // end Query Properties

/////////////////
// DER Overwrites
/////////////////

// Import Power
// - send a EndShed command through the UCM with a zero duration for undefined
// - amount of time
void ElectricWaterHeater::ImportPower () {
	device_ptr_->basicEndShed (0);  // zero duration means as along as possible
}  // end Import Power

// Export Power
// - DO NOTHING since water heaters cannot export power
void ElectricWaterHeater::ExportPower () {
	// just overwrite so DER Loop () doesn't do anything to properties
}  // end Export Power

// Idle Loss
// - probably not the best name for this method, but when idle we want the
// - water heater to no be on so we send a Shed () command through the UCM with
// - a duration of 0 for undefined amount of time
void ElectricWaterHeater::IdleLoss () {
	device_ptr_->basicShed (0);
}