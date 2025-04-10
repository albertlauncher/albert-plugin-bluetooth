// Copyright (c) 2024-2025 Manuel Schneider

#include "bluez.h"
#include "plugin.h"
#include "pluginprivatebase.h"
#include <QDBusConnection>
#include <memory>
#include <albert/logging.h>
using namespace std;


class PluginPrivate : public PluginPrivateBase
{
public:
    unique_ptr<IObjectManager> object_manager;
};


Plugin::Plugin(): d(make_unique<PluginPrivateBase>())
{
    auto &p = *static_cast<PluginPrivate*>(d.get());

    p.object_manager = make_unique<IObjectManager>(bluez_service,
                                                   bluez_object_manager_path,
                                                   QDBusConnection::systemBus());

    // Device tracking

    connect(p.object_manager.get(), &IObjectManager::InterfacesAdded,
            this, [this](const QDBusObjectPath &object, NestedVariantMap interfaces)
            {
                auto &p = *static_cast<PluginPrivate*>(d.get());

                if (interfaces.contains(IAdapter::staticInterfaceName()))
                {
                   // Add adapter
                }
                if (interfaces.contains(IDevice1::staticInterfaceName()))
                {
                   // Add device
                }

            });

    connect(p.object_manager.get(), &IObjectManager::InterfacesRemoved,
            this, [this](const QDBusObjectPath &object, const QStringList &interfaces)
            {
                auto &p = *static_cast<PluginPrivate*>(d.get());

                if (interfaces.contains(IAdapter::staticInterfaceName()))
                {
                   // Remove adapter
                }
                if (interfaces.contains(IDevice1::staticInterfaceName()))
                {
                   // Remove device
                }

            });

    auto reply = p.object_manager->GetManagedObjects();
    if (reply.isError())
        throw runtime_error("Call to GetManagedObjects failed");
    auto managed_objects = reply.value();

    for (const auto &[object_path, interfaces] : managed_objects.asKeyValueRange())
    {
        if (auto it = interfaces.find(IAdapter::staticInterfaceName());
            it != interfaces.end())
        {
            // Add adapter
            auto adapter = make_unique<IAdapter>(bluez_service, object_path.path(),
                                                 QDBusConnection::systemBus());
            DEBG << QString("Found adapter: %1").arg(adapter->address());
        }

        if (auto it = interfaces.find(IDevice1::staticInterfaceName());
            it != interfaces.end())
        {
            // Add device
            auto device = make_unique<IDevice1>(bluez_service, object_path.path(), QDBusConnection::systemBus());
            DEBG << QString("Found device: %1").arg(device->address());
        }
    }



    // Get initial list of controllers



    // on macos there is only one active host controller
    // d->controllers.emplace(c->address(), move(c));
}

Plugin::~Plugin()
{

}
