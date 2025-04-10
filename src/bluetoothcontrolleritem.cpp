// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothcontrolleritem.h"
#include <albert/albert.h>
#include <albert/logging.h>
using namespace albert;
using namespace std;

const QString BluetoothControllerItem::tr_bluetooth = QObject::tr("Bluetooth");

BluetoothControllerItem::BluetoothControllerItem(shared_ptr<BluetoothController> c) :
    controller(::move(c))
{
    connect(controller.get(), &BluetoothController::poweredOnChanged,
            this, &BluetoothControllerItem::notifyObservers);
    connect(controller.get(), &BluetoothController::nameChanged,
            this, &BluetoothControllerItem::notifyObservers);
}

QString BluetoothControllerItem::id() const { return tr_bluetooth; }

QString BluetoothControllerItem::text() const { return tr_bluetooth; }

QString BluetoothControllerItem::subtext() const
{
    if (controller->name().isEmpty())
        return controller->poweredOn() ? tr("Enabled") : tr("Disabled");
    else
        return QStringLiteral("%1  |  %2")
            .arg(controller->poweredOn() ? tr("Enabled") : tr("Disabled"), controller->name());
}

QStringList BluetoothControllerItem::iconUrls() const
{ return {controller->poweredOn() ? QStringLiteral(":bt-active") : QStringLiteral(":bt-inactive")}; }

QString BluetoothControllerItem::inputActionText() const { return tr_bluetooth; }

vector<Action> BluetoothControllerItem::actions() const
{
    return {
        {
            QStringLiteral("pow"),
            controller->poweredOn() ? tr("Disable") : tr("Enable"),
            [this] { controller->toggle(); }
        }
#ifdef Q_OS_MACOS
        ,{
            QStringLiteral("bluetooth"),
            tr("Open Bluetooth settings"),
            [] { openUrl("x-apple.systempreferences:com.apple.Bluetooth"); }
        }
#endif
    };
}

void BluetoothControllerItem::addObserver(Observer *observer) { observers.insert(observer); }

void BluetoothControllerItem::removeObserver(Observer *observer) { observers.erase(observer); }

void BluetoothControllerItem::notifyObservers()
{ for (auto observer : observers) observer->notify(this); }

