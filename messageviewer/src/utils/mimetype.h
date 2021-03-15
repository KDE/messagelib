/*
  SPDX-FileCopyrightText: 2005 Till Adam <adam@kde.org>
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MESSAGEVIEWER_UTIL_MIMETYPE_H
#define MESSAGEVIEWER_UTIL_MIMETYPE_H

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
Q_REQUIRED_RESULT QString MESSAGEVIEWER_EXPORT iconPathForContent(KMime::Content *node, int iconSize);

struct AttachmentDisplayInfo {
    QString label;
    QString icon;
    bool displayInHeader = false;
};

Q_REQUIRED_RESULT AttachmentDisplayInfo attachmentDisplayInfo(KMime::Content *node);
}
}
#endif
