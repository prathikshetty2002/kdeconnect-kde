/*
 * SPDX-FileCopyrightText: 2011 Alejandro Fiestas Olivares <afiestas@kde.org>
 * SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "sendfileitemaction.h"

#include <QList>
#include <QMenu>
#include <QAction>
#include <QWidget>
#include <QVariantList>
#include <QUrl>
#include <QIcon>

#include <KPluginFactory>
#include <KLocalizedString>

#include <interfaces/devicesmodel.h>
#include <interfaces/dbusinterfaces.h>

#include <dbushelper.h>

#include "kdeconnect_fileitemaction_debug.h"

K_PLUGIN_CLASS_WITH_JSON(SendFileItemAction, "kdeconnectsendfile.json")

SendFileItemAction::SendFileItemAction(QObject* parent, const QVariantList& ): KAbstractFileItemActionPlugin(parent)
{
}

QList<QAction*> SendFileItemAction::actions(const KFileItemListProperties& fileItemInfos, QWidget* parentWidget)
{
    QList<QAction*> actions;

    DaemonDbusInterface iface;
    if (!iface.isValid()) {
        return actions;
    }

    QDBusPendingReply<QStringList> reply = iface.devices(true, true);
    reply.waitForFinished();
    const QStringList devices = reply.value();
    for (const QString& id : devices) {
        DeviceDbusInterface deviceIface(id);
        if (!deviceIface.isValid()) {
            continue;
        }
        if (!deviceIface.hasPlugin(QStringLiteral("kdeconnect_share"))) {
            continue;
        }
        QAction* action = new QAction(QIcon::fromTheme(deviceIface.iconName()), deviceIface.name(), parentWidget);
        action->setProperty("id", id);
        action->setProperty("urls", QVariant::fromValue(fileItemInfos.urlList()));
        action->setProperty("parentWidget", QVariant::fromValue(parentWidget));
        connect(action, &QAction::triggered, this, &SendFileItemAction::sendFile);
        actions += action;
    }

    if (actions.count() > 1) {
        QAction* menuAction = new QAction(QIcon::fromTheme(QStringLiteral("kdeconnect")), i18n("Send via KDE Connect"), parentWidget);
        QMenu* menu = new QMenu(parentWidget);
        menu->addActions(actions);
        menuAction->setMenu(menu);
        return QList<QAction*>() << menuAction;
    } else {
        if(actions.count() == 1) {
            actions.first()->setText(i18n("Send to '%1' via KDE Connect", actions.first()->text()));
        }
        return actions;
    }
}

void SendFileItemAction::sendFile()
{
    const QList<QUrl> urls = sender()->property("urls").value<QList<QUrl>>();
    QString id = sender()->property("id").toString();
    for (const QUrl& url : urls) {
        QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.kdeconnect"), QStringLiteral("/modules/kdeconnect/devices/") + id + QStringLiteral("/share"), QStringLiteral("org.kde.kdeconnect.device.share"), QStringLiteral("shareUrl"));
        msg.setArguments(QVariantList() << url.toString());
        QDBusConnection::sessionBus().asyncCall(msg);
    }
}

#include "sendfileitemaction.moc"
