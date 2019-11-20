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

#include "dkimmanager.h"
#include "messageviewer_dkimcheckerdebug.h"
#include "dkimmanagerkey.h"
#include "dkimresultattribute.h"
#include "dkimstoreresultjob.h"
#include "dkimcheckpolicyjob.h"
#include "dkimgeneraterulejob.h"
#include "dkimcheckauthenticationstatusjob.h"
#include "settings/messageviewersettings.h"
#include <AkonadiCore/AttributeFactory>
using namespace MessageViewer;
DKIMManager::DKIMManager(QObject *parent)
    : QObject(parent)
{
    Akonadi::AttributeFactory::registerAttribute<MessageViewer::DKIMResultAttribute>();
}

DKIMManager::~DKIMManager()
{
}

DKIMManager *DKIMManager::self()
{
    static DKIMManager s_self;
    return &s_self;
}

void DKIMManager::checkDKim(const Akonadi::Item &item)
{
    if (mCheckPolicy.saveDkimResult()) {
        if (item.hasAttribute<MessageViewer::DKIMResultAttribute>()) {
            const MessageViewer::DKIMResultAttribute *const attr
                = item.attribute<MessageViewer::DKIMResultAttribute>();
            if (attr) {
                DKIMCheckSignatureJob::CheckSignatureResult checkResult;
                checkResult.item = item;
                checkResult.error = static_cast<DKIMCheckSignatureJob::DKIMError>(attr->error());
                checkResult.warning = static_cast<DKIMCheckSignatureJob::DKIMWarning>(attr->warning());
                checkResult.status = static_cast<DKIMCheckSignatureJob::DKIMStatus>(attr->status());
                qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << "result : status " << checkResult.status << " error : " << checkResult.error << " warning " << checkResult.warning;
                Q_EMIT result(checkResult);
                return;
            }
        }
    }

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

void DKIMManager::checkSignature(const Akonadi::Item &item)
{
    DKIMCheckSignatureJob *job = new DKIMCheckSignatureJob(this);
    connect(job, &DKIMCheckSignatureJob::storeKey, this, &DKIMManager::storeKey);
    connect(job, &DKIMCheckSignatureJob::result, this, &DKIMManager::slotCheckSignatureResult);
    job->setItem(item);
    job->setPolicy(mCheckPolicy);
    job->start();
}

void DKIMManager::clearInfoWidget()
{
    Q_EMIT clearInfo();
}

void DKIMManager::checkDKim(const KMime::Message::Ptr &message)
{
    if (mCheckPolicy.useAuthenticationResults()) {
        //TODO ?
    }
    DKIMCheckSignatureJob *job = new DKIMCheckSignatureJob(this);
    connect(job, &DKIMCheckSignatureJob::storeKey, this, &DKIMManager::storeKey);
    connect(job, &DKIMCheckSignatureJob::result, this, &DKIMManager::slotCheckSignatureResult);
    job->setPolicy(mCheckPolicy);
    job->setMessage(message);
    job->start();
}

void DKIMManager::storeKey(const QString &key, const QString &domain, const QString &selector)
{
    if (mCheckPolicy.saveKey()) {
        const MessageViewer::KeyInfo info {key, selector, domain};
        MessageViewer::DKIMManagerKey::self()->addKey(info);
    }
}

void DKIMManager::slotCheckAuthenticationStatusResult(const MessageViewer::DKIMAuthenticationStatusInfo &info, const Akonadi::Item &item)
{
    //TODO check info ! if auth is ok not necessary to checkSignature
    checkSignature(item);
}

void DKIMManager::storeResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
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
}

void DKIMManager::slotCheckSignatureResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    if (mCheckPolicy.checkIfEmailShouldBeSigned()
        && (checkResult.status == DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned)) {
        DKIMCheckPolicyJob *job = new DKIMCheckPolicyJob(this);
        connect(job, &DKIMCheckPolicyJob::result, this, &DKIMManager::storeResult);
        job->setCheckResult(checkResult);
        job->setEmailAddress(checkResult.fromEmail);
        if (!job->start()) {
            storeResult(checkResult);
        }
    } else {
        storeResult(checkResult);
    }
}

DKIMCheckPolicy DKIMManager::policy() const
{
    return mCheckPolicy;
}

void DKIMManager::setPolicy(const DKIMCheckPolicy &policy)
{
    mCheckPolicy = policy;
}
