// Copyright (c) 2023-2025 Manuel Schneider

#pragma once
#include <QVariantMap>
#include <QDBusObjectPath>

using NestedVariantMap = QMap<QString, QVariantMap>;
Q_DECLARE_METATYPE(NestedVariantMap)

using ManagedObjects = QMap<QDBusObjectPath, NestedVariantMap>;
Q_DECLARE_METATYPE(ManagedObjects)

class OrgFreedesktopDBusPropertiesInterface;
class OrgFreedesktopDBusObjectManagerInterface;
class OrgBluezAdapter1Interface;
class OrgBluezDevice1Interface;

using IProperties    = OrgFreedesktopDBusPropertiesInterface;
using IObjectManager = OrgFreedesktopDBusObjectManagerInterface;
using IAdapter       = OrgBluezAdapter1Interface;
using IDevice1       = OrgBluezDevice1Interface;

