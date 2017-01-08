/*
   Copyright (C) 2014-2017 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "attachmentfromfolderjob.h"
#include "attachmentfromurljob.h"
#include "attachmentfromurlutils.h"
#include "MessageCore/MessageCoreSettings"
#include "messagecore_debug.h"

#include <QMimeDatabase>
#include <QMimeType>

namespace MessageCore
{
MessageCore::AttachmentFromUrlBaseJob *AttachmentFromUrlUtils::createAttachmentJob(const QUrl &url, QObject *parent)
{
    MessageCore::AttachmentFromUrlBaseJob *ajob = nullptr;
    QMimeDatabase db;
    if (db.mimeTypeForUrl(url).name() == QLatin1String("inode/directory")) {
        qCDebug(MESSAGECORE_LOG) << "Creating attachment from folder";
        ajob = new MessageCore::AttachmentFromFolderJob(url, parent);
    } else {
        ajob = new MessageCore::AttachmentFromUrlJob(url, parent);
        qCDebug(MESSAGECORE_LOG) << "Creating attachment from file";
    }
    if (MessageCore::MessageCoreSettings::maximumAttachmentSize() > 0) {
        ajob->setMaximumAllowedSize(MessageCore::MessageCoreSettings::maximumAttachmentSize());
    }
    return ajob;
}
}

