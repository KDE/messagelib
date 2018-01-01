/*
  Copyright (c) 2013-2018 Montel Laurent <montel@kde.org>

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

#ifndef SCAMCHECKSHORTURL_H
#define SCAMCHECKSHORTURL_H

#include "messageviewer_export.h"

#include <QObject>
#include <QUrl>
#include <QStringList>

namespace MessageViewer {
class MESSAGEVIEWER_EXPORT ScamCheckShortUrl : public QObject
{
    Q_OBJECT
public:
    explicit ScamCheckShortUrl(QObject *parent = nullptr);
    ~ScamCheckShortUrl();

    static bool isShortUrl(const QUrl &url);

    void expandedUrl(const QUrl &url);

    static void loadLongUrlServices();

private:
    static QStringList sSupportedServices;
};
}

#endif // SCAMCHECKSHORTURL_H
