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

    [[nodiscard]] QByteArray data() const;
    void setData(const QByteArray &data);
    /** Indicated the data set with setData() is already encoded with the selected
     *  content transfer encoding.
     *  @default @c false
     */
    void setDataIsEncoded(bool encoded);

    /// created on first call. delete them if you don't use the content
    [[nodiscard]] KMime::Headers::ContentDescription *contentDescription();
    [[nodiscard]] KMime::Headers::ContentDisposition *contentDisposition();
    [[nodiscard]] KMime::Headers::ContentID *contentID();
    [[nodiscard]] KMime::Headers::ContentTransferEncoding *contentTransferEncoding();
    [[nodiscard]] KMime::Headers::ContentType *contentType();

protected Q_SLOTS:
    void process() override;

private:
    Q_DECLARE_PRIVATE(SinglepartJob)
};
}
