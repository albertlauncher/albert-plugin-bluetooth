// Copyright (c) 2024-2025 Manuel Schneider

#include "plugin.h"
#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "items.h"
#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include <albert/albert.h>
#include <albert/logging.h>
#include <albert/matcher.h>
#include <albert/notification.h>
#include <albert/standarditem.h>
#include <albert/timeit.h>
ALBERT_LOGGING_CATEGORY("bluetooth")
using namespace albert;
using namespace std;

QString Plugin::defaultTrigger() const { return QStringLiteral("bt "); }

bool Plugin::supportsFuzzyMatching() const { return true; }

void Plugin::setFuzzyMatching(bool val) { fuzzy_ = val; }

vector<RankItem> Plugin::handleGlobalQuery(const Query &query)
{
    vector<RankItem> r;
    Matcher matcher(query.string(), {.fuzzy = fuzzy_});

    if (auto m = matcher.match(BluetoothControllerItem::tr_bluetooth))
    {
        auto item = make_shared<BluetoothControllerItem>(controller);
        item->moveToThread(qApp->thread());
        r.emplace_back(item, m.score());
    }

    for (const auto &device : controller.devices())
        if (auto m = matcher.match(device->name()))
        {
            auto item = make_shared<BluetoothDeviceItem>(device);
            item->moveToThread(qApp->thread());
            r.emplace_back(item, m.score());
        }

    return r;
}

