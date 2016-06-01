/*
  Copyright (c) 2013-2016 Montel Laurent <montel@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.

*/

#include "scamcheckshorturl.h"
#include "scamexpandurljob.h"
#include "messageviewer_debug.h"

#include <QFile>

#include <QStandardPaths>
#include <QJsonDocument>

using namespace MessageViewer;
QStringList ScamCheckShortUrl::sSupportedServices = QStringList();

ScamCheckShortUrl::ScamCheckShortUrl(QObject *parent)
    : QObject(parent)
{
    loadLongUrlServices();
}

ScamCheckShortUrl::~ScamCheckShortUrl()
{
}

void ScamCheckShortUrl::expandedUrl(const QUrl &url)
{
    MessageViewer::ScamExpandUrlJob *job = new MessageViewer::ScamExpandUrlJob(this);
    job->expandedUrl(url);
}

bool ScamCheckShortUrl::isShortUrl(const QUrl &url)
{
    if (!url.path().isEmpty() && QString::compare(url.path(), QStringLiteral("/")) && sSupportedServices.contains(url.host())) {
        return true;
    } else  {
        return false;
    }
}

void ScamCheckShortUrl::loadLongUrlServices()
{
    QFile servicesFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("messageviewer/longurlServices.json")));
    if (servicesFile.open(QIODevice::ReadOnly)) {
        QJsonParseError error;
        const QJsonDocument json = QJsonDocument::fromJson(servicesFile.readAll(), &error);
        if (error.error != QJsonParseError::NoError || json.isNull()) {
            qCDebug(MESSAGEVIEWER_LOG) << " Error during read longurlServices.json";
            return;
        }
        const QMap<QString, QVariant> response = json.toVariant().toMap();
        sSupportedServices = response.uniqueKeys();
    } else {
        qCDebug(MESSAGEVIEWER_LOG) << " json file \'longurlServices.json\' not found";
    }
}
