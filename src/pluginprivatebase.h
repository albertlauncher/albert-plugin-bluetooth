// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QString>
#include <map>
#include <shared_mutex>
#include <memory>
class BluetoothController;

class PluginPrivateBase
{
public:

    bool fuzzy;
    std::shared_mutex mutex;
    std::map<QString, std::shared_ptr<BluetoothController>> controllers;

};
