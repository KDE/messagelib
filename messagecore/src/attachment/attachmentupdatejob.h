/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "attachmentpart.h"
#include "messagecore_export.h"
#include <KJob>

namespace MessageCore
{
/**
 * @brief The AttachmentUpdateJob class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECORE_EXPORT AttachmentUpdateJob : public KJob
{
    Q_OBJECT
public:
    explicit AttachmentUpdateJob(const AttachmentPart::Ptr &part, QObject *parent = nullptr);
    ~AttachmentUpdateJob() override;

    void start() override;
    [[nodiscard]] AttachmentPart::Ptr originalPart() const;
    [[nodiscard]] AttachmentPart::Ptr updatedPart() const;

private:
    //@cond PRIVATE
    class AttachmentUpdateJobPrivate;
    std::unique_ptr<AttachmentUpdateJobPrivate> const d;
};
}
