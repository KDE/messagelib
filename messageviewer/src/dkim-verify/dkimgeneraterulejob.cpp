/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimgeneraterulejob.h"
#include "dkimmanagerrules.h"
#include "messageviewer_debug.h"
using namespace MessageViewer;

DKIMGenerateRuleJob::DKIMGenerateRuleJob(QObject *parent)
    : QObject(parent)
{
}

DKIMGenerateRuleJob::~DKIMGenerateRuleJob() = default;

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
    // TODO add Domain when we create a rule from base domain.
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

#include "moc_dkimgeneraterulejob.cpp"
