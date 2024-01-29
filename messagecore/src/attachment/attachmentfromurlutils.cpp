/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentfromurlutils.h"
#include "MessageCore/MessageCoreSettings"
#include "attachmentfromfolderjob.h"
#include "attachmentfromurljob.h"
#include "messagecore_debug.h"

#include <QMimeDatabase>

namespace MessageCore
{
MessageCore::AttachmentFromUrlBaseJob *AttachmentFromUrlUtils::createAttachmentJob(const QUrl &url, QObject *parent)
{
    MessageCore::AttachmentFromUrlBaseJob *ajob = nullptr;
    QMimeDatabase db;
    if (db.mimeTypeForUrl(url).name() == QLatin1StringView("inode/directory")) {
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
