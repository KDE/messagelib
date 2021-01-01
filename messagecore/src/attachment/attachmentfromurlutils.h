/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ATTACHMENTFROMURLUTILS_H
#define ATTACHMENTFROMURLUTILS_H
#include <QObject>
#include "messagecore_export.h"
namespace MessageCore {
class AttachmentFromUrlBaseJob;
namespace AttachmentFromUrlUtils {
Q_REQUIRED_RESULT MESSAGECORE_EXPORT MessageCore::AttachmentFromUrlBaseJob *createAttachmentJob(const QUrl &url, QObject *parent);
}
}

#endif // ATTACHMENTFROMURLUTILS_H
