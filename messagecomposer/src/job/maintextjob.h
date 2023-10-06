/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "contentjobbase.h"
#include "messagecomposer_export.h"

namespace MessageComposer
{
class MainTextJobPrivate;
class TextPart;
/**
 * @brief The MainTextJob class
 */
class MESSAGECOMPOSER_EXPORT MainTextJob : public ContentJobBase
{
    Q_OBJECT

public:
    explicit MainTextJob(TextPart *textPart = nullptr, QObject *parent = nullptr);
    ~MainTextJob() override;

    [[nodiscard]] TextPart *textPart() const;
    void setTextPart(TextPart *part);

protected Q_SLOTS:
    void doStart() override;
    void process() override;

private:
    Q_DECLARE_PRIVATE(MainTextJob)
};
} // namespace MessageComposer
