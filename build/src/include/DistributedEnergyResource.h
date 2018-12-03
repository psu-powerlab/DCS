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
        // set export methods
        void SetExportWatts (unsigned int power);
        void SetExportPower (float power);
        void SetExportEnergy (float energy);
        void SetRatedExportPower (unsigned int watts);
        void SetRatedExportEnergy (unsigned int watt_hours);
        void SetExportRamp (unsigned int watts_per_second);

        // set import methods
        void SetImportWatts (unsigned int power);
        void SetImportPower (float power);
        void SetImportEnergy (float energy);
        void SetRatedImportPower (unsigned int watts);
        void SetRatedImportEnergy (unsigned int watt_hours);
        void SetImportRamp (unsigned int watts_per_second);

        // set idle methods
        void SetIdleLosses (unsigned int energy_per_hour);

        // set remote properties
        void SetRemoteTime (unsigned int utc);
        void SetPrice (float price);

        // set log properties
        void SetLogPath (std::string path);
        void SetLogIncrement (unsigned int inc);


    public:
        // get export methods
        unsigned int GetRatedExportPower ();
        unsigned int GetExportPower ();
        unsigned int GetRatedExportEnergy ();
        unsigned int GetExportEnergy ();
        unsigned int GetExportRamp ();

        // get import methods
        unsigned int GetRatedImportPower ();
        unsigned int GetImportPower ();
        unsigned int GetRatedImportEnergy ();
        unsigned int GetImportEnergy ();
        unsigned int GetImportRamp ();

        // get idle methods
        unsigned int GetIdleLosses ();

        // get log properties
        std::string GetLogPath ();

        // remote properties
        unsigned int GetRemoteTime ();
        float GetPrice ();

    private:
        // controls
        virtual void ImportPower ();
        virtual void ExportPower ();
        virtual void IdleLoss ();
        virtual void Log ();

    private:       
        // rated export properties
        unsigned int rated_export_power_;       // (W) to grid
        unsigned int rated_export_energy_;      // (Wh)
        unsigned int export_ramp_;              // (W s^-1)

        // rated import properties
        unsigned int rated_import_power_;       // (W) from grid
        unsigned int rated_import_energy_;      // (Wh)
        unsigned int import_ramp_;              // (W s^-1)

        // rated idle properties
        unsigned int idle_losses_;              // (Wh h^-1)

    private:
        // dynamic properties
        float export_power_;
        float export_energy_;
        float import_power_;
        float import_energy_;

    private:
        // control properties
        unsigned int export_watts_;
        unsigned int import_watts_;
        unsigned int remote_utc_;
        float price_;
        float delta_time_;  // milliseconds

    private:
        // log settings
        unsigned int last_utc_;
        unsigned int log_inc_;
        std::string log_path_;
};

#endif // DISTRIBUTEDENERGYRESOURCE_H_INCLUDED
