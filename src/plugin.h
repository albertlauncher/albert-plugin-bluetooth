// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include "bluetoothcontroller.h"
#include <albert/globalqueryhandler.h>
#include <albert/extensionplugin.h>

class Plugin : public albert::ExtensionPlugin,
               public albert::GlobalQueryHandler
{
    ALBERT_PLUGIN

public:

    QString defaultTrigger() const override;
    bool supportsFuzzyMatching() const override;
    void setFuzzyMatching(bool) override;
    std::vector<albert::RankItem> handleGlobalQuery(const albert::Query &) override;

private:

    bool fuzzy_;
    BluetoothController controller;

};
