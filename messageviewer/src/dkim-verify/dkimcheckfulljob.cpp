/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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

#include "dkimcheckfulljob.h"
#include "dkimcheckpolicyjob.h"
#include "dkimchecksignaturejob.h"
#include "dkimgeneraterulejob.h"
#include "dkimmanagerkey.h"
#include "dkimstoreresultjob.h"
#include "messageviewer_dkimcheckerdebug.h"
#include <KMessageBox>
#include <KLocalizedString>
using namespace MessageViewer;

DKIMCheckFullJob::DKIMCheckFullJob(QObject *parent)
    : QObject(parent)
{

}

DKIMCheckFullJob::~DKIMCheckFullJob()
{

}

DKIMCheckPolicy DKIMCheckFullJob::policy() const
{
    return mCheckPolicy;
}

void DKIMCheckFullJob::setPolicy(const DKIMCheckPolicy &policy)
{
    mCheckPolicy = policy;
}

void DKIMCheckFullJob::startCheckFullInfo(const Akonadi::Item &item)
{
    if (mCheckPolicy.useAuthenticationResults()) {
#if 0
        DKIMCheckAuthenticationStatusJob *job = new  DKIMCheckAuthenticationStatusJob(job);
        job->setItem(item);
        job->setAuthenticationResult(QStringLiteral("...."));
        connect(job, &DKIMCheckAuthenticationStatusJob::result, this, &DKIMManager::slotCheckAuthenticationStatusResult);
        job->start();
#else
        checkSignature(item);
#endif
    } else {
        checkSignature(item);
    }
}

void DKIMCheckFullJob::checkSignature(const Akonadi::Item &item)
{
    DKIMCheckSignatureJob *job = new DKIMCheckSignatureJob(this);
    connect(job, &DKIMCheckSignatureJob::storeKey, this, &DKIMCheckFullJob::storeKey);
    connect(job, &DKIMCheckSignatureJob::result, this, &DKIMCheckFullJob::slotCheckSignatureResult);
    job->setItem(item);
    job->setPolicy(mCheckPolicy);
    job->start();
}

void DKIMCheckFullJob::startCheckFullInfo(const KMime::Message::Ptr &message)
{
    if (mCheckPolicy.useAuthenticationResults()) {
        //TODO ?
    }
    DKIMCheckSignatureJob *job = new DKIMCheckSignatureJob(this);
    connect(job, &DKIMCheckSignatureJob::storeKey, this, &DKIMCheckFullJob::storeKey);
    connect(job, &DKIMCheckSignatureJob::result, this, &DKIMCheckFullJob::slotCheckSignatureResult);
    job->setPolicy(mCheckPolicy);
    job->setMessage(message);
    job->start();
}

void DKIMCheckFullJob::storeKey(const QString &key, const QString &domain, const QString &selector)
{
   if (mCheckPolicy.saveKey()) {
	storeInKeyManager(key, selector, domain, false);
    }
}

void DKIMCheckFullJob::storeInKeyManager(const QString &key, const QString &domain, const QString &selector, bool verify)
{
    const MessageViewer::KeyInfo info {key, selector, domain};
    Q_UNUSED(verify);
    MessageViewer::DKIMManagerKey::self()->addKey(info);
}

void DKIMCheckFullJob::slotCheckAuthenticationStatusResult(const MessageViewer::DKIMAuthenticationStatusInfo &info, const Akonadi::Item &item)
{
    //TODO check info ! if auth is ok not necessary to checkSignature
    checkSignature(item);
}

void DKIMCheckFullJob::storeResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    if (mCheckPolicy.saveDkimResult()) {
        if (checkResult.status == DKIMCheckSignatureJob::DKIMStatus::Valid
            || checkResult.status == DKIMCheckSignatureJob::DKIMStatus::Invalid
            || checkResult.status == DKIMCheckSignatureJob::DKIMStatus::NeedToBeSigned) {
            DKIMStoreResultJob *job = new DKIMStoreResultJob(this);
            job->setResult(checkResult);
            job->start();
        }
    }
    if (mCheckPolicy.autogenerateRule()) {
        if (checkResult.status == DKIMCheckSignatureJob::DKIMStatus::Valid) {
            DKIMGenerateRuleJob *job = new DKIMGenerateRuleJob(this);
            job->setResult(checkResult);
            if (!job->start()) {
                qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start autogenerate rule";
            }
        }
    }

    qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << "result : status " << checkResult.status << " error : " << checkResult.error << " warning " << checkResult.warning;
    Q_EMIT result(checkResult);
    deleteLater();
}

void DKIMCheckFullJob::slotCheckSignatureResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    if (mCheckPolicy.checkIfEmailShouldBeSigned()
        && (checkResult.status == DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned)) {
        DKIMCheckPolicyJob *job = new DKIMCheckPolicyJob(this);
        connect(job, &DKIMCheckPolicyJob::result, this, &DKIMCheckFullJob::storeResult);
        job->setCheckResult(checkResult);
        job->setEmailAddress(checkResult.fromEmail);
        if (!job->start()) {
            storeResult(checkResult);
        }
    } else {
        storeResult(checkResult);
    }
}

