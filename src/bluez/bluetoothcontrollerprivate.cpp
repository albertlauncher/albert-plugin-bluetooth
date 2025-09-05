// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothcontrollerprivate.h"
#include "bluetoothdevice.h"
#include "bluetoothdeviceprivate.h"
#include "bluez.h"
#include <albert/logging.h>
using namespace std;


unique_ptr<BluetoothController> controllerFromNative(const QString &object_path)
{ return make_unique<BluetoothController>(make_unique<BluetoothControllerPrivate>(object_path)); }


BluetoothController::BluetoothController(unique_ptr<BluetoothControllerPrivate> &&p)
    : address_(p->adapter.address())
    , name_(p->adapter.alias())
    , powered_on_(p->adapter.powered())
    , d(::move(p))
{
    // connect(p->properties.get(), &IProperties::PropertiesChanged, this,
    //         [this](const QString &interface, const QVariantMap &changed, const QStringList &)
    // {
    //     if (interface == IAdapter::staticInterfaceName())
    //         if (auto it = changed.find("Powered"); it != changed.end())
    //         {
    //             static_cast<BluetoothControllerPrivateBluez*>(d.get())->adapter.setPowered(it->toBool());
    //             CRIT << "BluetoothController::PropertiesChanged Powered" << it->toBool();
    //         }
    // });
}

BluetoothController::~BluetoothController() {}


void BluetoothController::toggle() { d->adapter.setPowered(!d->adapter.powered()); }

