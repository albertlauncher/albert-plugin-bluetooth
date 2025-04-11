// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include "bluetoothdevice.h"
#include <QString>
class BluetoothController;

class BluetoothDevicePrivateBase
{
public:

    BluetoothDevicePrivateBase(BluetoothController &controller,
                               const QString &address,
                               const QString &name,
                               BluetoothDevice::State state):
        controller_(controller),
        address_(address),
        name_(name),
        state_(state)
    {
    }
    virtual ~BluetoothDevicePrivateBase() = default;

private:

    BluetoothController &controller_;
    QString address_;
    QString name_;
    BluetoothDevice::State state_;
    friend class BluetoothDevice;

};
