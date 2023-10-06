/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"

#include "attachmentpart.h"

#include <KJob>
#include <memory>

namespace MessageCore
{
/**
 * @short A job to compress the attachment of an email.
 *
 * @author Constantin Berzan <exit3219@gmail.com>
 */
class MESSAGECORE_EXPORT AttachmentCompressJob : public KJob
{
    Q_OBJECT

public:
    /**
     * Creates a new attachment compress job.
     *
     * @param part The part of the attachment to compress.
     * @param parent The parent object.
     */
    explicit AttachmentCompressJob(const AttachmentPart::Ptr &part, QObject *parent = nullptr);

    /**
     * Destroys the attachment compress job.
     */
    ~AttachmentCompressJob() override;

    /**
     * Starts the attachment compress job.
     */
    void start() override;

    /**
     * Sets the original @p part of the compressed attachment.
     */
    void setOriginalPart(const AttachmentPart::Ptr &part);

    /**
     * Returns the original part of the compressed attachment.
     */
    [[nodiscard]] const AttachmentPart::Ptr originalPart() const;

    /**
     * Returns the compressed part of the attachment.
     *
     * @note does not delete it unless it failed...
     */
    [[nodiscard]] AttachmentPart::Ptr compressedPart() const;

    /**
     * Returns whether the compressed part is larger than the original part.
     */
    [[nodiscard]] bool isCompressedPartLarger() const;

private:
    //@cond PRIVATE
    class AttachmentCompressJobPrivate;
    std::unique_ptr<AttachmentCompressJobPrivate> const d;
    //@endcond
};
}
