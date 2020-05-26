/**
 * Copyright 2015 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "remotecontrolplugin.h"

#include <KLocalizedString>
#include <KPluginFactory>

#include <QDebug>
#include <QDBusConnection>
#include <QPoint>

#include <core/device.h>
#include "plugin_remotecontrol_debug.h"

K_PLUGIN_CLASS_WITH_JSON(RemoteControlPlugin, "kdeconnect_remotecontrol.json")

RemoteControlPlugin::RemoteControlPlugin(QObject* parent, const QVariantList &args)
    : KdeConnectPlugin(parent, args)
{
}

RemoteControlPlugin::~RemoteControlPlugin()
{}

void RemoteControlPlugin::moveCursor(const QPoint &p)
{
    NetworkPacket np(PACKET_TYPE_MOUSEPAD_REQUEST, {
        {QStringLiteral("dx"), p.x()},
        {QStringLiteral("dy"), p.y()}
    });
    sendPacket(np);
}

void RemoteControlPlugin::sendCommand(const QString &name, bool val)
{
    NetworkPacket np(PACKET_TYPE_MOUSEPAD_REQUEST, {{name, val}});
    sendPacket(np);
}

QString RemoteControlPlugin::dbusPath() const
{
    return QStringLiteral("/modules/kdeconnect/devices/") + device()->id() + QStringLiteral("/remotecontrol");
}

#include "remotecontrolplugin.moc"
