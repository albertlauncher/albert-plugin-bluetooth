// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QString>
#include "bluez.h"
class BluetoothDevice;
class BluetoothController;

std::unique_ptr<BluetoothDevice> deviceFromNative(BluetoothController *controller, const QString &object_path);

class BluetoothDevicePrivate
{
public:
    BluetoothDevicePrivate(const QString &object_path)
        : object_path(object_path)
        , device(bluez_service, object_path, QDBusConnection::systemBus())
        , properties(bluez_service, object_path, QDBusConnection::systemBus())
    {}

    QString object_path;
    OrgBluezDevice1Interface device;
    OrgFreedesktopDBusPropertiesInterface properties;
};
