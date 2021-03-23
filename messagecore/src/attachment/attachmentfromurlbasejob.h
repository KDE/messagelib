/*
    SPDX-FileCopyrightText: 2011 Martin Bednár <serafean@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"

#include "attachmentloadjob.h"

#include <QUrl>

namespace MessageCore
{
/**
 * @brief The AttachmentFromUrlBaseJob class
 */
class MESSAGECORE_EXPORT AttachmentFromUrlBaseJob : public AttachmentLoadJob
{
    Q_OBJECT

public:
    explicit AttachmentFromUrlBaseJob(const QUrl &url = QUrl(), QObject *parent = nullptr);
    ~AttachmentFromUrlBaseJob() override;

    /**
     * Returns the url that will be loaded as attachment.
     */
    Q_REQUIRED_RESULT QUrl url() const;

    /**
     * Returns the maximum size the attachment is allowed to have.
     */
    Q_REQUIRED_RESULT qint64 maximumAllowedSize() const;

    /**
     * Sets the @p url of the folder that will be loaded as attachment.
     */
    void setUrl(const QUrl &url);

    /**
     * Sets the maximum @p size the attachment is allowed to have.
     */
    void setMaximumAllowedSize(qint64 size);

protected Q_SLOTS:
    virtual void doStart() override = 0;

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
};
}
