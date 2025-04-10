// Copyright (c) 2024-2025 Manuel Schneider

#include "plugin.h"
#include "pluginprivatebase.h"
#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "bluetoothdeviceitem.h"
#include "bluetoothcontrolleritem.h"
#include <QCoreApplication>
#include <mutex>
#include <albert/matcher.h>
#include <albert/logging.h>
ALBERT_LOGGING_CATEGORY("bluetooth")
using namespace albert::util;
using namespace albert;
using namespace std;

QString Plugin::defaultTrigger() const { return QStringLiteral("bt "); }

bool Plugin::supportsFuzzyMatching() const { return true; }

void Plugin::setFuzzyMatching(bool val) { d->fuzzy = val; }

vector<RankItem> Plugin::handleGlobalQuery(const Query &query)
{
    vector<RankItem> r;
    Matcher matcher(query.string(), {.fuzzy = d->fuzzy});
    Match m;

    shared_lock lock(d->mutex);
    for (const auto &controller : controllers())
        if (m = matcher.match(BluetoothControllerItem::tr_bluetooth,
                              controller->name(),
                              controller->address()),
            m)
        {
            auto item = make_shared<BluetoothControllerItem>(controller);
            item->moveToThread(qApp->thread());
            r.emplace_back(item, m.score());
        }

    for (const auto &device : devices())
        if (m = matcher.match(device->name(), device->address()), m)
        {
            auto item = make_shared<BluetoothDeviceItem>(device);
            item->moveToThread(qApp->thread());
            r.emplace_back(item, m.score());
        }

    return r;
}

vector<shared_ptr<BluetoothController>> Plugin::controllers()
{
    shared_lock lock(d->mutex);
    auto v = d->controllers | views::values;
    return vector<shared_ptr<BluetoothController>>(v.begin(), v.end());
}


bool Plugin::addController(const shared_ptr<BluetoothController> &ctl)
{
    unique_lock lock(d->mutex);
    const auto &[it, success] = d->controllers.emplace(ctl->address(), ctl);
    if (success)
    {
        DEBG << QStringLiteral("Controller added: '%1' (%2)").arg(ctl->name(), ctl->address());
        emit controllerAdded(ctl->address());
    }
    else
        DEBG << QStringLiteral("Controller already exists: '%1' (%2)").arg(ctl->name(), ctl->address());

    return success;
}

bool Plugin::removeController(const QString &address)
{
    unique_lock lock(d->mutex);
    const auto it = d->controllers.find(address);
    if (it != d->controllers.end())
    {
        DEBG << QStringLiteral("Controller removed: '%1' (%2)").arg(it->second->name(), address);
        d->controllers.erase(it);
        emit controllerRemoved(address);
        return true;
    }
    else
    {
        DEBG << QStringLiteral("Controller to remove not found: %1").arg(address);
        return false;
    }
}

vector<shared_ptr<BluetoothDevice>> Plugin::devices()
{
    shared_lock lock(d->mutex);
    auto v = d->devices | views::values;
    return vector<shared_ptr<BluetoothDevice>>(v.begin(), v.end());
}

bool Plugin::addDevice(const shared_ptr<BluetoothDevice> &dev)
{
    unique_lock lock(d->mutex);
    const auto &[it, success] = d->devices.emplace(dev->address(), dev);
    if (success)
    {
        DEBG << QStringLiteral("Device added: '%1' (%2)").arg(dev->name(), dev->address());
        emit deviceAdded(dev->address());
    }
    else
        DEBG << QStringLiteral("Device already exists: '%1' (%2)").arg(dev->name(), dev->address());

    return success;
}

bool Plugin::removeDevice(const QString &address)
{
    unique_lock lock(d->mutex);
    const auto it = d->devices.find(address);
    if (it != d->devices.end())
    {
        DEBG << QStringLiteral("Device removed: '%1' (%2)").arg(it->second->name(), address);
        d->devices.erase(it);
        emit deviceRemoved(address);
        return true;
    }
    else
    {
        DEBG << QStringLiteral("Device to remove not found: %1").arg(address);
        return false;
    }
}

