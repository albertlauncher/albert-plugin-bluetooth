// Copyright (c) 2024-2025 Manuel Schneider

#include "plugin.h"
#include "bluetoothcontroller.h"
#include "pluginprivatebase.h"

Plugin::Plugin(): d(new PluginPrivateBase())
{
    // on macos there is only one active host controller
    auto c = std::make_shared<BluetoothController>();
    d->controllers.emplace(c->address(), std::move(c));
}

Plugin::~Plugin()
{

}
