// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothcontroller_p.h"
#include "bluetoothdevice.h"
#include "bluetoothdevice_p.h"
#include "bluez.h"
#include <albert/logging.h>
#include <ranges>
using namespace albert;
using namespace std;
static constexpr const char *service = "org.bluez";
static constexpr const char *object_path_adapter  = "/";

using IProperties = OrgFreedesktopDBusPropertiesInterface;
using IAdapter = OrgBluezAdapter1Interface;

class BluetoothControllerPrivateBluez : public BluetoothControllerPrivate
{
public:
    using BluetoothControllerPrivate::BluetoothControllerPrivate;
    unique_ptr<IProperties> properties;
    unique_ptr<IAdapter> adapter;
};

BluetoothController::BluetoothController():
    d(make_unique<BluetoothControllerPrivateBluez>(
        this,
        false))
{ 
    auto p = static_cast<BluetoothControllerPrivateBluez*>(d.get());


    p->properties = make_unique<IProperties>(service, object_path_adapter, QDBusConnection::systemBus());
    p->adapter = make_unique<IAdapter>(service, object_path_adapter, QDBusConnection::systemBus());

    connect(p->properties.get(), &IProperties::PropertiesChanged, this,
            [this](const QString &interface,
                   const QVariantMap &changed,
                   const QStringList &)
    {
        if (interface == IAdapter::staticInterfaceName())
            if (auto it = changed.find("Powered"); it != changed.end())
                setPoweredOn(it->toBool());
    });

    p->setPoweredOn(p->adapter->powered());

    // Initially create devices for all paired devices
    //TODO
}

BluetoothController::~BluetoothController() {}

void BluetoothController::setPoweredOn(bool power_on)
{
    static_cast<BluetoothControllerPrivateBluez*>(d.get())->adapter->setPowered(power_on);
}

vector<shared_ptr<BluetoothDevice>> BluetoothController::devices()
{
    // auto v = views::values(static_cast<BluetoothControllerPrivateMac*>(d.get())->devices);
    // return vector<shared_ptr<BluetoothDevice>>{v.begin(), v.end()};
    return {};
}

