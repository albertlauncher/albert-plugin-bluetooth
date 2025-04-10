// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothcontrollerprivate.h"
#include "bluetoothdevice.h"
#include "bluetoothdeviceprivate.h"
#include "bluez.h"
#include <albert/logging.h>
using namespace std;

BluetoothControllerPrivate::BluetoothControllerPrivate(unique_ptr<IProperties> &&properties,
                                                       unique_ptr<IAdapter> &&adapter):
    BluetoothControllerPrivateBase(
        adapter->address(),
        adapter->name(),
        adapter->powered()
    ),
    properties(::move(properties)),
    adapter(::move(adapter))
{}


BluetoothController::BluetoothController(unique_ptr<BluetoothControllerPrivateBase> &&btcp):
    d(::move(btcp))
{ 
    auto p = static_cast<BluetoothControllerPrivate*>(d.get());

    p->object_manager = make_unique<IObjectManager>(service, "/", QDBusConnection::systemBus());
    p->properties = make_unique<IProperties>(service, object_path_adapter, QDBusConnection::systemBus());

    connect(p->properties.get(), &IProperties::PropertiesChanged, this,
            [this](const QString &interface, const QVariantMap &changed, const QStringList &)
    {
        if (interface == IAdapter::staticInterfaceName())
            if (auto it = changed.find("Powered"); it != changed.end())
            {
                static_cast<BluetoothControllerPrivateBluez*>(d.get())->adapter->setPowered(it->toBool());
                CRIT << "BluetoothController::PropertiesChanged Powered" << it->toBool();
            }
    });

    auto pending_reply = p->object_manager->GetManagedObjects();
    pending_reply.waitForFinished();
    if (pending_reply.isError())
        throw runtime_error("Call to GetManagedObjects failed");
    auto managed_objects = pending_reply.value();


    // Find default adapter
    auto it = managed_objects.find(QDBusObjectPath(object_path_adapter));
    if (it == managed_objects.end())
        throw runtime_error(QString("Default controller %1 not found")
                                .arg(object_path_adapter).toStdString());

    p->adapter = make_unique<IAdapter>(service, object_path_adapter, QDBusConnection::systemBus());
    p->setPoweredOn(p->adapter->powered());

    // Create initial devices
    for (const auto &[path, interfaces] : managed_objects)
    {
        if (interfaces.contains(IDevice1::staticInterfaceName()))
        {
            auto device = make_unique<IDevice1>(service, path.path(), QDBusConnection::systemBus());

            auto btdp = make_unique<BluetoothDevicePrivate>(
                nullptr,
                *this,
                device->Name(),
                device->Connected ? BluetoothDevice::State::Connected
                                  : BluetoothDevice::State::Disconnected,
                ::move(device)
            );
            p.devices.emplace(device, make_shared<BluetoothDevice>(::move(btdp)));
        }
    }
}

BluetoothController::~BluetoothController() {}

void BluetoothController::setPoweredOn(bool power_on)
{
    CRIT << "BluetoothController::setPoweredOn" << power_on;
    static_cast<BluetoothControllerPrivateBluez*>(d.get())->adapter->setPowered(power_on);
}

vector<shared_ptr<BluetoothDevice>> BluetoothController::devices()
{
    // auto v = views::values(static_cast<BluetoothControllerPrivateMac*>(d.get())->devices);
    // return vector<shared_ptr<BluetoothDevice>>{v.begin(), v.end()};
    return {};
}

