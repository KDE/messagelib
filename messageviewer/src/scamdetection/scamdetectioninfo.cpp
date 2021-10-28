/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "scamdetectioninfo.h"
using namespace MessageViewer;
ScamDetectionInfo::ScamDetectionInfo()
{
}

ScamDetectionInfo::~ScamDetectionInfo()
{
}

const QString &ScamDetectionInfo::domainOrEmail() const
{
    return mDomainOrEmail;
}

void ScamDetectionInfo::setDomainOrEmail(const QString &newDomainOrEmail)
{
    mDomainOrEmail = newDomainOrEmail;
}

bool ScamDetectionInfo::enabled() const
{
    return mEnabled;
}

void ScamDetectionInfo::setEnabled(bool newEnabled)
{
    mEnabled = newEnabled;
}

QDebug operator<<(QDebug d, const MessageViewer::ScamDetectionInfo &t)
{
    d << "Enabled " << t.enabled();
    d << "DomainOrName " << t.domainOrEmail();
    return d;
}
