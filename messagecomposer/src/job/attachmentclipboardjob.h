/*
  SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "MessageCore/AttachmentLoadJob"
#include "messagecomposer_export.h"
namespace MessageComposer
{
/**
 * @brief The AttachmentClipBoardJob class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT AttachmentClipBoardJob : public MessageCore::AttachmentLoadJob
{
    Q_OBJECT
public:
    explicit AttachmentClipBoardJob(QObject *parent = nullptr);
    ~AttachmentClipBoardJob() override;

protected Q_SLOTS:
    void doStart() override;

private:
    void addAttachment(const QByteArray &data, const QString &attachmentName);
};
}
