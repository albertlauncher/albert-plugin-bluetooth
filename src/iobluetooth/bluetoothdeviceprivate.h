// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include "bluetoothdeviceprivatebase.h"
#include <Foundation/Foundation.h>
#include <IOBluetooth/IOBluetooth.h>
class BluetoothController;
@class IOBluetoothDevice;
@class BluetoothConnectionHandler;

class BluetoothDevicePrivate : public BluetoothDevicePrivateBase
{
public:
    BluetoothDevicePrivate(BluetoothController &controller, IOBluetoothDevice *_device);

    __strong IOBluetoothDevice *device;
    __strong BluetoothConnectionHandler* connection_handler;

};

