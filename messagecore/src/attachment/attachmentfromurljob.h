/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"

#include "MessageCore/AttachmentFromUrlBaseJob"

namespace MessageCore
{
/**
 * @short A job to load an attachment from an url.
 *
 * @author Constantin Berzan <exit3219@gmail.com>
 */
class MESSAGECORE_EXPORT AttachmentFromUrlJob : public AttachmentFromUrlBaseJob
{
    Q_OBJECT

public:
    /**
     * Creates a new job.
     *
     * @param url The url that will be loaded as attachment.
     * @param parent The parent object.
     */
    explicit AttachmentFromUrlJob(const QUrl &url = QUrl(), QObject *parent = nullptr);

    /**
     * Destroys the job.
     */
    ~AttachmentFromUrlJob() override;

protected Q_SLOTS:
    void doStart() override;

private:
    //@cond PRIVATE
    class AttachmentLoadJobPrivate;
    std::unique_ptr<AttachmentLoadJobPrivate> const d;
    //@endcond
};
}
