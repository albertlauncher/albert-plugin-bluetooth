// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothcontrollerprivate.h"
#include "bluetoothmanager.h"
#include "bluetoothmanagerprivate.h"
#include "bluez.h"
#include <QDBusConnection>
#include <albert/logging.h>
#include <memory>
using namespace std;

BluetoothManager::BluetoothManager()
    : d(make_unique<BluetoothManagerPrivate>(this))
{
    d->object_manager = make_unique<IObjectManager>(bluez_service,
                                                    bluez_object_manager_path,
                                                    QDBusConnection::systemBus());

    // Device tracking

    QObject::connect(d->object_manager.get(), &IObjectManager::InterfacesAdded,
                     this, [this](const QDBusObjectPath &object_path, NestedVariantMap interfaces){
        if (interfaces.contains(QString::fromLatin1(IAdapter::staticInterfaceName())))
            addController(controllerFromNative(object_path.path()));

        // if (interfaces.contains(QString::fromLatin1(IDevice1::staticInterfaceName())))
        // {
        // // Add device
        // }
    });

    QObject::connect(d->object_manager.get(), &IObjectManager::InterfacesRemoved,
                     this, [this](const QDBusObjectPath &object_path, const QStringList &interfaces) {
        if (interfaces.contains(QString::fromLatin1(IAdapter::staticInterfaceName())))
            removeController(IAdapter(bluez_service, object_path.path(), QDBusConnection::systemBus()).address());

        // if (interfaces.contains(QString::fromLatin1(IAdapter::staticInterfaceName())))
        // {
        // // Remove adapter
        // }
        // if (interfaces.contains(QString::fromLatin1(IDevice1::staticInterfaceName())))
        // {
        // // Remove device
        // }

    });

    auto reply = d->object_manager->GetManagedObjects();
    if (reply.isError())
        throw runtime_error("Call to GetManagedObjects failed");
    auto managed_objects = reply.value();

    for (const auto &[object_path, interfaces] : managed_objects.asKeyValueRange())
    {
        if (auto it = interfaces.find(QString::fromLatin1(IAdapter::staticInterfaceName()));
            it != interfaces.end())
            addController(controllerFromNative(object_path.path()));

        // if (auto it = interfaces.find(QString::fromLatin1(IDevice1::staticInterfaceName()));
        //     it != interfaces.end())
        // {
        //     // Add device
        //     auto device = make_unique<IDevice1>(bluez_service, object_path.path(), QDBusConnection::systemBus());
        //     DEBG << u"Found device: %1"_s.arg(device->address());
        // }
    }
}

BluetoothManager::~BluetoothManager()
{

}
