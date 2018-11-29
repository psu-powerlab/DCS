// INCLUDES
#include <string>
#include "include/ServerListener.h"

ServerListener::ServerListener(
    ajn::BusAttachment* bus_ptr,
    ajn::Observer* obs_ptr,
    DistributedEnergyResource* der_ptr,
    const char* server_name) : bus_ptr_(bus_ptr),
                               obs_ptr_(obs_ptr),
                               der_ptr_(der_ptr),
                               server_interface_(server_name),
                               time_(0),
                               price_(0) {
} // end ServerListener

const char* ServerListener::props[] = {"EMSName", "Time", "price"};

// ObjectDiscovered
// - a remote device has advertised the interface we are looking for
void ServerListener::ObjectDiscovered (ajn::ProxyBusObject& proxy) {
    const char* name = proxy.GetUniqueName().c_str();
    std::printf("[LISTENER] : %s has been discovered\n", name);
    bus_ptr_->EnableConcurrentCallbacks();
    proxy.RegisterPropertiesChangedListener(
        server_interface_, props, 3, *this, NULL
    );
} // end ObjectDiscovered

// ObjectLost
// - the remote device is no longer available
void ServerListener::ObjectLost (ajn::ProxyBusObject& proxy) {
    const char* name = proxy.GetUniqueName().c_str();
    const char* path = proxy.GetPath().c_str();
    std::printf("[LISTENER] : %s connection lost\n", name);
    std::printf("\tPath : %s no longer exists\n", path);
} // end ObjectLost

// PropertiesChanged
// - callback to recieve property changed event from remote bus object
void ServerListener::PropertiesChanged (ajn::ProxyBusObject& obj,
                                                const char* interface_name,
                                                const ajn::MsgArg& changed,
                                                const ajn::MsgArg& invalidated,
                                                void* context) {
    size_t nelem = 0;
    ajn::MsgArg* elems = NULL;
    QStatus status = changed.Get("a{sv}", &nelem, &elems);
    if (status == ER_OK) {
        for (size_t i = 0; i < nelem; i++) {
            const char* name;
            ajn::MsgArg* val;
            status = elems[i].Get("{sv}", &name, &val);
            if (status == ER_OK) {
                if (!strcmp(name,"price")) {
                    status = val->Get("i", &price_);

                    // the price is in tenths of a cent when delivered
                    float price = price_/10;
                    der_ptr_->SetPrice(price);
                } else if (!strcmp(name,"Time")) {
                    status = val->Get("u", &time_);
                    der_ptr_->SetRemoteTime(time_);
                }
            } else {
                std::printf("[LISTENER] : invalid property change!\n");
            }
        }
    }
} // end PropertiesChanged
