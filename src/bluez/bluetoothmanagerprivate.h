// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <memory>
class BluetoothManager;
class OrgFreedesktopDBusObjectManagerInterface;

class BluetoothManagerPrivate
{
public:
    BluetoothManager *q;
    std::unique_ptr<OrgFreedesktopDBusObjectManagerInterface> object_manager;
};
