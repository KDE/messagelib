/*
  SPDX-FileCopyrightText: 2005 Till Adam <adam@kde.org>
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "utils/mimetype.h"
#include "utils/iconnamecache.h"

#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/Util>

#include <KIconLoader>
#include <KMime/Content>

QString MessageViewer::Util::iconPathForMimetype(const QString &mimeType, int iconSize, const QString &fallbackFileName1, const QString &fallbackFileName2)
{
    return IconNameCache::instance()->iconPath(MimeTreeParser::Util::iconNameForMimetype(mimeType, fallbackFileName1, fallbackFileName2), iconSize);
}

QString MessageViewer::Util::iconPathForContent(KMime::Content *node, int size)
{
    return IconNameCache::instance()->iconPath(MimeTreeParser::Util::iconNameForContent(node), size);
}

MessageViewer::Util::AttachmentDisplayInfo MessageViewer::Util::attachmentDisplayInfo(KMime::Content *node)
{
    AttachmentDisplayInfo info;
    info.icon = iconPathForContent(node, KIconLoader::Small);
    info.label = MimeTreeParser::Util::labelForContent(node);

    const bool typeBlacklisted = MimeTreeParser::Util::isTypeBlacklisted(node);
    info.displayInHeader = !info.label.isEmpty() && !info.icon.isEmpty() && !typeBlacklisted;
    return info;
}
