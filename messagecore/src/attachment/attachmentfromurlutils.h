/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messagecore_export.h"
#include <QObject>
namespace MessageCore
{
class AttachmentFromUrlBaseJob;
namespace AttachmentFromUrlUtils
{
[[nodiscard]] MESSAGECORE_EXPORT MessageCore::AttachmentFromUrlBaseJob *createAttachmentJob(const QUrl &url, QObject *parent);
}
}
