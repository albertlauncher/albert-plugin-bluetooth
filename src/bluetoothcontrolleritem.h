// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QObject>
#include <albert/item.h>
#include <set>
#include <memory>
class BluetoothController;

class BluetoothControllerItem : public QObject, public albert::Item
{
    std::shared_ptr<BluetoothController> controller;

public:

    BluetoothControllerItem(std::shared_ptr<BluetoothController>);

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

    static const QString tr_bluetooth;

};

