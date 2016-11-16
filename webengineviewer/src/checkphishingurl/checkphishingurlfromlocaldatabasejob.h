/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#ifndef CHECKPHISHINGURLFROMLOCALDATABASE_H
#define CHECKPHISHINGURLFROMLOCALDATABASE_H

#include <QObject>
#include "webengineviewer_export.h"
#include <QUrl>

namespace WebEngineViewer
{
class WEBENGINEVIEWER_EXPORT CheckPhishingUrlFromLocalDataBaseJob : public QObject
{
    Q_OBJECT
public:
    enum UrlStatus {
        Ok = 0,
        MalWare,
        BrokenNetwork,
        InvalidUrl,
        Unknown
    };

    explicit CheckPhishingUrlFromLocalDataBaseJob(QObject *parent = Q_NULLPTR);
    ~CheckPhishingUrlFromLocalDataBaseJob();
    void setCheckPhisingUrl(const QUrl &url);

    void start();
    bool canStart() const;

    QString createHash();

Q_SIGNALS:
    void finished(WebEngineViewer::CheckPhishingUrlFromLocalDataBaseJob::UrlStatus status);

private:
    QUrl mUrl;
};
}
#endif // CHECKPHISHINGURLFROMLOCALDATABASE_H
