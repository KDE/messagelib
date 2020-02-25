/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "dkimrule.h"

using namespace MessageViewer;
DKIMRule::DKIMRule()
{
}

QString DKIMRule::domain() const
{
    return mDomain;
}

void DKIMRule::setDomain(const QString &domain)
{
    mDomain = domain;
}

QStringList DKIMRule::signedDomainIdentifier() const
{
    return mSignedDomainIdentifier;
}

void DKIMRule::setSignedDomainIdentifier(const QStringList &signedDomainIdentifier)
{
    mSignedDomainIdentifier = signedDomainIdentifier;
}

QString DKIMRule::from() const
{
    return mFrom;
}

void DKIMRule::setFrom(const QString &from)
{
    mFrom = from;
}

bool DKIMRule::enabled() const
{
    return mEnabled;
}

void DKIMRule::setEnabled(bool enabled)
{
    mEnabled = enabled;
}

bool DKIMRule::isValid() const
{
    return !mDomain.isEmpty() && !mFrom.isEmpty() && (mRuleType != DKIMRule::RuleType::Unknown);
}

DKIMRule::RuleType DKIMRule::ruleType() const
{
    return mRuleType;
}

void DKIMRule::setRuleType(RuleType ruleType)
{
    mRuleType = ruleType;
}

QString DKIMRule::listId() const
{
    return mListId;
}

void DKIMRule::setListId(const QString &listId)
{
    mListId = listId;
}

bool DKIMRule::operator==(const DKIMRule &other) const
{
    if (other.domain() == mDomain
        && other.signedDomainIdentifier() == mSignedDomainIdentifier
        && other.from() == mFrom
        && other.listId() == mListId
        && other.ruleType() == mRuleType
        && other.enabled() == mEnabled
        && other.priority() == mPriority) {
        return true;
    }
    return false;
}

bool DKIMRule::operator!=(const DKIMRule &other) const
{
    return !operator==(other);
}

int DKIMRule::priority() const
{
    return mPriority;
}

void DKIMRule::setPriority(int priority)
{
    mPriority = priority;
}

QDebug operator <<(QDebug d, const DKIMRule &t)
{
    d << "mDomain: " << t.domain();
    d << "mSignedDomainIdentifier: " << t.signedDomainIdentifier();
    d << "mFrom: " << t.from();
    d << "mEnabled: " << t.enabled();
    d << "mRuleType " << t.ruleType();
    d << "mListId " << t.listId();
    d << "mPriority " << t.priority();
    return d;
}
