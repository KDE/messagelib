/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "contentjobbase.h"
#include "messagecomposer_export.h"

namespace MessageComposer
{
class MultipartJobPrivate;

/**
 * @brief The MultipartJob class
 */
class MESSAGECOMPOSER_EXPORT MultipartJob : public ContentJobBase
{
    Q_OBJECT

public:
    explicit MultipartJob(QObject *parent = nullptr);
    ~MultipartJob() override;

    [[nodiscard]] QByteArray multipartSubtype() const;
    void setMultipartSubtype(const QByteArray &subtype);

protected Q_SLOTS:
    void process() override;

private:
    Q_DECLARE_PRIVATE(MultipartJob)
};
}
