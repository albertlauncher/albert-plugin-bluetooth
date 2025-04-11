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
                .arg((v ? "on" : "off"), d->name_, d->address_);

    d->powered_on_ = v;
    emit poweredOnChanged(v);
}
