
// INCLUDES
#include <iostream>
#include <ctime>
#include <cmath>
#include "include/SmartGridDevice.h"

// Constructor
// - initialize bus object interface and smart grid device properties
SmartGridDevice::SmartGridDevice (DistributedEnergyResource* der_ptr,
                                  ajn::BusAttachment* bus_ptr,
                                  const char* name,
                                  const char* path) : ajn::BusObject(path),
                                                      der_ptr_(der_ptr),
                                                      bus_ptr_(bus_ptr),
                                                      signal_(NULL),
                                                      interface_(name) {
    const ajn::InterfaceDescription* interface = bus_ptr_->GetInterface(interface_);
    assert(interface != NULL);
    AddInterface(*interface, ANNOUNCED);

    const ajn::BusObject::MethodEntry methods[] = {{
            interface->GetMember("ImportPower"), 
            static_cast <ajn::BusObject::MessageReceiver::MethodHandler>
            (&SmartGridDevice::ImportPowerHandler)
        }, {
            interface->GetMember("ExportPower"), 
            static_cast <ajn::BusObject::MessageReceiver::MethodHandler>
            (&SmartGridDevice::ExportPowerHandler)
        },

    };

    size_t count = sizeof (methods) / sizeof (methods[0]);
    QStatus status = AddMethodHandlers (methods, count);
    if (ER_OK != status) {
        throw status;
    }

    last_import_energy_ = der_ptr_->GetImportEnergy ();
    last_export_energy_ = der_ptr_->GetExportEnergy ();
}

// Import Power Handler
// - called by remote consumer and sends the watt value for import
void SmartGridDevice::ImportPowerHandler (
        const ajn::InterfaceDescription::Member* member,
        ajn::Message& message) {
    (void)member;
    unsigned int import_watts = message->GetArg(0)->v_uint32;
    std::cout << "[ALLJOYN]: Import...\t" << import_watts << std::endl;
    der_ptr_->SetImportWatts (import_watts);
}  // end Import Power Handler

// Export Power Handler
// - called by remote consumer and sends the watt value for export
void SmartGridDevice::ExportPowerHandler (
        const ajn::InterfaceDescription::Member* member,
        ajn::Message& message) {
    (void)member;
    unsigned int export_watts = message->GetArg(0)->v_uint32;
    std::cout << "[ALLJOYN]: Export...\t" << export_watts << std::endl;
    der_ptr_->SetExportWatts (export_watts);
}  // end Export Power Handler

// Get
// - this method will be called by remote devices looking to get this devices
// - properties
QStatus SmartGridDevice::Get (const char* interface,
                              const char* property,
                              ajn::MsgArg& value) {
    QStatus status;
    if (strcmp(interface, interface_)) {
        return ER_FAIL;
    }

    last_telemetry_utc_ = time (0);

    if (!strcmp(property,"rated_import_power")) {
        status = value.Set("u", der_ptr_->GetRatedImportPower ());
        return status;
    } else if (!strcmp(property,"rated_import_energy")) {
        status = value.Set("u", der_ptr_->GetRatedImportEnergy ());
        return status;
    } else if (!strcmp(property,"import_power")) {
        status = value.Set("u", der_ptr_->GetImportPower ());
        return status;
    } else if (!strcmp(property,"import_energy")) {
        last_import_energy_ = der_ptr_->GetImportEnergy ();
        status = value.Set("u", last_import_energy_);
        return status;
    } else if (!strcmp(property,"import_ramp")) {
        status = value.Set("u", der_ptr_->GetImportRamp ());
        return status;
    } else if (!strcmp(property,"rated_export_power")) {
        status = value.Set("u", der_ptr_->GetRatedExportPower ());
        return status;
    } else if (!strcmp(property,"rated_export_energy")) {
        status = value.Set("u", der_ptr_->GetRatedExportEnergy ());
        return status;
    } else if (!strcmp(property,"export_power")) {
        status = value.Set("u", der_ptr_->GetExportPower ());
        return status;
    } else if (!strcmp(property,"export_energy")) {
        last_export_energy_ = der_ptr_->GetExportEnergy ();
        status = value.Set("u", last_export_energy_);
        return status;
    } else if (!strcmp(property,"export_ramp")) {
        status = value.Set("u", der_ptr_->GetExportRamp ());
        return status;
    } else if (!strcmp(property,"idle_losses")) {
        status = value.Set("u", der_ptr_->GetIdleLosses ());
        return status;
    } else {
        return ER_FAIL;
    }
} // end Get

QStatus SmartGridDevice::SendPropertiesUpdate () {
const char* props[] = { "rated_export_power",
                        "export_power",
                        "rated_export_energy",
                        "export_energy",
                        "export_ramp",
                        "rated_import_power",
                        "import_power",
                        "rated_import_energy",
                        "import_energy",
                        "import_ramp",
                        "idle_losses"};
    QStatus status;
    status = EmitPropChanged (interface_, props, 11, ajn::SESSION_ID_ALL_HOSTED);
    return status;
}  // end Send Properties Update

// Loop
// - this loop will run in its own thread and check for high (dE/dt).
// - it will also send the regularly scheduled telemetry
void SmartGridDevice::Loop () {
    unsigned int utc = time (0);
    bool new_update = (last_telemetry_utc_ != utc);
    bool five_minutes = (utc % (5*60) == 0);
    bool one_hour = (utc % (60*60) == 0);

    float import_energy = der_ptr_->GetImportEnergy ();
    float export_energy = der_ptr_->GetExportEnergy ();
    float delta_import = std::abs((import_energy / last_import_energy_) - 1);
    float delta_export = std::abs((export_energy / last_export_energy_) - 1);
    // import/export energy setting. Trigger at 10% (arbitrary)
    bool energy_deviation = (delta_export > 0.1 || delta_import > 0.1);

    // every hour send telemetry update
    if (one_hour && new_update) {
        QStatus status = SmartGridDevice::SendPropertiesUpdate ();
        std::cout << "Sending telemetry update:\t" << status << std::endl;
        last_telemetry_utc_ = utc;
    // every 5 minutes check for deviation in import/export energy
    } else if (five_minutes && new_update && energy_deviation) {
        QStatus status = SmartGridDevice::SendPropertiesUpdate ();
        std::cout << "Energy deviation... sending property update:\t" 
            << status << std::endl;
        last_telemetry_utc_ = utc;
    }
}