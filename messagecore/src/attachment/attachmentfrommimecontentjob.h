/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_private_export.h"

#include "attachmentloadjob.h"
#include <memory>
namespace KMime
{
class Content;
}

namespace MessageCore
{
/**
 * @short A job to load an attachment from a mime content.
 *
 * @author Constantin Berzan <exit3219@gmail.com>
 */
class MESSAGECORE_TESTS_EXPORT AttachmentFromMimeContentJob : public AttachmentLoadJob
{
    Q_OBJECT

public:
    /**
     * Creates a new job.
     *
     * @param content The mime content to load the attachment from.
     * @param parent The parent object.
     */
    explicit AttachmentFromMimeContentJob(const KMime::Content *content, QObject *parent = nullptr);

    /**
     * Destroys the job.
     */
    ~AttachmentFromMimeContentJob() override;

    /**
     * Sets the mime @p content to load the attachment from.
     */
    void setMimeContent(const KMime::Content *content);

    /**
     * Returns the mime content to load the attachment from.
     */
    [[nodiscard]] const KMime::Content *mimeContent() const;

protected Q_SLOTS:
    void doStart() override;

private:
    //@cond PRIVATE
    class AttachmentFromMimeContentJobPrivate;
    std::unique_ptr<AttachmentFromMimeContentJobPrivate> const d;
    //@endcond
};
}
