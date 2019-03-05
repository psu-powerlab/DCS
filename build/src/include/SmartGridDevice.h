/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *    Copyright (c) V2 Systems, LLC.  All rights reserved.
 *
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Permission to use, copy, modify, and/or distribute this software for
 *    any purpose with or without fee is hereby granted, provided that the
 *    above copyright notice and this permission notice appear in all
 *    copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *    PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *    PERFORMANCE OF THIS SOFTWARE.
**********************************************************/

// Author: Tylor Slay
// Description:
//      This class is used to handle control signals from the server as well as
//      notify the server of property changes.

#ifndef SMARTGRIDDEVICE_HPP_INCLUDED
#define SMARTGRIDDEVICE_HPP_INCLUDED

#include <alljoyn/Status.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/BusAttachment.h>
#include "DistributedEnergyResource.h"

class SmartGridDevice : public ajn::BusObject {
public:
    // member methods
    SmartGridDevice (DistributedEnergyResource* der_ptr,
                     ajn::BusAttachment* bus_ptr, 
                     const char* name, 
                     const char* path
    );
    void ImportPowerHandler (const ajn::InterfaceDescription::Member* member,
                             ajn::Message& message
    );
    void ExportPowerHandler (const ajn::InterfaceDescription::Member* member,
                             ajn::Message& message
    );
    QStatus Get (const char* interface, 
                 const char* property, 
                 ajn::MsgArg& value
    );
    QStatus SendPropertiesUpdate ();
    void Loop ();

private:
    // class composition
    DistributedEnergyResource* der_ptr_;
    ajn::BusAttachment* bus_ptr_;
    const ajn::InterfaceDescription::Member* signal_;
    // alljoyn properties
    const char* interface_;
    const char* name_;

    // control properties
    unsigned int last_telemetry_utc_;
    unsigned int last_import_energy_;
    unsigned int last_export_energy_;

};

#endif // SMARTGRIDDEVICE_HPP_INCLUDED
