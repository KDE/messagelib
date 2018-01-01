/*
   Copyright (C) 2016-2018 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
    UrlHashing(const QUrl &url);
    ~UrlHashing();

    static QString canonicalizeUrl(QUrl url);
    static QStringList generatePathsToCheck(const QString &str, const QString &query);
    static QStringList generateHostsToCheck(const QString &str);

    /*long hash, short hash*/
    QHash<QByteArray, QByteArray> hashList() const;
private:
    QUrl mUrl;
};
}

#endif // URLHASHING_H
