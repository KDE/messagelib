/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include <MessageCore/AttachmentPart>

namespace MessageComposer
{
class Utils
{
public:
    [[nodiscard]] bool containsImage(const MessageCore::AttachmentPart::List &parts);
    [[nodiscard]] bool resizeImage(MessageCore::AttachmentPart::Ptr part);
    void changeFileName(MessageCore::AttachmentPart::Ptr part);
    [[nodiscard]] bool filterRecipients(const QStringList &recipients);
    [[nodiscard]] bool hasImage(const QByteArray &mimetype);
};
}
