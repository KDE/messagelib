/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_private_export.h"
#include <QString>
#include <QUrl>

namespace WebEngineViewer
{
// https://developers.google.com/safe-browsing/v4/urls-hashing
class WEBENGINEVIEWER_TESTS_EXPORT UrlHashing
{
public:
    explicit UrlHashing(const QUrl &url);
    ~UrlHashing();

    Q_REQUIRED_RESULT static QString canonicalizeUrl(QUrl url);
    Q_REQUIRED_RESULT static QStringList generatePathsToCheck(const QString &str, const QString &query);
    Q_REQUIRED_RESULT static QStringList generateHostsToCheck(const QString &str);

    /*long hash, short hash*/
    Q_REQUIRED_RESULT QHash<QByteArray, QByteArray> hashList() const;

private:
    const QUrl mUrl;
};
}

