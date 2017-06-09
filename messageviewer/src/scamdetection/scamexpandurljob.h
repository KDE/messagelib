/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

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

#ifndef SCAMEXPANDURLJOB_H
#define SCAMEXPANDURLJOB_H

#include <QObject>
#include <QNetworkReply>
#include "messageviewer_export.h"
namespace MessageViewer {
class ScamExpandUrlJobPrivate;
class MESSAGEVIEWER_EXPORT ScamExpandUrlJob : public QObject
{
    Q_OBJECT
public:
    explicit ScamExpandUrlJob(QObject *parent = nullptr);
    ~ScamExpandUrlJob();

    void expandedUrl(const QUrl &url);
Q_SIGNALS:
    void urlExpanded(const QString &shortUrl, const QString &expandedUrl);
    void expandUrlError(QNetworkReply::NetworkError error);

private:
    void slotError(QNetworkReply::NetworkError error);
    void slotExpandFinished(QNetworkReply *reply);

private:
    ScamExpandUrlJobPrivate *const d;
};
}
#endif // SCAMEXPANDURLJOB_H
