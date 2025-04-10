// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QObject>
#include <memory>
class BluetoothControllerPrivateBase;

class BluetoothController : public QObject
{
    Q_OBJECT

public:

    BluetoothController(std::unique_ptr<BluetoothControllerPrivateBase>&&);
    ~BluetoothController();

    const QString &address() const;
    const QString &name() const;

    void toggle();
    bool poweredOn() const;

protected:

    void setName(const QString &name);
    void setPoweredOn(bool);

signals:

    void nameChanged(const QString &);
    void poweredOnChanged(bool);

private:

    std::unique_ptr<BluetoothControllerPrivateBase> d;

};
