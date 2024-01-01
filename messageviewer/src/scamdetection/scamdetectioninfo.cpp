/*
  SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "scamdetectioninfo.h"
using namespace MessageViewer;
ScamDetectionInfo::ScamDetectionInfo() = default;

ScamDetectionInfo::~ScamDetectionInfo() = default;

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

bool ScamDetectionInfo::isValid() const
{
    return !mDomainOrEmail.isEmpty();
}

ScamDetectionInfo::ScamDetectionFields ScamDetectionInfo::scamChecks() const
{
    return mFields;
}

QDebug operator<<(QDebug d, const MessageViewer::ScamDetectionInfo &t)
{
    d.space() << "isValid" << t.isValid();
    d.space() << "Enabled" << t.enabled();
    d.space() << "DomainOrName" << t.domainOrEmail();
    d.space() << "scamChecks" << t.scamChecks();
    return d;
}

bool ScamDetectionInfo::operator==(const ScamDetectionInfo &other) const
{
    return domainOrEmail() == other.domainOrEmail() && enabled() == other.enabled() && scamChecks() == other.scamChecks();
}
