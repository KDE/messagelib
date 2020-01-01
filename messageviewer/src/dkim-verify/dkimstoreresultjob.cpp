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

#include "dkimstoreresultjob.h"
#include "dkimresultattribute.h"
#include "messageviewer_dkimcheckerdebug.h"
#include <AkonadiCore/ItemModifyJob>

using namespace MessageViewer;
DKIMStoreResultJob::DKIMStoreResultJob(QObject *parent)
    : QObject(parent)
{
}

DKIMStoreResultJob::~DKIMStoreResultJob()
{
}

void DKIMStoreResultJob::start()
{
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to store dkim result";
        deleteLater();
        return;
    }
    MessageViewer::DKIMResultAttribute *attr = mItem.attribute<MessageViewer::DKIMResultAttribute>(Akonadi::Item::AddIfMissing);
    attr->setError(static_cast<int>(mResult.error));
    attr->setWarning(static_cast<int>(mResult.warning));
    attr->setStatus(static_cast<int>(mResult.status));
    Akonadi::ItemModifyJob *modify = new Akonadi::ItemModifyJob(mItem);
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
