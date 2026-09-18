/*
  SPDX-FileCopyrightText: 2013-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "scamcheckshorturl.h"

#include "messageviewer_debug.h"
#include "scamexpandurljob.h"

#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QUrl>

using namespace Qt::Literals::StringLiterals;
using namespace MessageViewer;
QStringList ScamCheckShortUrl::sSupportedServices = QStringList();

ScamCheckShortUrl::ScamCheckShortUrl(QObject *parent)
    : QObject(parent)
{
    loadLongUrlServices();
}

ScamCheckShortUrl::~ScamCheckShortUrl() = default;

void ScamCheckShortUrl::expandedUrl(const QUrl &url)
{
    auto job = new MessageViewer::ScamExpandUrlJob(this);
    job->expandedUrl(url);
}

bool ScamCheckShortUrl::isShortUrl(const QUrl &url)
{
    if (!url.path().isEmpty() && QString::compare(url.path(), u"/"_s) && sSupportedServices.contains(url.host())) {
        return true;
    } else {
        return false;
    }
}

void ScamCheckShortUrl::loadLongUrlServices()
{
    QFile servicesFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"messageviewer/longurlServices.json"_s));
    if (servicesFile.open(QIODevice::ReadOnly)) {
        QJsonParseError error;
        const QJsonDocument json = QJsonDocument::fromJson(servicesFile.readAll(), &error);
        if (error.error != QJsonParseError::NoError || json.isNull()) {
            qCDebug(MESSAGEVIEWER_LOG) << " Error during read longurlServices.json";
            return;
        }
        sSupportedServices = json.object().keys();
    } else {
        qCDebug(MESSAGEVIEWER_LOG) << " json file \'longurlServices.json\' not found";
    }
}

#include "moc_scamcheckshorturl.cpp"
