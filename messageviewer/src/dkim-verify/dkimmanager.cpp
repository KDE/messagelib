/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanager.h"
#include "dkimcheckfulljob.h"
#include "dkimmanagerkey.h"
#include "dkimresultattribute.h"
#include "messageviewer_dkimcheckerdebug.h"
#include <Akonadi/AttributeFactory>

using namespace MessageViewer;
DKIMManager::DKIMManager(QObject *parent)
    : QObject(parent)
{
    // Initialize QCA
    MessageViewer::DKIMManagerKey::self();
    Akonadi::AttributeFactory::registerAttribute<MessageViewer::DKIMResultAttribute>();
}

DKIMManager::~DKIMManager() = default;

DKIMManager *DKIMManager::self()
{
    static DKIMManager s_self;
    return &s_self;
}

void DKIMManager::clearInfoWidget()
{
    Q_EMIT clearInfo();
}

DKIMCheckPolicy DKIMManager::policy() const
{
    return mCheckPolicy;
}

void DKIMManager::setPolicy(const DKIMCheckPolicy &policy)
{
    mCheckPolicy = policy;
}

void DKIMManager::recheckDKim(const Akonadi::Item &item)
{
    checkFullInfo(item);
}

void DKIMManager::checkDKim(const Akonadi::Item &item)
{
    if (mCheckPolicy.saveDkimResult()) {
        if (item.hasAttribute<MessageViewer::DKIMResultAttribute>()) {
            const auto *const attr = item.attribute<MessageViewer::DKIMResultAttribute>();
            if (attr) {
                DKIMCheckSignatureJob::CheckSignatureResult checkResult;
                checkResult.error = static_cast<DKIMCheckSignatureJob::DKIMError>(attr->error());
                checkResult.warning = static_cast<DKIMCheckSignatureJob::DKIMWarning>(attr->warning());
                checkResult.status = static_cast<DKIMCheckSignatureJob::DKIMStatus>(attr->status());
                qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG)
                    << "result : status " << checkResult.status << " error : " << checkResult.error << " warning " << checkResult.warning;
                Q_EMIT result(checkResult, item.id());
                return;
            }
        }
    }
    checkFullInfo(item);
}

void DKIMManager::checkFullInfo(const Akonadi::Item &item)
{
    auto job = new DKIMCheckFullJob(this);
    job->setPolicy(mCheckPolicy);
    connect(job, &DKIMCheckFullJob::result, this, &DKIMManager::result);
    job->startCheckFullInfo(item);
}

void DKIMManager::checkDKim(const KMime::Message::Ptr &message)
{
    auto job = new DKIMCheckFullJob(this);
    job->setPolicy(mCheckPolicy);
    connect(job, &DKIMCheckFullJob::result, this, &DKIMManager::result);
    job->startCheckFullInfo(message);
}

#include "moc_dkimmanager.cpp"
