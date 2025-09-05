// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothcontrollerprivate.h"
#include "bluetoothdevice.h"
#include "bluetoothdeviceprivate.h"
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
    registerBluezTypes();

    d->object_manager = make_unique<IObjectManager>(bluez_service,
                                                    bluez_object_manager_path,
                                                    QDBusConnection::systemBus());

    // Device tracking

    QObject::connect(d->object_manager.get(), &IObjectManager::InterfacesAdded,
                     this, [this](const QDBusObjectPath &object_path, NestedVariantMap interfaces) {
        for (const auto &[interface, _] : interfaces.asKeyValueRange())
            d->onObjectInterfaceAdded(object_path.path(), interface);
    });

    QObject::connect(d->object_manager.get(), &IObjectManager::InterfacesRemoved,
                     this, [this](const QDBusObjectPath &object_path, const QStringList &interfaces){
        for (const auto &interface : interfaces)
            d->onObjectInterfaceRemoved(object_path.path(), interface);
    });


    // Initial population

    auto reply = d->object_manager->GetManagedObjects();
    reply.waitForFinished();
    if (reply.isError())
        throw runtime_error("Call to GetManagedObjects failed");
    auto managed_objects = reply.value();

    for (const auto &[object_path, interfaces] : managed_objects.asKeyValueRange())
        for (const auto &[interface, _] : interfaces.asKeyValueRange())
            d->onObjectInterfaceAdded(object_path.path(), interface);
}

BluetoothManager::~BluetoothManager()
{

}

void BluetoothManagerPrivate::onObjectInterfaceAdded(const QString object_path, const QString &interface)
{
    if (interface == QString::fromLatin1(IAdapter::staticInterfaceName()))
        q->addController(object_path, controllerFromNative(object_path));

    if (interface == QString::fromLatin1(IDevice1::staticInterfaceName()))
    {
        if (auto it = q->controllers_.find(object_path); it != q->controllers_.end())
            it->second->addDevice(object_path, deviceFromNative(it->second.get(), object_path));
        else
            CRIT << "No controller found for added device" << object_path;
    }
}

void BluetoothManagerPrivate::onObjectInterfaceRemoved(const QString object_path,
                                                       const QString &interface)
{
    if (interface == QString::fromLatin1(IAdapter::staticInterfaceName()))
        q->removeController(object_path);

    if (interface == QString::fromLatin1(IDevice1::staticInterfaceName()))
    {
        if (auto it = q->controllers_.find(object_path); it != q->controllers_.end())
            it->second->removeDevice(object_path);
        else
            CRIT << "No controller found for removed device" << object_path;
    }
}
