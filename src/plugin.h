// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <albert/extensionplugin.h>
#include <albert/globalqueryhandler.h>
#include <vector>
#include <memory>
class BluetoothController;
class BluetoothDevice;
class PluginPrivateBase;

class Plugin : public albert::util::ExtensionPlugin,
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

    void isEnabled() const;
    void toggle();

    std::vector<std::shared_ptr<BluetoothController>> controllers();  // sorted by address
    bool addController(const std::shared_ptr<BluetoothController> &controller);
    bool removeController(const QString &address);

    std::vector<std::shared_ptr<BluetoothDevice>> devices();  // sorted by address
    bool addDevice(const std::shared_ptr<BluetoothDevice> &device);
    bool removeDevice(const QString &address);

private:

    std::unique_ptr<PluginPrivateBase> d;

signals:

    void controllerAdded(const QString &address);
    void controllerRemoved(const QString &address);

    void deviceAdded(const QString &address);
    void deviceRemoved(const QString &address);

};
