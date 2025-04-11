// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QObject>
#include <map>
#include <memory>
class BluetoothDevice;
class BluetoothControllerPrivateBase;

class BluetoothController : public QObject
{
    Q_OBJECT

public:

    BluetoothController();
    ~BluetoothController();

    const QString &address() const;
    const QString &name() const;
    std::map<QString, std::shared_ptr<BluetoothDevice>> devices() const;

    void toggle();
    bool poweredOn() const;


protected:

    void setName(const QString &);
    void setPoweredOn(bool);
    bool addDevice(const std::shared_ptr<BluetoothDevice>&);
    bool removeDevice(QString);

signals:

    void nameChanged(const QString &);
    void poweredOnChanged(bool);
    void deviceAdded(const QString &);
    void deviceRemoved(const QString &);

private:

    std::unique_ptr<BluetoothControllerPrivateBase> d;

};
