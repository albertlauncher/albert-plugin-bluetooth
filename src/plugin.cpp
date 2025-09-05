// Copyright (c) 2024-2025 Manuel Schneider

#include "plugin.h"
#include "bluetoothdevice.h"
#include "bluetoothcontroller.h"
#include "items.h"
#include <ranges>
#include <albert/logging.h>
ALBERT_LOGGING_CATEGORY("bluetooth")
using namespace albert::util;
using namespace std;

QString Plugin::defaultTrigger() const { return QStringLiteral("bt "); }

void Plugin::updateIndexItems()
{
    vector<IndexItem> index_items;

    for (const auto &controller : bt_mgr.controllers() | views::values)
    {
        auto controller_item = make_shared<BluetoothControllerItem>(controller);
        index_items.emplace_back(controller_item, controller->name());
        index_items.emplace_back(controller_item, controller->address());

        for (const auto &device : controller->devices() | views::values)
        {
            auto device_item = make_shared<BluetoothDeviceItem>(device);
            index_items.emplace_back(device_item, device->name());
            index_items.emplace_back(device_item, device->address());
        }
    }

    setIndexItems(::move(index_items));
}
