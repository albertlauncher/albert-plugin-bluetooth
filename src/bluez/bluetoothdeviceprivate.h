// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <memory>
class OrgBluezDevice1Interface;

class BluetoothDevicePrivate
{
public:
    std::unique_ptr<OrgBluezDevice1Interface> device;
};
