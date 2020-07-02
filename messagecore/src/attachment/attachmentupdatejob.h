/*
   SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ATTACHMENTUPDATEJOB_H
#define ATTACHMENTUPDATEJOB_H

#include "messagecore_export.h"
#include <KJob>
#include "attachmentpart.h"

namespace MessageCore {
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
    Q_REQUIRED_RESULT AttachmentPart::Ptr originalPart() const;
    Q_REQUIRED_RESULT AttachmentPart::Ptr updatedPart() const;
private:
    //@cond PRIVATE
    class Private;
    Private *const d;
};
}

#endif // ATTACHMENTUPDATEJOB_H
