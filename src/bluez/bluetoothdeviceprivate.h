// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include "bluetoothdeviceprivatebase.h"
#include <memory>
class BluetoothController;
class IDevice;
// using IDevice = OrgBluezDevice1Interface;

class BluetoothDevicePrivate : public BluetoothDevicePrivateBase
{
public:

    BluetoothDevicePrivate(BluetoothController &controller, std::unique_ptr<IDevice> &&_device);

    std::unique_ptr<IDevice> device;

};
