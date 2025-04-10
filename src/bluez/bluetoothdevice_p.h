// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QString>
#include "bluetoothdevice.h"
#include "bluez.h"
using IDevice = OrgBluezDevice1Interface;
class BluetoothController;

class BluetoothDevicePrivate
{
public:
    BluetoothDevice *q;
    BluetoothController &controller;
    QString name;
    BluetoothDevice::State state;

    void setStateAndNotify(BluetoothDevice::State s)
    {
        if (state == s)
            return;

        state = s;
        emit q->stateChanged(s);
    }

    IDevice device;
};
