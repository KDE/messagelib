/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "scamcheckshorturl.h"
#include "messageviewer_debug.h"
#include "scamexpandurljob.h"

#include <QFile>

#include <QJsonDocument>
#include <QStandardPaths>

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
    if (!url.path().isEmpty() && QString::compare(url.path(), QStringLiteral("/")) && sSupportedServices.contains(url.host())) {
        return true;
    } else {
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
        sSupportedServices = response.keys();
    } else {
        qCDebug(MESSAGEVIEWER_LOG) << " json file \'longurlServices.json\' not found";
    }
}

#include "moc_scamcheckshorturl.cpp"
