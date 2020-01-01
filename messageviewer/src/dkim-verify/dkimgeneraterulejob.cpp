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

#include "dkimgeneraterulejob.h"
#include "dkimmanagerrules.h"
#include "messageviewer_debug.h"
using namespace MessageViewer;

DKIMGenerateRuleJob::DKIMGenerateRuleJob(QObject *parent)
    : QObject(parent)
{
}

DKIMGenerateRuleJob::~DKIMGenerateRuleJob()
{
}

bool DKIMGenerateRuleJob::canStart() const
{
    if (!mResult.isValid()) {
        qCWarning(MESSAGEVIEWER_LOG) << "Rule is not valid";
        return false;
    }
    return true;
}

bool DKIMGenerateRuleJob::start()
{
    if (!canStart()) {
        deleteLater();
        qCWarning(MESSAGEVIEWER_LOG) << "Impossible to start DKIMGenerateRuleJob";
        return false;
    }
    verifyAndGenerateRule();
    return true;
}

void DKIMGenerateRuleJob::verifyAndGenerateRule()
{
    DKIMRule rule;
    rule.setEnabled(true);
    rule.setFrom(mResult.fromEmail);
    //TODO add Domain when we create a rule from base domain.
    rule.setSignedDomainIdentifier(QStringList() << mResult.sdid);
    rule.setRuleType(DKIMRule::RuleType::MustBeSigned);
    deleteLater();
}

DKIMCheckSignatureJob::CheckSignatureResult DKIMGenerateRuleJob::result() const
{
    return mResult;
}

void DKIMGenerateRuleJob::setResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    mResult = checkResult;
}
