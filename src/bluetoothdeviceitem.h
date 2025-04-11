// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QObject>
#include <albert/item.h>
#include <set>
class BluetoothDevice;

class BluetoothDeviceItem : public QObject, public albert::Item
{
    std::shared_ptr<BluetoothDevice> device;

public:

    BluetoothDeviceItem(std::shared_ptr<BluetoothDevice>);

    QString id() const override;
    QString text() const override;
    QString subtext() const override;
    QStringList iconUrls() const override;
    QString inputActionText() const override;
    std::vector<albert::Action> actions() const override;

    void addObserver(Observer *observer) override;
    void removeObserver(Observer *observer) override;
    void notifyObservers();
    std::set<Item::Observer*> observers;

};
