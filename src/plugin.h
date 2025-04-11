// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <albert/extensionplugin.h>
#include <albert/globalqueryhandler.h>
class PluginPrivateBase;

class Plugin : public albert::ExtensionPlugin,
               public albert::GlobalQueryHandler
{
    ALBERT_PLUGIN

public:

    Plugin();
    ~Plugin();

    QString defaultTrigger() const override;
    bool supportsFuzzyMatching() const override;
    void setFuzzyMatching(bool) override;
    std::vector<albert::RankItem> handleGlobalQuery(const albert::Query &) override;

private:

    std::unique_ptr<PluginPrivateBase> d;

};
