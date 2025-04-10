// Copyright (c) 2024-2025 Manuel Schneider

#include "albert/logging.h"
#include "bluetoothcontroller.h"
#include "bluetoothcontroller_p.h"
using namespace albert;
using namespace std;

bool BluetoothController::poweredOn() const { return d->poweredOn(); }

bool BluetoothControllerPrivate::poweredOn() const { return powered_on_; }

void BluetoothControllerPrivate::setPoweredOn(bool v)
{
    if (powered_on_ != v)
    {
        DEBG << "Bluetooth default controller powered" << (v ? "on" : "off");
        powered_on_ = v;
        emit q->poweredOnChanged(v);
    }
}
