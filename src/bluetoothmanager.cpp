// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothmanager.h"
#include "bluetoothmanagerprivate.h"
#include <albert/logging.h>
using namespace Qt::StringLiterals;
using namespace std;

bool BluetoothManager::addController(const QString &id, const shared_ptr<BluetoothController> &ctl)
{
    const auto &[it, success] = controllers_.emplace(id, ctl);
    if (success)
    {
        DEBG << u"Controller added: '%1' (%2)"_s.arg(ctl->name(), id);
        emit controllerAdded(id);
    }
    else
        DEBG << u"Controller already exists: '%1' (%2)"_s.arg(ctl->name(), id);

    return success;
}

bool BluetoothManager::removeController(const QString &id)
{
    const auto it = controllers_.find(id);
    if (it != controllers_.end())
    {
        DEBG << u"Controller removed: '%1' (%2)"_s.arg(it->second->name(), id);
        controllers_.erase(it);
        emit controllerRemoved(id);
        return true;
    }
    else
    {
        DEBG << u"Controller to remove not found: %1"_s.arg(id);
        return false;
    }
}
