/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef URLHASHING_H
#define URLHASHING_H

#include "webengineviewer_private_export.h"
#include <QUrl>
#include <QString>

namespace WebEngineViewer {
//https://developers.google.com/safe-browsing/v4/urls-hashing
class WEBENGINEVIEWER_TESTS_EXPORT UrlHashing
{
public:
    explicit UrlHashing(const QUrl &url);
    ~UrlHashing();

    static QString canonicalizeUrl(QUrl url);
    static QStringList generatePathsToCheck(const QString &str, const QString &query);
    static QStringList generateHostsToCheck(const QString &str);

    /*long hash, short hash*/
    QHash<QByteArray, QByteArray> hashList() const;
private:
    const QUrl mUrl;
};
}

#endif // URLHASHING_H
