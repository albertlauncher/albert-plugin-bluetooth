// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include <albert/logging.h>
using namespace Qt::StringLiterals;
using namespace std;

void BluetoothController::setName(const QString &name)
{
    if (name_ != name)
    {
        name_ = name;
        DEBG << u"Bluetooth controller name changed to '%1' (%2)"_s.arg(name_, address_);
        emit nameChanged(name_);
    }
}
void BluetoothController::setPoweredOn(bool on)
{
    if (powered_on_ != on)
    {
        powered_on_ = on;
        DEBG << u"Bluetooth controller state changed: '%1' (%2 %3)"_s
                    .arg(powered_on_ ? u"on"_s : u"off"_s, name_, address_);
        emit poweredOnChanged(powered_on_);
    }
}

bool BluetoothController::addDevice(const QString &id, const shared_ptr<BluetoothDevice> &dev)
{
    const auto &[it, success] = devices_.emplace(id, dev);
    if (success)
    {
        DEBG << u"Device added: '%1' (%2)"_s.arg(dev->name(), id);
        emit deviceAdded(id);
    }
    else
        DEBG << u"Device already exists: '%1' (%2)"_s.arg(dev->name(), id);

    return success;
}

bool BluetoothController::removeDevice(const QString &id)
{
    const auto it = devices_.find(id);
    if (it != devices_.end())
    {
        DEBG << u"Device removed: '%1' (%2)"_s.arg(it->second->name(), id);
        devices_.erase(it);
        emit deviceRemoved(id);
        return true;
    }
    else
    {
        DEBG << u"Device to remove not found: %1"_s.arg(id);
        return false;
    }
}
