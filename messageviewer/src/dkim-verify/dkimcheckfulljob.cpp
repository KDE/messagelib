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

#include "dkimcheckauthenticationstatusjob.h"
#include "dkimcheckfulljob.h"
#include "dkimcheckpolicyjob.h"
#include "dkimchecksignaturejob.h"
#include "dkimgeneraterulejob.h"
#include "dkimmanagerkey.h"
#include "dkimauthenticationstatusinfoconverter.h"
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
    if (!item.isValid()) {
        deleteLater();
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Invalid item";
        return;
    }
    mAkonadiItem = item;
    if (mAkonadiItem.hasPayload<KMime::Message::Ptr>()) {
        mMessage = mAkonadiItem.payload<KMime::Message::Ptr>();
    }
    if (!mMessage) {
        deleteLater();
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Invalid message";
        return;
    }
    checkAuthenticationResults();
}

void DKIMCheckFullJob::checkAuthenticationResults()
{
    if (mCheckPolicy.useAuthenticationResults()) {
        DKIMCheckAuthenticationStatusJob *job = new  DKIMCheckAuthenticationStatusJob(this);
        mHeaderParser.setHead(mMessage->head());
        mHeaderParser.parse();
        job->setHeaderParser(mHeaderParser);
        job->setUseRelaxedParsing(mCheckPolicy.useRelaxedParsing());
        connect(job, &DKIMCheckAuthenticationStatusJob::result, this, &DKIMCheckFullJob::slotCheckAuthenticationStatusResult);
        job->start();
    } else {
        checkSignature();
    }
}

void DKIMCheckFullJob::checkSignature()
{
    DKIMCheckSignatureJob *job = new DKIMCheckSignatureJob(this);
    connect(job, &DKIMCheckSignatureJob::storeKey, this, &DKIMCheckFullJob::storeKey);
    connect(job, &DKIMCheckSignatureJob::result, this, &DKIMCheckFullJob::slotCheckSignatureResult);
    job->setMessage(mMessage);
    job->setHeaderParser(mHeaderParser);
    job->setPolicy(mCheckPolicy);
    job->start();
}

void DKIMCheckFullJob::startCheckFullInfo(const KMime::Message::Ptr &message)
{
    mMessage = message;
    if (!mMessage) {
        deleteLater();
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Invalid message";
        return;
    }
    checkAuthenticationResults();
}

void DKIMCheckFullJob::storeKey(const QString &key, const QString &domain, const QString &selector)
{
    switch (mCheckPolicy.saveKey()) {
    case MessageViewer::MessageViewerSettings::EnumSaveKey::NotSaving:
        //Nothing
        break;
    case MessageViewer::MessageViewerSettings::EnumSaveKey::Save:
        storeInKeyManager(key, selector, domain, false);
        break;
    case MessageViewer::MessageViewerSettings::EnumSaveKey::SaveAndCompare:
        storeInKeyManager(key, selector, domain, true);
        break;
    }
}

void DKIMCheckFullJob::storeInKeyManager(const QString &key, const QString &domain, const QString &selector, bool verify)
{
    const MessageViewer::KeyInfo info {key, selector, domain};
    if (verify) {
        const QString keyStored = MessageViewer::DKIMManagerKey::self()->keyValue(selector, domain);
        if (!keyStored.isEmpty()) {
            if (keyStored != key) {
                if (KMessageBox::No == KMessageBox::warningYesNo(nullptr, i18n("Stored DKIM key is different from the current one. Do you want to store this one too?"), i18n("Key Changed"))) {
                    return;
                }
            }
        }
    }
    MessageViewer::DKIMManagerKey::self()->addKey(info);
}

void DKIMCheckFullJob::slotCheckAuthenticationStatusResult(const MessageViewer::DKIMAuthenticationStatusInfo &info)
{
    //qDebug() << "info " << info;
    DKIMAuthenticationStatusInfoConverter converter;
    converter.setStatusInfo(info);
    //TODO Convert to CheckSignatureAuthenticationResult + add this list to CheckSignatureResult directly
    const QVector<DKIMCheckSignatureJob::CheckSignatureResult> lst = converter.convert();
    for (const DKIMCheckSignatureJob::CheckSignatureResult &result : lst) {
        qDebug() << " lst" << result;
        //TODO verify it.
    }
    //TODO check info ! if auth is ok not necessary to checkSignature
    if (mCheckPolicy.useOnlyAuthenticationResults()) {
        //Don't check signature if not necessary.
    }
    checkSignature();
}

void DKIMCheckFullJob::storeResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    if (mCheckPolicy.saveDkimResult()) {
        if (checkResult.status == DKIMCheckSignatureJob::DKIMStatus::Valid
            || checkResult.status == DKIMCheckSignatureJob::DKIMStatus::Invalid
            || checkResult.status == DKIMCheckSignatureJob::DKIMStatus::NeedToBeSigned) {
            DKIMStoreResultJob *job = new DKIMStoreResultJob(this);
            job->setItem(mAkonadiItem);
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
    Q_EMIT result(checkResult, mAkonadiItem.id());
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
