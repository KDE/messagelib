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

#ifndef MAILNETWORKPLUGINURLINTERCEPTORINTERFACE_H
#define MAILNETWORKPLUGINURLINTERCEPTORINTERFACE_H

#include <QObject>
#include "messageviewer_export.h"
class QWebEngineUrlRequestInfo;
namespace MessageViewer
{
class MESSAGEVIEWER_EXPORT MailNetworkPluginUrlInterceptorInterface : public QObject
{
    Q_OBJECT
public:
    explicit MailNetworkPluginUrlInterceptorInterface(QObject *parent = Q_NULLPTR);
    ~MailNetworkPluginUrlInterceptorInterface();

    virtual void interceptRequest(QWebEngineUrlRequestInfo &info) = 0;
};
}
#endif // MAILNETWORKPLUGINURLINTERCEPTORINTERFACE_H
