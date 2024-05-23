/* SPDX-FileCopyrightText: 2009 Thomas McGuire <mcguire@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "iconnamecache.h"

#include <KIconLoader>
#include <QStandardPaths>

using namespace MessageViewer;

Q_GLOBAL_STATIC(IconNameCache, s_iconNameCache)

IconNameCache *IconNameCache::instance()
{
    return s_iconNameCache;
}

bool IconNameCache::Entry::operator<(const Entry &other) const
{
    const int fileNameCompare = fileName.compare(other.fileName);
    if (fileNameCompare != 0) {
        return fileNameCompare < 0;
    } else {
        return size < other.size;
    }
}

QString IconNameCache::iconPath(const QString &name, int size) const
{
    Entry entry;
    entry.fileName = name;
    entry.size = size;

    const QString val = mCachedEntries.value(entry);
    if (!val.isEmpty()) {
        return val;
    }

    QString fileName = KIconLoader::global()->iconPath(name, size);
    if (fileName.startsWith(QLatin1String(":/"))) {
        fileName = QStringLiteral("qrc") + fileName;
    }
    mCachedEntries.insert(entry, fileName);
    return fileName;
}

QString IconNameCache::iconPathFromLocal(const QString &name) const
{
    Entry entry;
    entry.fileName = name;
    entry.size = 0;

    const QString val = mCachedEntries.value(entry);
    if (!val.isEmpty()) {
        return val;
    }

    const QString fileName = picsPath() + name;
    mCachedEntries.insert(entry, fileName);
    return fileName;
}

QString IconNameCache::picsPath() const
{
    if (mPicsPath.isEmpty()) {
        mPicsPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("libmessageviewer/pics/"), QStandardPaths::LocateDirectory);
    }
    return mPicsPath;
}
