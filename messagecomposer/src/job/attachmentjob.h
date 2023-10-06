/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "contentjobbase.h"
#include "messagecomposer_export.h"

#include <MessageCore/AttachmentPart>

namespace MessageComposer
{
class AttachmentJobPrivate;

/**
 * @brief The AttachmentJob class
 */
class MESSAGECOMPOSER_EXPORT AttachmentJob : public ContentJobBase
{
    Q_OBJECT

public:
    explicit AttachmentJob(MessageCore::AttachmentPart::Ptr part, QObject *parent = nullptr);
    ~AttachmentJob() override;

    [[nodiscard]] MessageCore::AttachmentPart::Ptr attachmentPart() const;
    void setAttachmentPart(const MessageCore::AttachmentPart::Ptr &part);

protected Q_SLOTS:
    void doStart() override;
    void process() override;

private:
    Q_DECLARE_PRIVATE(AttachmentJob)
};
}
