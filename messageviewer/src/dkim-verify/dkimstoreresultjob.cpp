/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimstoreresultjob.h"
#include "dkimresultattribute.h"
#include "messageviewer_dkimcheckerdebug.h"
#include <Akonadi/ItemModifyJob>

using namespace MessageViewer;
DKIMStoreResultJob::DKIMStoreResultJob(QObject *parent)
    : QObject(parent)
{
}

DKIMStoreResultJob::~DKIMStoreResultJob() = default;

void DKIMStoreResultJob::start()
{
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to store dkim result";
        deleteLater();
        return;
    }
    auto attr = mItem.attribute<MessageViewer::DKIMResultAttribute>(Akonadi::Item::AddIfMissing);
    attr->setError(static_cast<int>(mResult.error));
    attr->setWarning(static_cast<int>(mResult.warning));
    attr->setStatus(static_cast<int>(mResult.status));
    auto modify = new Akonadi::ItemModifyJob(mItem);
    modify->setIgnorePayload(true);
    modify->disableRevisionCheck();
    connect(modify, &KJob::result, this, &DKIMStoreResultJob::slotModifyItemDone);
}

void DKIMStoreResultJob::slotModifyItemDone(KJob *job)
{
    if (job->error()) {
        qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << "DKIMStoreResultJob: modify item failed " << job->errorString();
    }
    deleteLater();
}

bool DKIMStoreResultJob::canStart() const
{
    if (mItem.isValid() && mResult.isValid()) {
        return true;
    }
    return false;
}

void DKIMStoreResultJob::setResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    mResult = checkResult;
}

void DKIMStoreResultJob::setItem(const Akonadi::Item &item)
{
    mItem = item;
}

#include "moc_dkimstoreresultjob.cpp"
