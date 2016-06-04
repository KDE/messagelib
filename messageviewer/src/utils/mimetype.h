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

#ifndef __MESSAGEVIEWER_UTIL_MIMETYPE_H__
#define __MESSAGEVIEWER_UTIL_MIMETYPE_H__

#include "messageviewer_export.h"

#include <QString>
class QMimeType;

namespace KMime
{
class Content;
}

namespace MessageViewer
{

/**
 * The Util namespace contains a collection of helper functions use in
 * various places.
 */
namespace Util
{

/**
     * Finds the filename of an icon based on the given mimetype or filenames.
     *
     * Always use this functions when looking up icon names for mime types, don't use
     * KMimeType directly.
     *
     * Uses the IconNameCache internally to speed things up.
     *
     * @param mimeType The primary mime type used to find the icon, e.g. "application/zip". Alias
     *                 mimetypes are resolved.
     * @param size Size of the requested icon, e.g. KIconLoader::Desktop
     * @param fallbackFileName(1|2) When the icon is not found by the given mime type, use the file
     *                              name extensions of these file names to look the icon up.
     *                              Example: "test.zip"
     * @return the full file name of the icon file
     */
QString MESSAGEVIEWER_EXPORT iconPathForMimetype(const QString &mimeType, int iconSize,
        const QString &fallbackFileName1 = QString(),
        const QString &fallbackFileName2 = QString());

QString MESSAGEVIEWER_EXPORT iconPathForContent(KMime::Content *node, int iconSize);

struct AttachmentDisplayInfo {
    QString label;
    QString icon;
    bool displayInHeader;
};

AttachmentDisplayInfo attachmentDisplayInfo(KMime::Content *node);

}
}
#endif
