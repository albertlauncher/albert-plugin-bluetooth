// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothcontrollerprivate.h"
#include "bluetoothdevice.h"
#include "bluetoothdeviceprivate.h"
#include "bluez.h"
#include <albert/logging.h>
using namespace std;


unique_ptr<BluetoothController> controllerFromNative(const QString &object_path)
{
    auto adapter = make_unique<IAdapter>(bluez_service, object_path, QDBusConnection::systemBus());
    auto properties = make_unique<IProperties>(bluez_service, object_path, QDBusConnection::systemBus());
    auto controller_private = make_unique<BluetoothControllerPrivate>(::move(properties), ::move(adapter));
    return make_unique<BluetoothController>(::move(controller_private));
}


BluetoothController::BluetoothController(unique_ptr<BluetoothControllerPrivate> &&p)
    : address_(p->adapter->address())
    , name_(p->adapter->name())
    , powered_on_(p->adapter->powered())
    , d(::move(p))
{
    // p.
    // p->object_manager = make_unique<IObjectManager>(service, "/", QDBusConnection::systemBus());
    // p->properties = make_unique<IProperties>(service, object_path_adapter, QDBusConnection::systemBus());

    // connect(p->properties.get(), &IProperties::PropertiesChanged, this,
    //         [this](const QString &interface, const QVariantMap &changed, const QStringList &)
    // {
    //     if (interface == IAdapter::staticInterfaceName())
    //         if (auto it = changed.find("Powered"); it != changed.end())
    //         {
    //             static_cast<BluetoothControllerPrivateBluez*>(d.get())->adapter->setPowered(it->toBool());
    //             CRIT << "BluetoothController::PropertiesChanged Powered" << it->toBool();
    //         }
    // });

    // auto pending_reply = p->object_manager->GetManagedObjects();
    // pending_reply.waitForFinished();
    // if (pending_reply.isError())
    //     throw runtime_error("Call to GetManagedObjects failed");
    // auto managed_objects = pending_reply.value();


    // // Find default adapter
    // auto it = managed_objects.find(QDBusObjectPath(object_path_adapter));
    // if (it == managed_objects.end())
    //     throw runtime_error(QString("Default controller %1 not found")
    //                             .arg(object_path_adapter).toStdString());

    // p->adapter = make_unique<IAdapter>(service, object_path_adapter, QDBusConnection::systemBus());
    // p->setPoweredOn(p->adapter->powered());

    // // Create initial devices
    // for (const auto &[path, interfaces] : managed_objects)
    // {
    //     if (interfaces.contains(IDevice1::staticInterfaceName()))
    //     {
    //         auto device = make_unique<IDevice1>(service, path.path(), QDBusConnection::systemBus());

    //         auto btdp = make_unique<BluetoothDevicePrivate>(
    //             nullptr,
    //             *this,
    //             device->Name(),
    //             device->Connected ? BluetoothDevice::State::Connected
    //                               : BluetoothDevice::State::Disconnected,
    //             ::move(device)
    //         );
    //         p.devices.emplace(device, make_shared<BluetoothDevice>(::move(btdp)));
    //     }
    // }
}

BluetoothController::~BluetoothController() {}


void BluetoothController::toggle() { d->adapter->setPowered(!d->adapter->powered()); }

