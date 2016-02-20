/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "mailnetworkpluginulrinterceptor.h"
#include "mailnetworkulrinterceptorpluginmanager.h"

#include <KPluginLoader>
#include <KPluginFactory>
#include <QFileInfo>
#include <QVariant>
#include <QSet>
#include <kpluginmetadata.h>

using namespace MessageViewer;

class MailNetworkUlrInterceptorPluginManagerInstancePrivate
{
public:
    MailNetworkUlrInterceptorPluginManagerInstancePrivate()
        : mailNetworkUrlInterceptorPluginManager(new MailNetworkUlrInterceptorPluginManager)
    {
    }

    ~MailNetworkUlrInterceptorPluginManagerInstancePrivate()
    {
        delete mailNetworkUrlInterceptorPluginManager;
    }

    MailNetworkUlrInterceptorPluginManager *mailNetworkUrlInterceptorPluginManager;
};

Q_GLOBAL_STATIC(MailNetworkUlrInterceptorPluginManagerInstancePrivate, sInstance)

class MailNetworkUlrInterceptorPluginInfo
{
public:
    MailNetworkUlrInterceptorPluginInfo()
        : plugin(Q_NULLPTR)
    {

    }
    QString saveName() const;

    KPluginMetaData metaData;
    MessageViewer::MailNetworkPluginUlrInterceptor *plugin;
};

QString MailNetworkUlrInterceptorPluginInfo::saveName() const
{
    return QFileInfo(metaData.fileName()).baseName();
}

namespace
{
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

class MessageViewer::MailNetworkUlrInterceptorPluginManagerPrivate
{
public:
    MailNetworkUlrInterceptorPluginManagerPrivate(MailNetworkUlrInterceptorPluginManager *qq)
        : q(qq)
    {

    }
    void initializePluginList();

    void loadPlugin(MailNetworkUlrInterceptorPluginInfo *item);
    QVector<MessageViewer::MailNetworkPluginUlrInterceptor *> pluginsList() const;

    QVector<MailNetworkUlrInterceptorPluginInfo> mPluginList;
    MailNetworkUlrInterceptorPluginManager *q;
};

void MailNetworkUlrInterceptorPluginManagerPrivate::initializePluginList()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("messageviewer"), [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(QStringLiteral("MessageViewer/UrlInterceptor"));
    });

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        MailNetworkUlrInterceptorPluginInfo info;
        info.metaData = i.previous();

        const QString version = info.metaData.version();
        if (pluginVersion() == version) {

            // only load plugins once, even if found multiple times!
            if (unique.contains(info.saveName())) {
                continue;
            }
            info.plugin = Q_NULLPTR;
            mPluginList.append(info);
            unique.insert(info.saveName());
        }
    }
    QVector<MailNetworkUlrInterceptorPluginInfo>::iterator end(mPluginList.end());
    for (QVector<MailNetworkUlrInterceptorPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

QVector<MessageViewer::MailNetworkPluginUlrInterceptor *> MailNetworkUlrInterceptorPluginManagerPrivate::pluginsList() const
{
    QVector<MessageViewer::MailNetworkPluginUlrInterceptor *> lst;
    QVector<MailNetworkUlrInterceptorPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<MailNetworkUlrInterceptorPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).plugin) {
            lst << (*it).plugin;
        }
    }
    return lst;
}

void MailNetworkUlrInterceptorPluginManagerPrivate::loadPlugin(MailNetworkUlrInterceptorPluginInfo *item)
{
    item->plugin = KPluginLoader(item->metaData.fileName()).factory()->create<MessageViewer::MailNetworkPluginUlrInterceptor>(q, QVariantList() << item->saveName());
}

MailNetworkUlrInterceptorPluginManager *MailNetworkUlrInterceptorPluginManager::self()
{
    return sInstance->mailNetworkUrlInterceptorPluginManager;
}

MailNetworkUlrInterceptorPluginManager::MailNetworkUlrInterceptorPluginManager(QObject *parent)
    : QObject(parent),
      d(new MailNetworkUlrInterceptorPluginManagerPrivate(this))
{
    d->initializePluginList();
}

MailNetworkUlrInterceptorPluginManager::~MailNetworkUlrInterceptorPluginManager()
{
    delete d;
}

QVector<MessageViewer::MailNetworkPluginUlrInterceptor *> MailNetworkUlrInterceptorPluginManager::pluginsList() const
{
    return d->pluginsList();
}
