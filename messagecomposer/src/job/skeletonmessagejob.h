/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "jobbase.h"
#include "messagecomposer_export.h"

namespace KMime
{
class Message;
}

namespace MessageComposer
{
class SkeletonMessageJobPrivate;
class InfoPart;
class GlobalPart;

/**
  A message containing only the headers...
*/
class MESSAGECOMPOSER_EXPORT SkeletonMessageJob : public JobBase
{
    Q_OBJECT

public:
    explicit SkeletonMessageJob(InfoPart *infoPart = nullptr, GlobalPart *globalPart = nullptr, QObject *parent = nullptr);
    ~SkeletonMessageJob() override;

    [[nodiscard]] InfoPart *infoPart() const;
    void setInfoPart(InfoPart *part);

    [[nodiscard]] GlobalPart *globalPart() const;
    void setGlobalPart(GlobalPart *part);

    [[nodiscard]] KMime::Message *message() const;

    void start() override;

private:
    Q_DECLARE_PRIVATE(SkeletonMessageJob)

    Q_PRIVATE_SLOT(d_func(), void doStart())
};
} // namespace MessageComposer
