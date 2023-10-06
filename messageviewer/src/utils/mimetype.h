/*
  SPDX-FileCopyrightText: 2005 Till Adam <adam@kde.org>
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <QString>

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
 * @param iconSize Size of the requested icon, e.g. KIconLoader::Desktop
 * @param fallbackFileName1 When the icon is not found by the given mime type, use the file
 *                              name extensions of these file names to look the icon up.
 *                              Example: "test.zip"
 * @param fallbackFileName2 Fallback for @p fallbackFileName1.
 * @return the full file name of the icon file
 */
[[nodiscard]] QString MESSAGEVIEWER_EXPORT iconPathForMimetype(const QString &mimeType,
                                                               int iconSize,
                                                               const QString &fallbackFileName1 = QString(),
                                                               const QString &fallbackFileName2 = QString());

[[nodiscard]] QString MESSAGEVIEWER_EXPORT iconPathForContent(KMime::Content *node, int iconSize);

struct AttachmentDisplayInfo {
    QString label;
    QString icon;
    bool displayInHeader = false;
};

[[nodiscard]] AttachmentDisplayInfo attachmentDisplayInfo(KMime::Content *node);
}
}
