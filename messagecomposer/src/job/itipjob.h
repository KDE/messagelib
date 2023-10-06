/*
  SPDX-FileCopyrightText: 2023 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "contentjobbase.h"
#include "messagecomposer_export.h"

namespace MessageComposer
{
class ItipJobPrivate;
class ItipPart;
/**
 * @brief The ItipJob class
 */
class MESSAGECOMPOSER_EXPORT ItipJob : public ContentJobBase
{
    Q_OBJECT

public:
    explicit ItipJob(ItipPart *itipPart = nullptr, QObject *parent = nullptr);
    ~ItipJob() override;

    [[nodiscard]] ItipPart *itipPart() const;
    void setItipPart(ItipPart *part);

protected Q_SLOTS:
    void doStart() override;
    void process() override;

private:
    Q_DECLARE_PRIVATE(ItipJob)
};
}
