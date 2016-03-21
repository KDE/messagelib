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

#ifndef MESSAGEVIEWER_UTIL_MIMETYPE_H
#define MESSAGEVIEWER_UTIL_MIMETYPE_H

#include "mimetreeparser_export.h"

#include <QString>
class QMimeType;

namespace MessageViewer
{

/**
 * The Util namespace contains a collection of helper functions use in
 * various places.
 */
namespace Util
{

/**
      * Describes the type of the displayed message. This depends on the MIME structure
      * of the mail and on whether HTML mode is enabled (which is decided by htmlMail())
      */
enum HtmlMode {
    Normal,         ///< A normal plaintext message, non-multipart
    Html,           ///< A HTML message, non-multipart
    MultipartPlain, ///< A multipart/alternative message, the plain text part is currently displayed
    MultipartHtml   ///< A multipart/altervative message, the HTML part is currently displayed
};

/**
 * Search mimetype from filename when mimetype is empty or application/octet-stream
 **/
QMimeType MIMETREEPARSER_EXPORT mimetype(const QString &name);

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
QString MIMETREEPARSER_EXPORT fileNameForMimetype(const QString &mimeType, int iconSize,
        const QString &fallbackFileName1 = QString(),
        const QString &fallbackFileName2 = QString());
}
}
#endif