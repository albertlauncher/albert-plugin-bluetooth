// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include "bluetoothcontrollerprivatebase.h"
#include <memory>
class BluetoothController;
class BluetoothDevice;
class IObjectManager;
class IProperties;
class IAdapter;


class BluetoothControllerPrivate : public BluetoothControllerPrivateBase
{
public:
    BluetoothControllerPrivate(IOBluetoothHostController*);

    unique_ptr<IObjectManager> object_manager;
    unique_ptr<IProperties> properties;
    unique_ptr<IAdapter> adapter;
};
