/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "contentjobbase.h"
#include "messagecomposer_export.h"

namespace KMime
{
namespace Headers
{
class ContentDescription;
class ContentDisposition;
class ContentID;
class ContentTransferEncoding;
class ContentType;
}
}

namespace MessageComposer
{
class SinglepartJobPrivate;

/**
 * @brief The SinglepartJob class
 */
class MESSAGECOMPOSER_EXPORT SinglepartJob : public ContentJobBase
{
    Q_OBJECT

public:
    explicit SinglepartJob(QObject *parent = nullptr);
    ~SinglepartJob() override;

    Q_REQUIRED_RESULT QByteArray data() const;
    void setData(const QByteArray &data);

    /// created on first call. delete them if you don't use the content
    Q_REQUIRED_RESULT KMime::Headers::ContentDescription *contentDescription();
    Q_REQUIRED_RESULT KMime::Headers::ContentDisposition *contentDisposition();
    Q_REQUIRED_RESULT KMime::Headers::ContentID *contentID();
    Q_REQUIRED_RESULT KMime::Headers::ContentTransferEncoding *contentTransferEncoding();
    Q_REQUIRED_RESULT KMime::Headers::ContentType *contentType();

protected Q_SLOTS:
    void process() override;

private:
    Q_DECLARE_PRIVATE(SinglepartJob)
};
}

