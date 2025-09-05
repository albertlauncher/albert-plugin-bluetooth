// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothmanager.h"
#include "bluetoothmanagerprivate.h"
#include <albert/logging.h>
using namespace std;

BluetoothManager::BluetoothManager() : d(new BluetoothManagerPrivate(this)) {}

BluetoothManager::~BluetoothManager() {}

bool BluetoothManager::addController(const shared_ptr<BluetoothController> &ctl)
{
    const auto &[it, success] = controllers_.emplace(ctl->address(), ctl);
    if (success)
    {
        DEBG << QStringLiteral("Controller added: '%1' (%2)").arg(ctl->name(), ctl->address());
        emit controllerAdded(ctl->address());
    }
    else
        DEBG << QStringLiteral("Controller already exists: '%1' (%2)").arg(ctl->name(), ctl->address());

    return success;
}

bool BluetoothManager::removeController(const QString &address)
{
    const auto it = controllers_.find(address);
    if (it != controllers_.end())
    {
        DEBG << QStringLiteral("Controller removed: '%1' (%2)").arg(it->second->name(), address);
        controllers_.erase(it);
        emit controllerRemoved(address);
        return true;
    }
    else
    {
        DEBG << QStringLiteral("Controller to remove not found: %1").arg(address);
        return false;
    }
}
