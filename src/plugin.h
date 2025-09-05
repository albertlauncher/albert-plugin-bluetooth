// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <albert/extensionplugin.h>
#include <albert/indexqueryhandler.h>
#include "bluetoothmanager.h"

class Plugin : public albert::util::ExtensionPlugin,
               public albert::util::IndexQueryHandler
{
    ALBERT_PLUGIN

public:

    QString defaultTrigger() const override;
    void updateIndexItems() override;

private:

    BluetoothManager bt_mgr;

};
