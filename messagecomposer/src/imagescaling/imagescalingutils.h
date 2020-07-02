/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef IMAGESCALINGUTILS_H
#define IMAGESCALINGUTILS_H
#include <MessageCore/AttachmentPart>

namespace MessageComposer {
class Utils
{
public:
    Q_REQUIRED_RESULT bool containsImage(const MessageCore::AttachmentPart::List &parts);
    Q_REQUIRED_RESULT bool resizeImage(MessageCore::AttachmentPart::Ptr part);
    void changeFileName(MessageCore::AttachmentPart::Ptr part);
    Q_REQUIRED_RESULT bool filterRecipients(const QStringList &recipients);
    Q_REQUIRED_RESULT bool hasImage(const QByteArray &mimetype);
};
}

#endif // IMAGESCALINGUTILS_H
