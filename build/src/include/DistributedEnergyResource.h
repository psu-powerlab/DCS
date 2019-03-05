// Author: Tylor Slay
// Description: This class is used to describe all of the primary attributes
//      and control methods for a basic distributed energy resource. It will be
//      inherited by sub-classes of der such as BESS, EWH, and PV. By defining 
//      this class all the other class that will used the BESS, EWH, and PV will
//      only need to take a DER as an argument. The DER class also serves as a
//      simple simulator for der interactions using AllJoyn.

#ifndef DISTRIBUTEDENERGYRESOURCE_H_INCLUDED
#define DISTRIBUTEDENERGYRESOURCE_H_INCLUDED

#include <string>
#include <map>

class DistributedEnergyResource {
    public:
        // constructor / destructor
        DistributedEnergyResource ();  // defaults to zero value properties
        DistributedEnergyResource (std::map <std::string, std::string> init);
        virtual ~DistributedEnergyResource ();
        virtual void Loop (float delta_time);
        virtual void Display ();

    public:
        // accessor methods
        // export
        void SetExportWatts (unsigned int power);
        void SetExportPower (float power);
        void SetExportEnergy (float energy);
        void SetRatedExportPower (unsigned int watts);
        void SetRatedExportEnergy (unsigned int watt_hours);
        void SetExportRamp (unsigned int watts_per_second);
        unsigned int GetExportWatts ();
        unsigned int GetRatedExportPower ();
        unsigned int GetExportPower ();
        unsigned int GetRatedExportEnergy ();
        unsigned int GetExportEnergy ();
        unsigned int GetExportRamp ();
        // import
        void SetImportWatts (unsigned int power);
        void SetImportPower (float power);
        void SetImportEnergy (float energy);
        void SetRatedImportPower (unsigned int watts);
        void SetRatedImportEnergy (unsigned int watt_hours);
        void SetImportRamp (unsigned int watts_per_second);
        unsigned int GetImportWatts ();
        unsigned int GetRatedImportPower ();
        unsigned int GetImportPower ();
        unsigned int GetRatedImportEnergy ();
        unsigned int GetImportEnergy ();
        unsigned int GetImportRamp ();
        // idle
        void SetIdleLosses (unsigned int energy_per_hour);
        unsigned int GetIdleLosses ();
        // DERAS properties
        void SetRemoteTime (unsigned int utc);
        void SetPrice (float price);
        unsigned int GetRemoteTime ();
        float GetPrice ();
        // logging
        void SetLogPath (std::string path);
        void SetLogIncrement (unsigned int inc);
        std::string GetLogPath ();

    private:
        // controls
        virtual void ImportPower ();
        virtual void ExportPower ();
        virtual void IdleLoss ();
        virtual void Log ();

    private:       
        // rated export
        unsigned int rated_export_power_;       // (W) to grid
        unsigned int rated_export_energy_;      // (Wh)
        unsigned int export_ramp_;              // (W s^-1)
        // rated import
        unsigned int rated_import_power_;       // (W) from grid
        unsigned int rated_import_energy_;      // (Wh)
        unsigned int import_ramp_;              // (W s^-1)
        // rated idle 
        unsigned int idle_losses_;              // (Wh h^-1)
        // dynamic properties
        float export_power_;
        float export_energy_;
        float import_power_;
        float import_energy_;
        // control properties
        unsigned int export_watts_;
        unsigned int import_watts_;
        unsigned int remote_utc_;
        float price_;                           // $/kW
        float delta_time_;                      // milliseconds
        // log properties
        unsigned int last_utc_;  // used to prevent multiple logs per cycle
        unsigned int log_inc_;
        std::string log_path_;
};

#endif // DISTRIBUTEDENERGYRESOURCE_H_INCLUDED
