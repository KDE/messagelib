/* SPDX-FileCopyrightText: 2009 Thomas McGuire <mcguire@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#pragma once

#include "messageviewer_export.h"
#include <QMap>
#include <QString>

namespace MessageViewer
{
/**
 * This class is a replacement for KIconLoader::iconPath(), because the iconPath()
 * function can be slow for non-existing icons or icons that fall back to a generic icon.
 * Reason is that KIconLoader does slow system calls for finding the icons.
 *
 * The IconNameCache caches the result of iconPath() in a map and solves the slowness.
 */
class MESSAGEVIEWER_EXPORT IconNameCache
{
public:
    static IconNameCache *instance();
    [[nodiscard]] QString iconPath(const QString &name, int size) const;
    [[nodiscard]] QString iconPathFromLocal(const QString &name) const;

private:
    [[nodiscard]] QString picsPath() const;

    class Entry
    {
    public:
        QString fileName;
        int size;

        [[nodiscard]] bool operator<(const Entry &other) const;
    };

    mutable QMap<Entry, QString> mCachedEntries;
    mutable QString mPicsPath;
};
}
