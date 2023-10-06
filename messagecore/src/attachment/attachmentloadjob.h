/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"

#include "attachmentpart.h"

#include <KJob>

namespace MessageCore
{
/**
 * @short A base class for jobs to load attachments from different sources.
 *
 * @author Constantin Berzan <exit3219@gmail.com>
 */
class MESSAGECORE_EXPORT AttachmentLoadJob : public KJob
{
    Q_OBJECT

public:
    /**
     * Creates a new attachment load job.
     *
     * @param parent The parent object.
     */
    explicit AttachmentLoadJob(QObject *parent = nullptr);

    /**
     * Destroys the attachment load job.
     */
    ~AttachmentLoadJob() override;

    /**
     * Starts the attachment load job.
     */
    void start() override;

    /**
     * Returns the loaded attachment.
     */
    [[nodiscard]] AttachmentPart::Ptr attachmentPart() const;

protected:
    /**
     * Subclasses use this method to set the loaded @p part.
     */
    void setAttachmentPart(const AttachmentPart::Ptr &part);

protected Q_SLOTS:
    virtual void doStart() = 0;

private:
    //@cond PRIVATE
    class AttachmentLoadJobPrivate;
    std::unique_ptr<AttachmentLoadJobPrivate> const d;
    //@endcond
};
}
