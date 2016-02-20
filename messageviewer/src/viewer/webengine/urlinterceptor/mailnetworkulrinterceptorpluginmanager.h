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

#ifndef MAILNETWORKURLINTERCEPTORPLUGINMANAGER_H
#define MAILNETWORKURLINTERCEPTORPLUGINMANAGER_H

#include <QObject>
#include <QVector>
#include "messageviewer_export.h"
namespace MessageViewer
{
class MailNetworkUrlInterceptorPluginManagerPrivate;
class MailNetworkPluginUrlInterceptor;
class MESSAGEVIEWER_EXPORT MailNetworkUrlInterceptorPluginManager : public QObject
{
    Q_OBJECT
public:
    static MailNetworkUrlInterceptorPluginManager *self();
    explicit MailNetworkUrlInterceptorPluginManager(QObject *parent = Q_NULLPTR);
    ~MailNetworkUrlInterceptorPluginManager();

    QVector<MessageViewer::MailNetworkPluginUrlInterceptor *> pluginsList() const;
private:
    MailNetworkUrlInterceptorPluginManagerPrivate *const d;
};
}

#endif // MAILNETWORKURLINTERCEPTORPLUGINMANAGER_H
