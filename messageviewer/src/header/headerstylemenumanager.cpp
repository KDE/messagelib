/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "headerstylemenumanager.h"
#include "header/headerstrategy.h"
#include "header/headerstyle.h"
#include "headerstyleinterface.h"
#include "headerstyleplugin.h"
#include "headerstylepluginmanager.h"
#include "messageviewer/messageviewersettings.h"
#include "messageviewer_debug.h"
#include "utils/messageviewerutil_p.h"

#include <KActionCollection>
#include <KActionMenu>
#include <KLocalizedString>

using namespace MessageViewer;

class MessageViewer::HeaderStyleMenuManagerPrivate
{
public:
    HeaderStyleMenuManagerPrivate(HeaderStyleMenuManager *qq)
        : q(qq)
    {
    }

    void readSettings();
    void writeSettings(const QString &pluginName);
    void initialize(KActionCollection *ac);
    void setPluginName(const QString &pluginName);
    QHash<QString, MessageViewer::HeaderStyleInterface *> lstInterface;
    QActionGroup *group = nullptr;
    KActionMenu *headerMenu = nullptr;
    HeaderStyleMenuManager *const q = nullptr;
};

void HeaderStyleMenuManagerPrivate::setPluginName(const QString &pluginName)
{
    MessageViewer::HeaderStyleInterface *interface = lstInterface.value(pluginName);
    if (interface) {
        if (!interface->action().isEmpty()) {
            interface->activateAction();
        }
    } else {
        if (lstInterface.isEmpty()) {
            qCCritical(MESSAGEVIEWER_LOG) << "No Header Style plugin found !";
            return;
        } else {
            interface = lstInterface.cbegin().value();
            if (!interface->action().isEmpty()) {
                interface->activateAction();
            }
        }
    }
    Q_EMIT q->styleChanged(interface->headerStylePlugin());
}

void HeaderStyleMenuManagerPrivate::readSettings()
{
    QString headerStyleName = MessageViewer::MessageViewerSettings::self()->headerPluginStyleName();
    if (headerStyleName.isEmpty()) {
        const QString headerStyle = MessageViewer::MessageViewerSettings::self()->headerStyle();
        const QString headerSetDisplayed = MessageViewer::MessageViewerSettings::self()->headerSetDisplayed();

        if ((headerStyle == QLatin1String("custom")) && (headerSetDisplayed == QLatin1String("custom"))) { // Custom
            headerStyleName = QStringLiteral("custom");
        } else if ((headerStyle == QLatin1String("plain")) && (headerSetDisplayed == QLatin1String("all"))) { // all
            headerStyleName = QStringLiteral("all-headers");
        } else if ((headerStyle == QLatin1String("brief")) && (headerSetDisplayed == QLatin1String("brief"))) { // brief
            headerStyleName = QStringLiteral("brief");
        } else if ((headerStyle == QLatin1String("enterprise")) && (headerSetDisplayed == QLatin1String("rich"))) { // enterprise
            headerStyleName = QStringLiteral("enterprise");
        } else if ((headerStyle == QLatin1String("fancy")) && (headerSetDisplayed == QLatin1String("rich"))) { // fancy
            headerStyleName = QStringLiteral("fancy");
        } else if ((headerStyle == QLatin1String("grantlee")) && (headerSetDisplayed == QLatin1String("grantlee"))) { // grantlee
            headerStyleName = QStringLiteral("grantlee");
        } else if ((headerStyle == QLatin1String("plain")) && (headerSetDisplayed == QLatin1String("rich"))) { // longheader
            headerStyleName = QStringLiteral("long-header");
        } else if ((headerStyle == QLatin1String("plain")) && (headerSetDisplayed == QLatin1String("standard"))) { // Standard
            headerStyleName = QStringLiteral("standards-header");
        } else {
            qCDebug(MESSAGEVIEWER_LOG) << "unknown style : headerstyle " << headerStyle << " headerstrategy :" << headerSetDisplayed;
        }
        MessageViewer::MessageViewerSettings::self()->setHeaderPluginStyleName(headerStyleName);
    }
    // Fallback
    if (headerStyleName.isEmpty()) {
        headerStyleName = QStringLiteral("fancy");
    }
    setPluginName(headerStyleName);
}

void HeaderStyleMenuManagerPrivate::writeSettings(const QString &pluginName)
{
    MessageViewer::MessageViewerSettings::self()->setHeaderPluginStyleName(pluginName);
    MessageViewer::MessageViewerSettings::self()->save();
}

void HeaderStyleMenuManagerPrivate::initialize(KActionCollection *ac)
{
    headerMenu = new KActionMenu(i18nc("View->", "&Headers"), q);
    if (ac) {
        ac->addAction(QStringLiteral("view_headers"), headerMenu);
    }
    MessageViewer::Util::addHelpTextAction(headerMenu, i18n("Choose display style of message headers"));
    group = new QActionGroup(q);

    const QVector<MessageViewer::HeaderStylePlugin *> lstPlugin = MessageViewer::HeaderStylePluginManager::self()->pluginsList();
    for (MessageViewer::HeaderStylePlugin *plugin : lstPlugin) {
        if (plugin->isEnabled()) {
            MessageViewer::HeaderStyleInterface *interface = plugin->createView(headerMenu, group, ac, q);
            lstInterface.insert(plugin->name(), interface);
            q->connect(interface, &HeaderStyleInterface::styleChanged, q, &HeaderStyleMenuManager::slotStyleChanged);
            q->connect(interface, &HeaderStyleInterface::styleUpdated, q, &HeaderStyleMenuManager::styleUpdated);
        }
    }
}

HeaderStyleMenuManager::HeaderStyleMenuManager(KActionCollection *ac, QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::HeaderStyleMenuManagerPrivate(this))
{
    d->initialize(ac);
}

HeaderStyleMenuManager::~HeaderStyleMenuManager()
{
    delete d;
}

KActionMenu *HeaderStyleMenuManager::menu() const
{
    return d->headerMenu;
}

void MessageViewer::HeaderStyleMenuManager::setPluginName(const QString &pluginName)
{
    d->setPluginName(pluginName);
}

void HeaderStyleMenuManager::slotStyleChanged(MessageViewer::HeaderStylePlugin *plugin)
{
    d->writeSettings(plugin->name());
    Q_EMIT styleChanged(plugin);
}

void HeaderStyleMenuManager::readConfig()
{
    d->readSettings();
}
