/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimcheckpolicyjob.h"
#include "dkim-verify/dkimmanagerrules.h"
#include "dkim-verify/dmarcmanager.h"
#include "messageviewer_dkimcheckerdebug.h"

using namespace MessageViewer;
DKIMCheckPolicyJob::DKIMCheckPolicyJob(QObject *parent)
    : QObject(parent)
{
}

DKIMCheckPolicyJob::~DKIMCheckPolicyJob() = default;

bool DKIMCheckPolicyJob::canStart() const
{
    return !mEmailAddress.isEmpty();
}

bool DKIMCheckPolicyJob::start()
{
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start DKIMCheckPolicyJob" << mEmailAddress;
        Q_EMIT result(mCheckResult);
        deleteLater();
        return false;
    }
    if (mPolicy.useDMarc()) {
        if (DMARCManager::self()->isNoDMarcServerAddress(mEmailAddress)) {
            Q_EMIT result(mCheckResult);
            deleteLater();
            return true;
        }
        auto job = new DMARCPolicyJob(this);
        job->setEmailAddress(mEmailAddress);
        connect(job, &DMARCPolicyJob::result, this, &DKIMCheckPolicyJob::dmarcPolicyResult);
        if (!job->start()) {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start DKIMCheckPolicyJob" << mEmailAddress;
            Q_EMIT result(mCheckResult);
            deleteLater();
            return false;
        }
    } else {
        if (mPolicy.useDefaultRules()) {
            compareWithDefaultRules();
        } else {
            Q_EMIT result(mCheckResult);
            deleteLater();
        }
    }
    return true;
}

void DKIMCheckPolicyJob::compareWithDefaultRules()
{
    const QList<DKIMRule> rules = DKIMManagerRules::self()->rules();
    for (const DKIMRule &rule : rules) {
        if (rule.enabled()) {
            if (rule.from() == mEmailAddress || rule.from() == QLatin1Char('*')) {
                // Check SDID
                const QStringList signedDomainIdentifier = rule.signedDomainIdentifier();
                for (const QString &ssid : signedDomainIdentifier) {
                    if (mCheckResult.sdid == ssid) {
                        switch (rule.ruleType()) {
                        case DKIMRule::RuleType::Unknown:
                            // Invalid rule !
                            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Invalid rule found " << rule;
                            break;
                        case DKIMRule::RuleType::MustBeSigned:
                            mCheckResult.status = DKIMCheckSignatureJob::DKIMStatus::NeedToBeSigned;
                            break;
                        case DKIMRule::RuleType::CanBeSigned:
                            // Show a warning ?
                            break;
                        case DKIMRule::RuleType::IgnoreEmailNotSigned:
                            // Nothing !
                            break;
                        }
                        break;
                    }
                }
            }
        }
    }
    Q_EMIT result(mCheckResult);
    deleteLater();
}

void DKIMCheckPolicyJob::dmarcPolicyResult(const MessageViewer::DMARCPolicyJob::DMARCResult &value, const QString &emailAddress)
{
    if (value.isValid()) {
        if (mCheckResult.status == DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned) {
            mCheckResult.status = DKIMCheckSignatureJob::DKIMStatus::NeedToBeSigned;
            // qDebug() << " void DKIMCheckPolicyJob::dmarcPolicyResult(const MessageViewer::DMARCPolicyJob::DMARCResult &value)"<<value.mDomain << "value " <<
            // value.mSource;
            // TODO verify it.
            mCheckResult.sdid = value.mSource;
        }
    } else {
        DMARCManager::self()->addNoDMarcServerAddress(emailAddress);
    }
    Q_EMIT result(mCheckResult);
    deleteLater();
}

MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult DKIMCheckPolicyJob::checkResult() const
{
    return mCheckResult;
}

void DKIMCheckPolicyJob::setCheckResult(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    mCheckResult = checkResult;
}

QString DKIMCheckPolicyJob::emailAddress() const
{
    return mEmailAddress;
}

void DKIMCheckPolicyJob::setEmailAddress(const QString &emailAddress)
{
    mEmailAddress = emailAddress;
}

DKIMCheckPolicy DKIMCheckPolicyJob::policy() const
{
    return mPolicy;
}

void DKIMCheckPolicyJob::setPolicy(const DKIMCheckPolicy &policy)
{
    mPolicy = policy;
}

#include "moc_dkimcheckpolicyjob.cpp"
