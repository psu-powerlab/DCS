#ifndef ELECTRICWATERHEATER_H_
#define ELECTRICWATERHEATER_H_

// INCLUDES
#include <map>
#include <string>
#include <cea2045/device/DeviceFactory.h>
#include <cea2045/communicationport/CEA2045SerialPort.h>
#include "UniversalControlModule.h"
#include "DistributedEnergyResource.h" 
#include "mcp3008.h"

class ElectricWaterHeater : public DistributedEnergyResource {
public:
	// Constructor/Destructor
	ElectricWaterHeater (std::map <std::string, std::string> configs);
	virtual ~ElectricWaterHeater ();

	// Set Methods
	void SetCriticalPeak ();
	void SetLoadUp ();
	void SetGridEmergency ();

private:
	// structures
	enum OpState {
		IDLE, NORMAL, CURTAILED, HEIGHTENED, GRID, SGD_ERROR,
	};

private:
	void QueryProperties ();
	void UpdateCommodityData ();
	unsigned int GetRealImportPower ();

	// DER Overwrites
	void ImportPower ();
	void ExportPower ();
	void IdleLoss ();

private:
	// Composition
	// - These are objects that a smart electric water heater will "have" 
	UniversalControlModule ucm_;
	cea2045::ResponseCodes response_codes_;
	cea2045::CEA2045SerialPort sp_;
	cea2045::ICEA2045DeviceUCM *device_ptr_;
	mcp3008 current_transducer_1_;

private:
	unsigned int heartbeat_;	// minute increment to update ucm
	int opstate_;


};

#endif  // end ELECTRICWATERHEATER_H_