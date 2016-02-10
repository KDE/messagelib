/*
  Copyright (C) 2005 Till Adam <adam@kde.org>
  Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "utils/mimetype.h"
#include "utils/iconnamecache.h"
#include "messageviewer_debug.h"

#include <QMimeDatabase>

QMimeType MessageViewer::Util::mimetype(const QString &name)
{
    QMimeDatabase db;
    // consider the filename if mimetype cannot be found by content-type
    auto mimeTypes = db.mimeTypesForFileName(name);
    foreach (const auto &mt, mimeTypes) {
        if (mt.name() != QLatin1String("application/octet-stream")) {
            return mt;
        }
    }

    // consider the attachment's contents if neither the Content-Type header
    // nor the filename give us a clue
    return db.mimeTypeForFile(name);
}

QString MessageViewer::Util::fileNameForMimetype(const QString &mimeType, int iconSize,
        const QString &fallbackFileName1,
        const QString &fallbackFileName2)
{
    QString fileName;
    QString tMimeType = mimeType;

    // convert non-registered types to registered types
    if (mimeType == QLatin1String("application/x-vnd.kolab.contact")) {
        tMimeType = QStringLiteral("text/x-vcard");
    } else if (mimeType == QLatin1String("application/x-vnd.kolab.event")) {
        tMimeType = QStringLiteral("application/x-vnd.akonadi.calendar.event");
    } else if (mimeType == QLatin1String("application/x-vnd.kolab.task")) {
        tMimeType = QStringLiteral("application/x-vnd.akonadi.calendar.todo");
    } else if (mimeType == QLatin1String("application/x-vnd.kolab.journal")) {
        tMimeType = QStringLiteral("application/x-vnd.akonadi.calendar.journal");
    } else if (mimeType == QLatin1String("application/x-vnd.kolab.note")) {
        tMimeType = QStringLiteral("application/x-vnd.akonadi.note");
    }
    QMimeDatabase mimeDb;
    auto mime = mimeDb.mimeTypeForName(tMimeType);
    if (mime.isValid()) {
        fileName = mime.iconName();
    } else {
        fileName = QStringLiteral("unknown");
        if (!tMimeType.isEmpty()) {
            qCWarning(MESSAGEVIEWER_LOG) << "unknown mimetype" << tMimeType;
        }
    }
    //WorkAround for #199083
    if (fileName == QLatin1String("text-vcard")) {
        fileName = QStringLiteral("text-x-vcard");
    }
    if (fileName.isEmpty()) {
        fileName = fallbackFileName1;
        if (fileName.isEmpty()) {
            fileName = fallbackFileName2;
        }
        if (!fileName.isEmpty()) {
            fileName = mimeDb.mimeTypeForFile(QLatin1String("/tmp/") + fileName).iconName();
        }
    }

    return IconNameCache::instance()->iconPath(fileName, iconSize);
}