// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <memory>
#include <QString>
#include "bluez.h"
class BluetoothController;

std::unique_ptr<BluetoothController> controllerFromNative(const QString &object_path);

class BluetoothControllerPrivate
{
public:
    BluetoothControllerPrivate(const QString &object_path)
        : object_path(object_path)
        , adapter(bluez_service, object_path, QDBusConnection::systemBus())
        , properties(bluez_service, object_path, QDBusConnection::systemBus())
    {}

    QString object_path;
    OrgBluezAdapter1Interface adapter;
    OrgFreedesktopDBusPropertiesInterface properties;
};
