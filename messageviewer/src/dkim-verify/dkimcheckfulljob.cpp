/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimcheckfulljob.h"
#include "dkimauthenticationstatusinfoconverter.h"
#include "dkimcheckauthenticationstatusjob.h"
#include "dkimcheckpolicyjob.h"
#include "dkimgeneraterulejob.h"
#include "dkimmanagerkey.h"
#include "dkimstoreresultjob.h"
#include "messageviewer_dkimcheckerdebug.h"
#include <KLocalizedString>
#include <KMessageBox>
using namespace MessageViewer;

DKIMCheckFullJob::DKIMCheckFullJob(QObject *parent)
    : QObject(parent)
{
}

DKIMCheckFullJob::~DKIMCheckFullJob() = default;

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
        auto job = new DKIMCheckAuthenticationStatusJob(this);
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

void DKIMCheckFullJob::checkSignature(const QList<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> &lst)
{
    auto job = new DKIMCheckSignatureJob(this);
    connect(job, &DKIMCheckSignatureJob::storeKey, this, &DKIMCheckFullJob::storeKey);
    connect(job, &DKIMCheckSignatureJob::result, this, &DKIMCheckFullJob::slotCheckSignatureResult);
    job->setMessage(mMessage);
    job->setHeaderParser(mHeaderParser);
    job->setPolicy(mCheckPolicy);
    job->setCheckSignatureAuthenticationResult(lst);
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
        // Nothing
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
    const MessageViewer::KeyInfo info{key, selector, domain, QDateTime::currentDateTime()};
    if (verify) {
        const QString keyStored = MessageViewer::DKIMManagerKey::self()->keyValue(selector, domain);
        if (!keyStored.isEmpty()) {
            if (keyStored != key) {
                // qDebug() << "storeInKeyManager : keyStored  " << keyStored << " key " << key;
                // qDebug() << "domain " << domain << " selector " << selector;
                const int answer = KMessageBox::warningTwoActions(nullptr,
                                                                  i18n("Stored DKIM key is different from the current one. Do you want to store this one too?"),
                                                                  i18nc("@title:window", "Key Changed"),
                                                                  KGuiItem(i18nc("@action:button", "Store")),
                                                                  KStandardGuiItem::discard());
                if (answer == KMessageBox::ButtonCode::SecondaryAction) {
                    return;
                }
            }
        }
    }
    MessageViewer::DKIMManagerKey::self()->addKey(info);
}

void DKIMCheckFullJob::slotCheckAuthenticationStatusResult(const MessageViewer::DKIMAuthenticationStatusInfo &info)
{
    // qDebug() << "info " << info;
    DKIMAuthenticationStatusInfoConverter converter;
    converter.setStatusInfo(info);
    // TODO Convert to CheckSignatureAuthenticationResult + add this list to CheckSignatureResult directly
    const QList<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> lst = converter.convert();
    // qDebug() << "  lst " << lst;
    // TODO use it.

    // TODO check info ! if auth is ok not necessary to checkSignature
    if (mCheckPolicy.useOnlyAuthenticationResults()) {
        // Don't check signature if not necessary.
    }
    checkSignature(lst);
}

void DKIMCheckFullJob::storeResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    if (mCheckPolicy.saveDkimResult()) {
        if (checkResult.status == DKIMCheckSignatureJob::DKIMStatus::Valid || checkResult.status == DKIMCheckSignatureJob::DKIMStatus::Invalid
            || checkResult.status == DKIMCheckSignatureJob::DKIMStatus::NeedToBeSigned) {
            auto job = new DKIMStoreResultJob(this);
            job->setItem(mAkonadiItem);
            job->setResult(checkResult);
            job->start();
        }
    }
    if (mCheckPolicy.autogenerateRule()) {
        if (mCheckPolicy.autogenerateRuleOnlyIfSenderInSDID()) {
            // TODO
            // FIXME Check value SDID !
            generateRule(checkResult);
        } else {
            generateRule(checkResult);
        }
    }

    qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << "result : status " << checkResult.status << " error : " << checkResult.error << " warning "
                                           << checkResult.warning;
    Q_EMIT result(checkResult, mAkonadiItem.id());
    deleteLater();
}

void DKIMCheckFullJob::generateRule(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    if (checkResult.status == DKIMCheckSignatureJob::DKIMStatus::Valid) {
        auto job = new DKIMGenerateRuleJob(this);
        job->setResult(checkResult);
        if (!job->start()) {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start autogenerate rule";
        }
    }
}

void DKIMCheckFullJob::slotCheckSignatureResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    if (mCheckPolicy.checkIfEmailShouldBeSigned() && (checkResult.status == DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned)) {
        auto job = new DKIMCheckPolicyJob(this);
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

#include "moc_dkimcheckfulljob.cpp"
