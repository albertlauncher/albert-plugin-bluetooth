// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothcontrollerprivatebase.h"
#include <albert/logging.h>
using namespace std;

const QString &BluetoothController::address() const { return d->address_; }

const QString &BluetoothController::name() const { return d->name_; }

void BluetoothController::setName(const QString &name)
{
    if (d->name_ == name)
        return;

    DEBG << QString("Bluetooth controller name changed to '%1' (%2)").arg(name, d->address_);

    d->name_ = name;
    emit nameChanged(name);
}

bool BluetoothController::poweredOn() const { return d->powered_on_; }

void BluetoothController::setPoweredOn(bool v)
{
    if (d->powered_on_ == v)
        return;

    DEBG << QString("Bluetooth controller state changed: '%1' (%2 %3)")
                .arg((v ? "on" : "off"), name(), address());

    d->powered_on_ = v;
    emit poweredOnChanged(v);
}

map<QString, shared_ptr<BluetoothDevice>> BluetoothController::devices() const
{
    std::shared_lock lock(d->lock);
    return d->devices_;
}

bool BluetoothController::addDevice(const shared_ptr<BluetoothDevice> &device)
{
    const auto &[it, success] = d->devices_.emplace(device->address(), device);
    if (success)
    {
        DEBG << QString("Bluetooth devices added: '%1' (%2)").arg(d->name_, d->address_);
        emit deviceAdded(device->address());
    }
    else
    {
        DEBG << QString("Bluetooth device already exists: '%1' (%2)").arg(d->name_, d->address_);
    }

    return success;
}

void BluetoothController::removeDevice(QString address)
{
    const auto it = d->devices_.find(address);
    if (it != d->devices_.end())
    {
        DEBG << QString("Bluetooth device removed: '%1' (%2)").arg(it->second->name(), address);
        d->devices_.erase(it);
        emit deviceRemoved(address);
        return true;
    }
    else
    {
        DEBG << QString("BluetoothController::removeDevice: Bluetooth device not found: '%1' (%2)").arg(d->name_, address);
        return false;
    }
}
