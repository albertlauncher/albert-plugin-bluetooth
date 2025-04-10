// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include "bluetoothcontrollerprivatebase.h"
#include "custom_types.h"
#include <memory>

class BluetoothControllerPrivate : public BluetoothControllerPrivateBase
{
public:
    BluetoothControllerPrivate(std::unique_ptr<IProperties> &&properties,
                               std::unique_ptr<IAdapter> &&adapter);
    std::unique_ptr<IProperties> properties;
    std::unique_ptr<IAdapter> adapter;
};
