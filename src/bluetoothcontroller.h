// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QObject>
#include <memory>
#include <map>
class BluetoothDevice;
class BluetoothControllerPrivate;

class BluetoothController : public QObject
{
    Q_OBJECT
    class Private;

public:

    BluetoothController(std::unique_ptr<BluetoothControllerPrivate> &&p);
    ~BluetoothController();

    inline const QString &address() { return address_; }
    inline const QString &name() const { return name_; }
    inline bool poweredOn() const { return powered_on_; }
    inline const std::map<QString, std::shared_ptr<BluetoothDevice>> &devices() const { return devices_; }

    void toggle();

protected:

    void setName(const QString &name);
    void setPoweredOn(bool);

signals:

    void nameChanged(const QString &);
    void poweredOnChanged(bool);

    void deviceAdded(const QString &address);
    void deviceRemoved(const QString &address);

private:

    bool addDevice(const std::shared_ptr<BluetoothDevice> &device);
    bool removeDevice(const QString &address);

    void setDeviceState();

    const QString address_;
    QString name_;
    bool powered_on_;
    std::map<QString, std::shared_ptr<BluetoothDevice>> devices_;
    std::unique_ptr<BluetoothControllerPrivate> d;

    friend class BluetoothManager;

};
