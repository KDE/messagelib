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

#include "mailnetworkpluginurlinterceptor.h"
#include "mailnetworkurlinterceptorpluginmanager.h"

#include <KPluginLoader>
#include <KPluginFactory>
#include <QFileInfo>
#include <QVariant>
#include <QSet>
#include <kpluginmetadata.h>

using namespace MessageViewer;

class MailNetworkUrlInterceptorPluginManagerInstancePrivate
{
public:
    MailNetworkUrlInterceptorPluginManagerInstancePrivate()
        : mailNetworkUrlInterceptorPluginManager(new MailNetworkUrlInterceptorPluginManager)
    {
    }

    ~MailNetworkUrlInterceptorPluginManagerInstancePrivate()
    {
        delete mailNetworkUrlInterceptorPluginManager;
    }

    MailNetworkUrlInterceptorPluginManager *mailNetworkUrlInterceptorPluginManager;
};

Q_GLOBAL_STATIC(MailNetworkUrlInterceptorPluginManagerInstancePrivate, sInstance)

class MailNetworkUrlInterceptorPluginInfo
{
public:
    MailNetworkUrlInterceptorPluginInfo()
        : plugin(Q_NULLPTR)
    {

    }
    QString saveName() const;

    KPluginMetaData metaData;
    MessageViewer::MailNetworkPluginUrlInterceptor *plugin;
};

QString MailNetworkUrlInterceptorPluginInfo::saveName() const
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

class MessageViewer::MailNetworkUrlInterceptorPluginManagerPrivate
{
public:
    MailNetworkUrlInterceptorPluginManagerPrivate(MailNetworkUrlInterceptorPluginManager *qq)
        : q(qq)
    {

    }
    void initializePluginList();

    void loadPlugin(MailNetworkUrlInterceptorPluginInfo *item);
    QVector<MessageViewer::MailNetworkPluginUrlInterceptor *> pluginsList() const;

    QVector<MailNetworkUrlInterceptorPluginInfo> mPluginList;
    MailNetworkUrlInterceptorPluginManager *q;
};

void MailNetworkUrlInterceptorPluginManagerPrivate::initializePluginList()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("messageviewer"), [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(QStringLiteral("MessageViewer/UrlInterceptor"));
    });

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        MailNetworkUrlInterceptorPluginInfo info;
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
    QVector<MailNetworkUrlInterceptorPluginInfo>::iterator end(mPluginList.end());
    for (QVector<MailNetworkUrlInterceptorPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

QVector<MessageViewer::MailNetworkPluginUrlInterceptor *> MailNetworkUrlInterceptorPluginManagerPrivate::pluginsList() const
{
    QVector<MessageViewer::MailNetworkPluginUrlInterceptor *> lst;
    QVector<MailNetworkUrlInterceptorPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<MailNetworkUrlInterceptorPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).plugin) {
            lst << (*it).plugin;
        }
    }
    return lst;
}

void MailNetworkUrlInterceptorPluginManagerPrivate::loadPlugin(MailNetworkUrlInterceptorPluginInfo *item)
{
    item->plugin = KPluginLoader(item->metaData.fileName()).factory()->create<MessageViewer::MailNetworkPluginUrlInterceptor>(q, QVariantList() << item->saveName());
}

MailNetworkUrlInterceptorPluginManager *MailNetworkUrlInterceptorPluginManager::self()
{
    return sInstance->mailNetworkUrlInterceptorPluginManager;
}

MailNetworkUrlInterceptorPluginManager::MailNetworkUrlInterceptorPluginManager(QObject *parent)
    : QObject(parent),
      d(new MailNetworkUrlInterceptorPluginManagerPrivate(this))
{
    d->initializePluginList();
}

MailNetworkUrlInterceptorPluginManager::~MailNetworkUrlInterceptorPluginManager()
{
    delete d;
}

QVector<MessageViewer::MailNetworkPluginUrlInterceptor *> MailNetworkUrlInterceptorPluginManager::pluginsList() const
{
    return d->pluginsList();
}
