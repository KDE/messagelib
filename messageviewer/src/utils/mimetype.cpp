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

#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/Util>

#include <KIconLoader>
#include <KMime/Content>


QString MessageViewer::Util::iconPathForMimetype(const QString &mimeType, int iconSize,
                                                 const QString &fallbackFileName1,
                                                 const QString &fallbackFileName2)
{
    return IconNameCache::instance()->iconPath(MimeTreeParser::Util::iconNameForMimetype(mimeType,
                                                                                         fallbackFileName1,
                                                                                         fallbackFileName2),
                                               iconSize);
}

QString MessageViewer::Util::iconPathForContent(KMime::Content *node, int size)
{
    return IconNameCache::instance()->iconPath(MimeTreeParser::Util::iconNameForContent(node),
                                               size);
}

MessageViewer::Util::AttachmentDisplayInfo MessageViewer::Util::attachmentDisplayInfo(
    KMime::Content *node)
{
    AttachmentDisplayInfo info;
    info.icon = iconPathForContent(node, KIconLoader::Small);
    info.label = MimeTreeParser::Util::labelForContent(node);

    bool typeBlacklisted = MimeTreeParser::Util::isTypeBlacklisted(node);
    info.displayInHeader = !info.label.isEmpty() && !info.icon.isEmpty() && !typeBlacklisted;
    return info;
}
