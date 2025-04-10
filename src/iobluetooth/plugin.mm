// Copyright (c) 2024-2025 Manuel Schneider

#include "plugin.h"
#include "bluetoothcontroller.h"
#include "bluetoothcontrollerprivate.h"
#include "pluginprivatebase.h"
using namespace std;


extern "C" {
int IOBluetoothPreferencesAvailable();
int IOBluetoothPreferenceGetControllerPowerState();
void IOBluetoothPreferenceSetControllerPowerState(int state);
int IOBluetoothPreferenceGetDiscoverableState();
void IOBluetoothPreferenceSetDiscoverableState(int state);
}

Plugin::Plugin(): d(new PluginPrivateBase())
{
    // On macos there is only one active host controller

    auto c = make_shared<BluetoothController>(
        make_unique<BluetoothControllerPrivate>(
            *this, [IOBluetoothHostController defaultController]));


    d->controllers.emplace(c->address(), std::move(c));
}

Plugin::~Plugin()
{

}

void Plugin::isEnabled() const
{

}

void Plugin::toggle()
{

}
