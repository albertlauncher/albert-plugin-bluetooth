// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "bluetoothcontrollerprivatebase.h"
#include <albert/logging.h>
using namespace std;

const QString &BluetoothController::address() const { return d->address; }

const QString &BluetoothController::name() const { return d->name; }

void BluetoothController::setName(const QString &name)
{
    if (d->name == name)
        return;

    DEBG << QStringLiteral("Bluetooth controller name changed to '%1' (%2)").arg(name, d->address);

    d->name = name;
    emit nameChanged(name);
}

bool BluetoothController::poweredOn() const { return d->powered_on; }

void BluetoothController::setPoweredOn(bool v)
{
    if (d->powered_on == v)
        return;

    DEBG << QStringLiteral("Bluetooth controller state changed: '%1' (%2 %3)")
                .arg((v ? QStringLiteral("on")
                        : QStringLiteral("off")),
                     name(),
                     address());

    d->powered_on = v;
    emit poweredOnChanged(v);
}
