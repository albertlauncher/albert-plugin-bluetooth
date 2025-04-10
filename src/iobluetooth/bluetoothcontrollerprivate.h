// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include "bluetoothcontrollerprivatebase.h"
#include <Foundation/Foundation.h>
#include <IOBluetooth/IOBluetooth.h>
#include <map>
#include <memory>
#include <shared_mutex>
@class BluetoothConnectionObserver;
@class BluetoothPowerDelegate;
class BluetoothController;
class BluetoothDevice;
class Plugin;


class BluetoothControllerPrivate : public BluetoothControllerPrivateBase
{
public:
    BluetoothControllerPrivate(Plugin& plugin, IOBluetoothHostController *controller);

    Plugin& plugin;
    __strong IOBluetoothHostController *controller;
    __strong BluetoothPowerDelegate* power_delegate;
    __strong BluetoothConnectionObserver* connection_observer;
};
