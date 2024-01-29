/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "headerstylemenumanager.h"
#include "headerstyleinterface.h"
#include "headerstyleplugin.h"
#include "headerstylepluginmanager.h"
#include "messageviewer/messageviewersettings.h"
#include "messageviewer_debug.h"
#include "utils/messageviewerutil_p.h"

#include <KActionCollection>
#include <KActionMenu>
#include <KLocalizedString>

#include <QActionGroup>

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

        if ((headerStyle == QLatin1StringView("custom")) && (headerSetDisplayed == QLatin1StringView("custom"))) { // Custom
            headerStyleName = QStringLiteral("custom");
        } else if ((headerStyle == QLatin1StringView("plain")) && (headerSetDisplayed == QLatin1StringView("all"))) { // all
            headerStyleName = QStringLiteral("all-headers");
        } else if ((headerStyle == QLatin1StringView("brief")) && (headerSetDisplayed == QLatin1StringView("brief"))) { // brief
            headerStyleName = QStringLiteral("brief");
        } else if ((headerStyle == QLatin1StringView("enterprise")) && (headerSetDisplayed == QLatin1StringView("rich"))) { // enterprise
            headerStyleName = QStringLiteral("enterprise");
        } else if ((headerStyle == QLatin1StringView("fancy")) && (headerSetDisplayed == QLatin1StringView("rich"))) { // fancy
            headerStyleName = QStringLiteral("fancy");
        } else if ((headerStyle == QLatin1StringView("grantlee")) && (headerSetDisplayed == QLatin1StringView("grantlee"))) { // grantlee
            headerStyleName = QStringLiteral("grantlee");
        } else if ((headerStyle == QLatin1StringView("plain")) && (headerSetDisplayed == QLatin1StringView("rich"))) { // longheader
            headerStyleName = QStringLiteral("long-header");
        } else if ((headerStyle == QLatin1StringView("plain")) && (headerSetDisplayed == QLatin1StringView("standard"))) { // Standard
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

    const QList<MessageViewer::HeaderStylePlugin *> lstPlugin = MessageViewer::HeaderStylePluginManager::self()->pluginsList();
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

HeaderStyleMenuManager::~HeaderStyleMenuManager() = default;

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

#include "moc_headerstylemenumanager.cpp"
