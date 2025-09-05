// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothmanager.h"
#include "bluetoothmanagerprivate.h"
#include <albert/logging.h>
using namespace Qt::StringLiterals;
using namespace std;

bool BluetoothManager::addController(const shared_ptr<BluetoothController> &ctl)
{
    const auto &[it, success] = controllers_.emplace(ctl->address(), ctl);
    if (success)
    {
        DEBG << u"Controller added: '%1' (%2)"_s.arg(ctl->name(), ctl->address());
        emit controllerAdded(ctl->address());
    }
    else
        DEBG << u"Controller already exists: '%1' (%2)"_s.arg(ctl->name(), ctl->address());

    return success;
}

bool BluetoothManager::removeController(const QString &address)
{
    const auto it = controllers_.find(address);
    if (it != controllers_.end())
    {
        DEBG << u"Controller removed: '%1' (%2)"_s.arg(it->second->name(), address);
        controllers_.erase(it);
        emit controllerRemoved(address);
        return true;
    }
    else
    {
        DEBG << u"Controller to remove not found: %1"_s.arg(address);
        return false;
    }
}
