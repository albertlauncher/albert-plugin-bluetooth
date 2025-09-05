// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <memory>
class OrgFreedesktopDBusPropertiesInterface;
class OrgBluezAdapter1Interface;
class BluetoothDevice;
class BluetoothController;
class QString;


std::unique_ptr<BluetoothController> controllerFromNative(const QString &object_path);


class BluetoothControllerPrivate
{
public:
    std::unique_ptr<OrgFreedesktopDBusPropertiesInterface> properties;
    std::unique_ptr<OrgBluezAdapter1Interface> adapter;
};
