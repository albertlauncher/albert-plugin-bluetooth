// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "items.h"
#include <QEventLoop>
#include <albert/logging.h>
#include <albert/messagebox.h>
#include <albert/systemutil.h>
using enum BluetoothController::State;
using enum BluetoothDevice::State;
using namespace Qt::StringLiterals;
using namespace albert::util;
using namespace albert;
using namespace std;


static void showError(const optional<QString> &err){
    if (err)
        albert::util::warning(*err);
};

// -------------------------------------------------------------------------------------------------

const QString BluetoothControllerItem::tr_bluetooth = QObject::tr("Bluetooth");

BluetoothControllerItem::BluetoothControllerItem(shared_ptr<BluetoothController> c) :
    controller(::move(c))
{
    connect(controller.get(), &BluetoothController::stateChanged,
            this, [this]{ dataChanged(); });
    connect(controller.get(), &BluetoothController::nameChanged,
            this, [this]{ dataChanged(); });
}

QString BluetoothControllerItem::id() const { return tr_bluetooth; }

QString BluetoothControllerItem::text() const { return tr_bluetooth; }

QString BluetoothControllerItem::subtext() const
{
    if (controller->name().isEmpty())
        return controller->stateString();
    else
        return u"%1  ·  %2"_s.arg(controller->stateString(), controller->name());
}

QStringList BluetoothControllerItem::iconUrls() const
{
    switch (controller->state()) {
    case PoweredOff:  return {u":bt-inactive"_s};
    case PoweringOn:  return {u":bt-change"_s};
    case PoweredOn:   return {u":bt-active"_s};
    case PoweringOff: return {u":bt-change"_s};
    }
    return {};
}

QString BluetoothControllerItem::inputActionText() const { return tr_bluetooth; }

vector<Action> BluetoothControllerItem::actions() const
{
    vector<Action> actions;

    actions.emplace_back(u"pow"_s, tr("Toggle"), [this] { showError(toggle()); });

#ifdef Q_OS_MACOS
    actions.emplace_back(
        u"bluetooth"_s,
        tr("Open Bluetooth settings"),
        [] { openUrl(u"x-apple.systempreferences:com.apple.Bluetooth"_s); }
        );
#endif

    return actions;
}

optional<QString> BluetoothControllerItem::toggle() const
{
    switch (controller->state()) {
    case PoweredOn:
    case PoweringOn:
        return controller->powerOff();
    case PoweredOff:
    case PoweringOff:
        return controller->powerOn();
    }
    return {};
}


// -------------------------------------------------------------------------------------------------

BluetoothDeviceItem::BluetoothDeviceItem(shared_ptr<BluetoothDevice> dev):
    device(::move(dev))
{
    connect(device.get(), &BluetoothDevice::stateChanged,
            this, [this]{ dataChanged(); });
}

QString BluetoothDeviceItem::id() const { return device->name(); }

QString BluetoothDeviceItem::text() const { return device->name(); }

QString BluetoothDeviceItem::subtext() const
{
    switch (device->state()) {
    case Connecting:
    case Disconnecting:
        return device->stateString() + u'…';
    case Connected:
    case Disconnected:
        return device->stateString();
    }
    return {};
}

QStringList BluetoothDeviceItem::iconUrls() const
{
    switch (device->state()) {
    case Disconnected:  return {u":bt-inactive"_s};
    case Connecting:    return {u":bt-change"_s};
    case Connected:     return {u":bt-active"_s};
    case Disconnecting: return {u":bt-change"_s};
    }
    return {};
}

QString BluetoothDeviceItem::inputActionText() const { return device->name(); }

vector<Action> BluetoothDeviceItem::actions() const
{
    switch (device->state()) {
    case Disconnected:
    case Disconnecting:
        return {{u"toogle"_s, tr("Connect"), [this] { showError(toggle()); }}};
    case Connected:
    case Connecting:
        return {{u"toogle"_s, tr("Disconnect"), [this] { showError(toggle()); }}};
    }
    return {};
}

optional<QString> BluetoothDeviceItem::toggle() const
{
    switch (device->state()) {
    case Connected:
    case Connecting:
        return device->disconnectDevice();
    case Disconnected:
    case Disconnecting:
        if (device->controller().state() == PoweredOn)
            return device->connectDevice();
        else
        {
            QEventLoop loop;
            connect(&device->controller(), &BluetoothController::stateChanged, &loop, &QEventLoop::quit);
            if (auto ret = device->controller().powerOn(); ret)
                return ret;
            loop.exec();

            return device->controller().state() == PoweredOn ? device->connectDevice()
                                                             : tr("Failed to power on controller.");
        }
    }
    return {};
}
