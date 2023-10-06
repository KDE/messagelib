/*
    SPDX-FileCopyrightText: 2011 Martin Bednár <serafean@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "MessageCore/AttachmentFromUrlBaseJob"

#include <KZip>
#include <memory>
namespace MessageCore
{
class AttachmentFromFolderJob : public AttachmentFromUrlBaseJob
{
    Q_OBJECT

public:
    /**
     * Creates a new job.
     *
     * @param url The url of the folder that will be compressed and added as attachment.
     * @param parent The parent object.
     */

    explicit AttachmentFromFolderJob(const QUrl &url = QUrl(), QObject *parent = nullptr);

    /**
     * Destroys the job.
     */

    ~AttachmentFromFolderJob() override;

    /**
     * Sets the @p compression method, either KZip::Deflate or KZip::NoCompression.
     */
    void setCompression(KZip::Compression compression);

    /**
     * Returns the compression method used
     */
    [[nodiscard]] KZip::Compression compression() const;

protected Q_SLOTS:
    void doStart() override;

private:
    //@cond PRIVATE
    class AttachmentLoadJobPrivate;
    std::unique_ptr<AttachmentLoadJobPrivate> const d;
};
}
